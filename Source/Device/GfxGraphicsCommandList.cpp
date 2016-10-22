

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

#include "State/GfxDepthStencilState.h"
#include "State/GfxBlendState.h"
#include "State/GfxRasterizerState.h"
#include "State/GfxInputLayout.h"



using namespace GfxLib;



GraphicsCommandList::GraphicsCommandList()
	:m_pDepthStencilState( nullptr )
	,m_pBlendState( nullptr )
	, m_pRasterizerState( nullptr )
	,m_pInputLayout( nullptr )
	, m_bPipelineDirty( true )
{
	memset(&m_PipelineState, 0, sizeof(m_PipelineState));
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


	{
		//	clear pipeline state

		memset(&m_PipelineState, 0, sizeof(m_PipelineState));

		//	Create Default State
		auto &desc = m_PipelineState;

		//	ラスタライザステートの設定
		D3D12_RASTERIZER_DESC descRS = {};

		descRS.FillMode = D3D12_FILL_MODE_SOLID;
		descRS.CullMode = D3D12_CULL_MODE_NONE;
		descRS.FrontCounterClockwise = FALSE;
		descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		descRS.DepthClipEnable = TRUE;
		descRS.MultisampleEnable = FALSE;
		descRS.AntialiasedLineEnable = FALSE;
		descRS.ForcedSampleCount = 0;
		descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		// レンダーターゲットのブレンド設定
		D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
			FALSE,	FALSE,
			D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
			D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
			D3D12_LOGIC_OP_NOOP,
			D3D12_COLOR_WRITE_ENABLE_ALL,
		};

		// ブレンドステートの設定
		D3D12_BLEND_DESC descBS;
		descBS.AlphaToCoverageEnable = FALSE;
		descBS.IndependentBlendEnable = FALSE;
		for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
			descBS.RenderTarget[i] = descRTBS;
		};


		//desc.InputLayout = { inputElement,_countof(inputElement) };
		//desc.pRootSignature = rootSig.GetD3DRootSignature();
		//desc.VS = vertexshader.GetD3D12ShaderBytecode();	//	TODO
		//desc.PS = pixelshader.GetD3D12ShaderBytecode();
		desc.RasterizerState = descRS;
		desc.BlendState = descBS;
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
		desc.DepthStencilState.StencilEnable = FALSE;
		desc.SampleMask = UINT_MAX;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
		desc.SampleDesc.Count = 1;


	}

	return b;

}


void	GraphicsCommandList::Finalize()
{



	SuperClass::Finalize();
}




void	GraphicsCommandList::SetDepthStencilState(const DepthStencilState* state)
{
	if (state == m_pDepthStencilState) return;

	m_pDepthStencilState = state;

	if (m_pDepthStencilState) {

		m_bPipelineDirty = true;
		m_PipelineState.DepthStencilState = m_pDepthStencilState->GetDesc();

	}

}



void	GraphicsCommandList::SetBlendState(const BlendState* state)
{
	if (state == m_pBlendState) return;

	m_pBlendState = state;

	if (m_pBlendState) {
		
		m_bPipelineDirty = true;
		m_PipelineState.BlendState = m_pBlendState->GetDesc();

	}

}



void	GraphicsCommandList::SetRasterizerState(const RasterizerState* state)
{
	if (state == m_pRasterizerState) return;

	m_pRasterizerState = state;
	if (m_pRasterizerState) {

		m_bPipelineDirty = true;
		m_PipelineState.RasterizerState = m_pRasterizerState->GetDesc();

	}


}



void	GraphicsCommandList::SetInputLayout(const InputLayout* layout)
{
	if (layout == m_pInputLayout) return;

	m_pInputLayout = layout;
	if (m_pInputLayout) {

		m_bPipelineDirty = true;
		m_PipelineState.InputLayout = m_pInputLayout->GetInputLayoutDesc();

	}
	else {

		m_bPipelineDirty = true;
		m_PipelineState.InputLayout = D3D12_INPUT_LAYOUT_DESC{ nullptr , 0 };

	}


}
