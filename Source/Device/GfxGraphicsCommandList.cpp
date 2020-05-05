

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
#include "State/GfxPipelineState.h"

#include "Resource/GfxRootSignature.h"
#include "Resource/GfxRenderTarget.h"
#include "Resource/GfxDepthStencil.h"

#include "Shader/GfxShader.h"



using namespace GfxLib;



GraphicsCommandList::GraphicsCommandList()
	:m_pRootSignature( nullptr )
	,m_pDepthStencilState( nullptr )
	,m_pBlendState( nullptr )
	, m_pRasterizerState( nullptr )
	,m_pInputLayout( nullptr )
	,m_pPS( nullptr )
	,m_pVS( nullptr )
	,m_pGS( nullptr )
	,m_pDS( nullptr )
	,m_pHS( nullptr )
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


	OnReset();


	return b;

}


void	GraphicsCommandList::Finalize()
{



	SuperClass::Finalize();
}




/***************************************************************
	@brief	RootSignatureの設定
	@par	[説明]
	@param
*/
void	GraphicsCommandList::SetRootSignature(const RootSignature* sig)
{
	if (m_pRootSignature == sig) {
		return;
	}

	m_pRootSignature = sig;

	if (m_pRootSignature) {

		m_PipelineState.pRootSignature = sig->GetD3DRootSignature();
		m_PipelineStateId.hashRootSignature = sig->GetHashValue();

	}else {
		//	どうする？
	}

	m_bPipelineDirty = true;

	if (sig) {
		m_pCmdList->SetGraphicsRootSignature(sig->GetD3DRootSignature());
	} else {
		m_pCmdList->SetGraphicsRootSignature(nullptr);

	}

}





void	GraphicsCommandList::OMSetDepthStencilState(const DepthStencilState* state)
{
	if (state == m_pDepthStencilState) return;

	m_pDepthStencilState = state;

	if (m_pDepthStencilState) {

		m_bPipelineDirty = true;
		m_PipelineState.DepthStencilState = m_pDepthStencilState->GetDesc();

		//	ハッシュ値
		m_PipelineStateId.hashDepthStencilState = state->GetHashValue();

	}

}



void	GraphicsCommandList::OMSetBlendState(const BlendState* state)
{
	if (state == m_pBlendState) return;

	m_pBlendState = state;

	if (m_pBlendState) {
		
		m_bPipelineDirty = true;
		m_PipelineState.BlendState = m_pBlendState->GetDesc();


		m_PipelineStateId.hashBlendState = m_pBlendState->GetHashValue();

	}

}



void	GraphicsCommandList::RSSetState(const RasterizerState* state)
{
	if (state == m_pRasterizerState) return;

	m_pRasterizerState = state;
	if (m_pRasterizerState) {

		m_bPipelineDirty = true;
		m_PipelineState.RasterizerState = m_pRasterizerState->GetDesc();

		m_PipelineStateId.hashRasterizerState = m_pRasterizerState->GetHashValue();

		
	}


}



void	GraphicsCommandList::IASetInputLayout(const InputLayout* layout)
{
	if (layout == m_pInputLayout) return;

	m_pInputLayout = layout;
	if (m_pInputLayout) {

		m_bPipelineDirty = true;
		m_PipelineState.InputLayout = m_pInputLayout->GetInputLayoutDesc();

		m_PipelineStateId.hashInputLayout = m_pInputLayout->GetHashValue();
	}
	else {

		m_bPipelineDirty = true;
		m_PipelineState.InputLayout = D3D12_INPUT_LAYOUT_DESC{ nullptr , 0 };

		m_PipelineStateId.hashInputLayout = 0;
	}


}




void	GraphicsCommandList::VSSetShader(const VertexShader *vs)
{
	if (m_pVS == vs) {
		return;
	}


	if (m_pVS = vs) {
		m_PipelineState.VS = vs->GetD3D12ShaderBytecode();
		m_PipelineStateId.hashVS = vs->GetHashValue();
	}
	else {
		m_PipelineState.VS = D3D12_SHADER_BYTECODE{ nullptr , 0 };
		m_PipelineStateId.hashVS = 0;
	}

	m_bPipelineDirty = true;

}



