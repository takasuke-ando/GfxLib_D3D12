





#include "stdafx.h"

#include "System/GfxCoreSystem.h"
#include "System/GfxFence.h"


using namespace GfxLib;





CoreSystem::CoreSystem()
	:m_pd3dDev(NULL)
	, m_featureLevel(D3D_FEATURE_LEVEL_11_0)
	, m_driverType(D3D_DRIVER_TYPE_HARDWARE)
	, m_nUpdateCount(0)
	, m_nFrameCount(0)
	, m_fFps(0)
	, m_nCurrentCmdAllocatorIndex(0)
	, m_uFenceValue(1)
{
	QueryPerformanceCounter(&m_lastFpsUpdateTime);
}

CoreSystem::~CoreSystem()
{
	Finalize();
}





void CoreSystem::Finalize()
{

	if (m_CmdQueue != nullptr) {

		//for (uint32_t i = 0; i < __crt_countof(m_aFence); ++i) {
		//全フェンスの終了を待機
		for (  auto & fence : m_aFence  ){
			fence.Sync();
		}


		Fence fence;
		fence.Initialize(this, false);

		InsertFence(&fence);

		// GPUの完了を待機
		fence.Sync();

		fence.Finalize();
	}


	m_CmdQueue.Release();
	for (uint32_t i = 0; i < __crt_countof(m_aCmdAllocator); ++i) {
		m_aCmdAllocator[i].Release();
	}
	m_GIFactory.Release();

	for (auto & fence : m_aFence) {
		fence.Finalize();
	}

	GFX_RELEASE(m_pd3dDev);

}



bool	CoreSystem::Initialize()
{

#ifdef _DEBUG
	{
		//	DebugLayer の有効化
		ID3D12Debug* debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
			debugController->Release();
		}
	}
#endif //_DEBUG




	HRESULT hr;

	// Deviceの作成

	hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_pd3dDev));


	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"D3D12CreateDevice Failed error=%08x", hr);
		return false;
	}


	// DXGIFactoryを作成


	hr = CreateDXGIFactory1(IID_PPV_ARGS(m_GIFactory.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateDXGIFactory1 Failed error=%08x", hr);

		return false;
	}


	D3D12_COMMAND_QUEUE_DESC	desc = {};

	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = m_pd3dDev->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CmdQueue.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateCommandQueue Failed error=%08x", hr);
		return false;
	}


	for (uint32_t i = 0; i < __crt_countof(m_aCmdAllocator); ++i) {
		hr = m_pd3dDev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_aCmdAllocator[i].InitialAccept()));
		if (FAILED(hr)) {
			GFX_ERROR_LOG(L"CreateCommandAllocator Failed error=%08x", hr);
			return false;
		}
	}


	for (uint32_t i = 0; i < __crt_countof(m_aFence); ++i) {
		m_aFence[i].Initialize(this, false);
	}



	m_nCurrentCmdAllocatorIndex = 0;

	return true;


}



/*

	毎フレーム呼び出してください

*/
void	CoreSystem::Update()
{
	++m_nUpdateCount;
	++m_nFrameCount;


	LARGE_INTEGER current;
	QueryPerformanceCounter(&current);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);


	if (current.QuadPart - m_lastFpsUpdateTime.QuadPart > freq.QuadPart)
	{

		double  period = (double)(current.QuadPart - m_lastFpsUpdateTime.QuadPart) / (double)freq.QuadPart;


		m_fFps = float(m_nFrameCount / period);


		m_lastFpsUpdateTime = current;
		m_nFrameCount = 0;

	}

}




bool		CoreSystem::Begin()
{
	++m_nCurrentCmdAllocatorIndex;
	if (m_nCurrentCmdAllocatorIndex >= MAX_FRAME_QUEUE) {
		m_nCurrentCmdAllocatorIndex = 0;
	}


	// コマンドアロケータの再利用の前
	m_aFence[m_nCurrentCmdAllocatorIndex].Sync();

	m_aCmdAllocator[m_nCurrentCmdAllocatorIndex]->Reset();


	return true;
}



void		CoreSystem::End()
{

	// このフレームのコマンドの最後を識別する
	InsertFence( &m_aFence[m_nCurrentCmdAllocatorIndex] );
	

}



void		CoreSystem::InsertFence(Fence *fence )
{

	// 内部で複数のメソッドが呼ばれるため、Lockしたほうがよい

	fence->_Insert(this);


}




uint64_t	CoreSystem::Signal(ID3D12Fence *fence)
{

	uint64_t fv = m_uFenceValue;
	m_CmdQueue->Signal(fence, m_uFenceValue);
	++m_uFenceValue;

	
	return fv;
}

