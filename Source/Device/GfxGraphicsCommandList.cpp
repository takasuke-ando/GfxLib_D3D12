

/***************************************************************
@file	GfxGraphicsCommandList.h
@brief
@par	[説明]

CommandListクラスの派生クラス
ID3D12GraphicsCommandListをカプセル化する


***************************************************************/










#include "stdafx.h"


#include "Device/GfxGraphicsCommandList.h"
#include "Device/GfxCommandQueue.h"

#include "System/GfxCoreSystem.h"



using namespace GfxLib;



GraphicsCommandList::GraphicsCommandList()
{
}

GraphicsCommandList::~GraphicsCommandList()
{
	GraphicsCommandList::Finalize();
}



bool	GraphicsCommandList::Initialize(CommandQueue *cmdQueue)
{


	CoreSystem *coreSystem = CoreSystem::GetInstance();

	HRESULT hr = coreSystem->GetD3DDevice()->CreateCommandList(
		0,									//	Node Mask  マルチGPU識別用
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		//coreSystem->GetCurrentCommandAllocator(),
		m_pCurCmdAllocator = cmdQueue->RequireCommandAllocator(),
		nullptr,
		IID_PPV_ARGS(m_pCmdList.InitialAccept())
		);


	if (FAILED(hr)) {

		GFX_ERROR(L"CreateCommandList error %08x", hr);

		return false;

	}


	bool b = SuperClass::Initialize(cmdQueue, m_pCmdList);


	return b;

}


void	GraphicsCommandList::Finalize()
{



	SuperClass::Finalize();
}