void	GraphicsCommandList::PSSetShader(const PixelShader *ps)
{
	if (m_pPS == ps) {
		return;
	}


	if (m_pPS = ps) {
		m_PipelineState.PS = ps->GetD3D12ShaderBytecode();
		m_PipelineStateId.hashPS = ps->GetHashValue();
	}
	else {
		m_PipelineState.PS = D3D12_SHADER_BYTECODE{ nullptr , 0 };
		m_PipelineStateId.hashPS = 0;
	}

	m_bPipelineDirty = true;

}



void	GraphicsCommandList::OMSetRenderTargets(uint32_t count, const RenderTarget* const * rtArray, const DepthStencil * depthStencil)
{

	// 最大D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT

	D3D12_CPU_DESCRIPTOR_HANDLE	aRTVDesc[D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT];

	uint32_t i = 0;
	for (; i < count; ++i) {
		aRTVDesc[i] = rtArray[i]->GetRTVDescriptorHandle();
		m_PipelineState.RTVFormats[i] = rtArray[i]->GetFormat();
		m_PipelineStateId.RTVFormats[i] = rtArray[i]->GetFormat();
	}
	for (; i < _countof(aRTVDesc); ++i) {
		m_PipelineState.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
		m_PipelineStateId.RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
	}

	m_pCmdList->OMSetRenderTargets(count, aRTVDesc, false,
		depthStencil ? &D3D12_CPU_DESCRIPTOR_HANDLE( depthStencil->GetDSVDescriptorHandle() ):nullptr );

	m_PipelineState.NumRenderTargets = count;
	m_PipelineState.DSVFormat = depthStencil ? depthStencil->GetFormat() : DXGI_FORMAT_UNKNOWN;
	m_PipelineStateId.NumRenderTargets = count;
	m_PipelineStateId.DSVFormat = depthStencil ? depthStencil->GetFormat() : DXGI_FORMAT_UNKNOWN;
	m_bPipelineDirty = true;

}






/***************************************************************
@brief	レンダーターゲットのクリア
@par	[説明]
D3DのAPIと同じ
*/
void GraphicsCommandList::ClearRenderTargetView(
	RenderTarget& rt,
	const float ColorRGBA[4],
	uint32_t NumRects,
	const D3D12_RECT *pRects)
{

	m_pCmdList->ClearRenderTargetView(rt.GetRTVDescriptorHandle(), ColorRGBA, NumRects,pRects );


}


/***************************************************************
@brief	デプスステンシルのクリア
@par	[説明]
D3DのAPIと同じ
*/
void GraphicsCommandList::ClearDepthStencilView(
	DepthStencil& ds,
	D3D12_CLEAR_FLAGS ClearFlags,
	float Depth,
	uint8_t Stencil,
	uint32_t NumRects,
	const D3D12_RECT *pRects)
{

	m_pCmdList->ClearDepthStencilView(ds.GetDSVDescriptorHandle(), ClearFlags, Depth, Stencil, NumRects, pRects);

}





void GraphicsCommandList::SetGraphicsRootDescriptorTable(
	uint32_t RootParameterIndex,
	const DescriptorBuffer &descBuffer)
{

	m_pCmdList->SetGraphicsRootDescriptorTable(RootParameterIndex, descBuffer.GetGPUDescriptorHandle());

}



/***************************************************************
@brief	PSOのフラッシュ
@par	[説明]
	PSOのフラッシュを行う
@param
*/
void	GraphicsCommandList::FlushPipeline()
{
	if (!m_bPipelineDirty) {
		return;
	}

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	PipelineState *state = coreSystem->AcquireGraphicsPso(m_PipelineStateId, m_PipelineState);


	GetD3DCommandList()->SetPipelineState(state->GetD3DPipelineState());

	m_bPipelineDirty = false;
}




/***************************************************************
@brief	コマンドリストオブジェクトの再利用時
@par	[説明]
	コマンドリストオブジェクトの再利用時に呼び出される
	内部状態をリセットする
@param
*/
void	GraphicsCommandList::OnReset()
{

	m_pRootSignature = (nullptr);
	m_pDepthStencilState = (nullptr);
	m_pBlendState = (nullptr);
	m_pRasterizerState = (nullptr);
	m_pInputLayout = (nullptr);
	m_pPS = (nullptr);
	m_pVS = (nullptr);
	m_pGS = (nullptr);
	m_pDS = (nullptr);
	m_pHS = (nullptr);
	m_bPipelineDirty = (true);


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

}