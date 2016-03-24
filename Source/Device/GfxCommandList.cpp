



#include "stdafx.h"


#include "GfxCommandList.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;


CommandList::CommandList()
{
}

CommandList::~CommandList()
{
	m_pCmdList.Release();
}


bool	CommandList::Initialize(CoreSystem * coreSystem)
{


	HRESULT hr = coreSystem->GetD3DDevice()->CreateCommandList(
		0,									//	Node Mask  マルチGPU識別用
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		coreSystem->GetCurrentCommandAllocator(),
		nullptr,
		IID_PPV_ARGS(m_pCmdList.InitialAccept())
	);


	if ( FAILED(hr)) {
		
		GFX_ERROR_LOG(L"CreateCommandList error %08x",hr);
		
		return false;

	}


	// おまじない
	m_pCmdList->Close();


	return true;
}




void	CommandList::Reset(CoreSystem * coreSystem)
{

	m_pCmdList->Reset(coreSystem->GetCurrentCommandAllocator(), nullptr /*PSO*/);

}
