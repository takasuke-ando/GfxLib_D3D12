





#include "stdafx.h"

#include "System/GfxCoreSystem.h"
#include "System/GfxFence.h"
#include "System/GfxDescriptorAllocator.h"
#include "System/GfxAdhocDescriptorHeap.h"
#include "Resource/GfxDescriptorHeap.h"
#include "System/GfxAdhocGpuBuffer.h"


using namespace GfxLib;



CoreSystem*	CoreSystem::s_pInstance = nullptr;


CoreSystem::CoreSystem()
	:m_pd3dDev(NULL)
	, m_featureLevel(D3D_FEATURE_LEVEL_11_0)
	, m_driverType(D3D_DRIVER_TYPE_HARDWARE)
	, m_pDescriptorAllocator(nullptr)
	, m_pAdhocDescriptorHeap(nullptr)
	, m_pAdhocGpuBuffer(nullptr)
	, m_bInsideBeginEnd(false)
	, m_nUpdateCount(0)
	, m_nFrameCount(0)
	, m_fFps(0)
	, m_nCurrentCmdAllocatorIndex(0)
{
	QueryPerformanceCounter(&m_lastFpsUpdateTime);
}

CoreSystem::~CoreSystem()
{
	Finalize();
}





void CoreSystem::Finalize()
{

	if (m_CommandQueue.GetD3DCommandQueue() != nullptr) {

		//for (uint32_t i = 0; i < __crt_countof(m_aFence); ++i) {
		//全フェンスの終了を待機
		for (  auto & fence : m_aFence  ){
			fence.Sync();
		}


		Fence fence;
		fence.Initialize(true);

		m_CommandQueue.InsertFence(&fence);

		// GPUの完了を待機
		fence.Sync();

		fence.Finalize();


	}


	delete m_pAdhocDescriptorHeap;
	m_pAdhocDescriptorHeap = nullptr;

	delete m_pAdhocGpuBuffer;
	m_pAdhocGpuBuffer = nullptr;

	//m_CmdQueue.Release();
	m_CommandQueue.Finalize();
	for (uint32_t i = 0; i < __crt_countof(m_aCmdAllocator); ++i) {
		m_aCmdAllocator[i].Release();
	}
	m_GIFactory.Release();

	for (auto & fence : m_aFence) {
		fence.Finalize();
	}

	if (m_pd3dDev) {
		m_DelayDelete.DeleteAll();
	}


	// デスクリプタアロケータ開放。CPUハンドルなので、遅延開放は絶対にない
	delete m_pDescriptorAllocator;
	m_pDescriptorAllocator = nullptr;

	GFX_RELEASE(m_pd3dDev);

	s_pInstance = nullptr;

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
		GFX_ERROR(L"D3D12CreateDevice Failed error=%08x", hr);
		return false;
	}


	// DXGIFactoryを作成


	hr = CreateDXGIFactory1(IID_PPV_ARGS(m_GIFactory.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR(L"CreateDXGIFactory1 Failed error=%08x", hr);

		return false;
	}


	/*
	D3D12_COMMAND_QUEUE_DESC	desc = {};

	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	hr = m_pd3dDev->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CmdQueue.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommandQueue Failed error=%08x", hr);
		return false;
	}
	*/


	for (uint32_t i = 0; i < __crt_countof(m_aCmdAllocator); ++i) {
		hr = m_pd3dDev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_aCmdAllocator[i].InitialAccept()));
		if (FAILED(hr)) {
			GFX_ERROR(L"CreateCommandAllocator Failed error=%08x", hr);
			return false;
		}
	}




	m_nCurrentCmdAllocatorIndex = 0;
	s_pInstance = this;	//	ここでシングルトンを保持しておかないと続く処理でCoreSystemへのアクセスができない

	//	最低限の初期化は完了


	//	内部オブジェクトの初期化

	m_CommandQueue.Initialize();

	//for (uint32_t i = 0; i < __crt_countof(m_aFence); ++i) {
	for ( auto& fence : m_aFence ) {
		fence.Initialize(true);
	}


	m_pDescriptorAllocator = new DescriptorAllocator;
	m_pAdhocDescriptorHeap = new AdhocDescriptorHeap(DescriptorHeapType::CBV_SRV_UAV);
	m_pAdhocGpuBuffer = new AdhocGpuBuffer;


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
	// このフレームのコマンドの最後を識別する
	// 本当はEndで入れたいところだが、コマンドアロケータなど描画時以外にアクセスが必要なものもあるため
	// 前のフレームの描画＆今フレームのUpdateで行われたコマンドに対してフェンスを発行する
	m_CommandQueue.InsertFence(&m_aFence[m_nCurrentCmdAllocatorIndex]);

	++m_nCurrentCmdAllocatorIndex;
	if (m_nCurrentCmdAllocatorIndex >= MAX_FRAME_QUEUE) {
		m_nCurrentCmdAllocatorIndex = 0;
	}


	// コマンドアロケータの再利用の前
	m_aFence[m_nCurrentCmdAllocatorIndex].Sync();

	// GPU待機の直後に行う
	m_DelayDelete.Update();

	m_pAdhocDescriptorHeap->NextFrame();
	m_pAdhocGpuBuffer->NextFrame();

	m_aCmdAllocator[m_nCurrentCmdAllocatorIndex]->Reset();



	m_bInsideBeginEnd = true;

	return true;
}



