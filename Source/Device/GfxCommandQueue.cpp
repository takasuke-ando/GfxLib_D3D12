/***************************************************************
	@brief			コマンドキュー
	@par	[説明]

		コマンドキューをカプセル化


`***************************************************************/





#include "stdafx.h"


#include "GfxCommandQueue.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;






CommandQueue::CommandQueue()
: m_uFenceValue(1)
{


}


CommandQueue::~CommandQueue()
{


}


bool CommandQueue::Initialize()
{
	Finalize();


	CoreSystem* coreSystem = CoreSystem::GetInstance();
	ID3D12Device *pDevice = coreSystem->GetD3DDevice();



	D3D12_COMMAND_QUEUE_DESC	desc = {};

	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HRESULT hr;
	hr = pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(m_CmdQueue.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateCommandQueue Failed error=%08x", hr);
		return false;
	}


	return true;

}


void CommandQueue::Finalize()
{

	m_CmdQueue.Release();


}




void		CommandQueue::InsertFence(Fence *fence)
{

	// 内部で複数のメソッドが呼ばれるため、Lockしたほうがよい

	fence->_Insert(this);


}




uint64_t	CommandQueue::Signal(ID3D12Fence *fence)
{

	uint64_t fv = m_uFenceValue;
	//m_CmdQueue->Signal(fence, m_uFenceValue);
	m_CmdQueue->Signal(fence, m_uFenceValue);
	++m_uFenceValue;


	return fv;
}




