/***************************************************************
	@brief			コマンドキュー
	@par	[説明]

		コマンドキューをカプセル化


`***************************************************************/





#include "stdafx.h"


#include "GfxCommandQueue.h"

#include "Device/GfxCommandAllocatorPool.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;






CommandQueue::CommandQueue()
:m_CmdListType(D3D12_COMMAND_LIST_TYPE_DIRECT)
, m_pCmdAllocatorPool(nullptr)
, m_uFenceValue(1)
{


}


CommandQueue::~CommandQueue()
{
	Finalize();
}


bool CommandQueue::Initialize(D3D12_COMMAND_LIST_TYPE type)
{
	Finalize();


	CoreSystem* coreSystem = CoreSystem::GetInstance();
	ID3D12Device *pDevice = coreSystem->GetD3DDevice();



	D3D12_COMMAND_QUEUE_DESC	desc = {};

	desc.Type = type;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HRESULT hr;
	hr = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CmdQueue.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommandQueue Failed error=%08x", hr);
		return false;
	}

	//	汎用のフェンスを作成
	
	hr = pDevice->CreateFence(0,D3D12_FENCE_FLAG_NONE,IID_PPV_ARGS(m_d3dFence.InitialAccept()));
	if (FAILED(hr)) {
		GFX_ERROR(L"CreateFence Failed error=%08x", hr);
		return false;
	}

	m_CmdListType = type;

	m_pCmdAllocatorPool = new CommandAllocatorPool(pDevice, type);

	return true;

}


/***************************************************************
	@brief	コマンドキューのファイナライズ
	@par	[説明]
		コマンドキューを終了する
		GPUの終了待機するため、このメソッドの呼び出しに時間がかかる可能性がある
	@param
*/
void CommandQueue::Finalize()
{

	if (m_CmdQueue != nullptr) {
		Fence fence;
		fence.Initialize(true);

		InsertFence(&fence);

		// GPUの完了を待機
		fence.Sync();
		fence.Finalize();
	}


	m_CmdQueue.Release();
	m_d3dFence.Release();

	delete m_pCmdAllocatorPool;
	m_pCmdAllocatorPool = nullptr;

}




void		CommandQueue::InsertFence(Fence *fence)
{

	// 内部で複数のメソッドが呼ばれるため、Lockしたほうがよい

	fence->_Insert(this);


}



uint64_t	CommandQueue::InsertFence()
{

	return Signal(m_d3dFence);

}



// フェンスを待機しているか
bool		CommandQueue::IsFencePending(uint64_t fenceValue)
{
	GFX_ASSERT( fenceValue < m_uFenceValue , L"Invalid Fence Value" )
	return m_d3dFence->GetCompletedValue() < fenceValue;

}




uint64_t	CommandQueue::Signal(ID3D12Fence *fence)
{

	uint64_t fv = m_uFenceValue;
	//m_CmdQueue->Signal(fence, m_uFenceValue);
	m_CmdQueue->Signal(fence, m_uFenceValue);
	++m_uFenceValue;


	return fv;
}