void		CoreSystem::End()
{

	

	m_bInsideBeginEnd = false;

}



#if 0
/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める

*/
DescriptorHeap*	CoreSystem::RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex)
{

	return m_pAdhocDescriptorHeap->Require(size, startIndex);

}

/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
同時に、ハンドルコピーも行う
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める
@param[in]	srcHandle:	コピー元となるハンドルの配列
*/
DescriptorHeap*	CoreSystem::RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex, const D3D12_CPU_DESCRIPTOR_HANDLE *srcHandle)
{

	DescriptorHeap *heap=  m_pAdhocDescriptorHeap->Require(size, startIndex);
	if (!heap) {
		return heap;
	}

	for (uint32_t i = 0; i < size; ++i) {

		m_pd3dDev->CopyDescriptorsSimple(1,
			heap->GetCPUDescriptorHandleByIndex(startIndex + i),
			srcHandle[i],
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}


	return heap;
}


#endif


#if 0
/***************************************************************
@brief	GPUバッファを確保
@par	[説明]
定数バッファ、頂点バッファなどに利用可能なGPUアドレスの確保
このフレームでしか利用できない
@param
*/
D3D12_GPU_VIRTUAL_ADDRESS	CoreSystem::AllocateGpuBuffer(void* &cpuBuffer, uint32_t size, uint32_t alignment)
{

	return m_pAdhocGpuBuffer->Require(cpuBuffer, size, alignment);

}
#endif



HRESULT CoreSystem::_CreateSwapChain(DXGI_SWAP_CHAIN_DESC& desc, IDXGISwapChain* &swapChain)
{

	return m_GIFactory->CreateSwapChain(
		m_CommandQueue.GetD3DCommandQueue(),	//	複数のSwapChainがある場合、別々のコマンドキューを渡すということも考えられる
		&desc,
		&swapChain
		);

}


D3D12_CPU_DESCRIPTOR_HANDLE		GfxLib::AllocateDescriptorHandle(DescriptorHeapType type )
{

	return GfxLib::CoreSystem::GetInstance()->GetDescriptorAllocator()->Allocate(type);

}


void							GfxLib::FreeDescriptorHandle(DescriptorHeapType type, D3D12_CPU_DESCRIPTOR_HANDLE handle )
{

	GfxLib::CoreSystem::GetInstance()->GetDescriptorAllocator()->Free(type,handle);

}


