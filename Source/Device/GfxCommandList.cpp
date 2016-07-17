



#include "stdafx.h"


#include "GfxCommandList.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;


CommandList::CommandList()
{
}

CommandList::~CommandList()
{
	Finalize();
}


bool	CommandList::Initialize()
{

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	HRESULT hr = coreSystem->GetD3DDevice()->CreateCommandList(
		0,									//	Node Mask  マルチGPU識別用
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		coreSystem->GetCurrentCommandAllocator(),
		nullptr,
		IID_PPV_ARGS(m_pCmdList.InitialAccept())
	);


	if ( FAILED(hr)) {
		
		GFX_ERROR(L"CreateCommandList error %08x",hr);
		
		return false;

	}

	m_GpuBufferAllocator.Initialize(coreSystem->GetAdhocGpuBufferHost());

	// おまじない
	m_pCmdList->Close();


	return true;
}



void	CommandList::Finalize()
{

	m_pCmdList.Release();


}





void	CommandList::Reset()
{

	CoreSystem * coreSystem = CoreSystem::GetInstance();

	m_pCmdList->Reset(coreSystem->GetCurrentCommandAllocator(), nullptr /*PSO*/);
	m_GpuBufferAllocator.Reset();

}



//	リソースバリアを設定する
void	CommandList::ResourceTransitionBarrier(ID3D12Resource* res , ResourceStates stateBefore, ResourceStates stateAfter )
{

	D3D12_RESOURCE_BARRIER barrier = {};

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = res;
	barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)stateBefore;
	barrier.Transition.StateAfter  = (D3D12_RESOURCE_STATES)stateAfter;

	m_pCmdList->ResourceBarrier(1, &barrier);

}



/***************************************************************
@brief	バッファの確保を行います
@par	[説明]
このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
@param[in]	size:		要求サイズ
@param[in]	alignment:	アライメント

*/
D3D12_GPU_VIRTUAL_ADDRESS	CommandList::AllocateGpuBuffer(void * &cpuAddress, uint32_t size, uint32_t alignment)
{

	return m_GpuBufferAllocator.Require(cpuAddress, size, alignment);

}


