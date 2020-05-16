/*


	フォントシステムを初期化する

	・ステートオブジェクトの作成
	・各種シェーダの読み込み
	などを行う


*/


#include "stdafx.h"

#include "GfxFontSystem.h"

#include <Device\GfxGraphicsCommandList.h>
#include <System\GfxCoreSystem.h>



namespace GfxLib
{

	UINT	FontBuffSize = sizeof( FontVertex ) * 6 * 128;


FontSystem::FontSystem()
{

}


FontSystem::~FontSystem()
{

	Finalize();
	
}



/*


	pDevice:	D3Dデバイス


*/
BOOL	FontSystem::Initialize( )
{
	Finalize();


	ID3D12Device* pDevice = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();

	{
		//	通常のブレンドステートを取得する
		D3D12_BLEND_DESC desc = BlendState::GetDefaultDesc();

		//*
		desc.RenderTarget[0].BlendEnable = TRUE;
		desc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		//*/

		m_BlendState.Initialize( desc );
	}

	{
		D3D12_DEPTH_STENCIL_DESC desc = DepthStencilState::GetDefaultDesc();
		desc.DepthEnable = FALSE;

		m_DepthStencilState.Initialize( desc );
	}

	{
		D3D12_RASTERIZER_DESC desc = RasterizerState::GetDefaultDesc();
		desc.CullMode = D3D12_CULL_MODE_NONE;
		m_RasterState.Initialize( desc );
	}

	{
		D3D12_SAMPLER_DESC desc = Sampler::GetDefaultDesc();
		m_Sampler.Initialize( desc );
	}


	if ( !m_VertexShader.CreateFromFile(L"../x64/debug/FontVS.cso" ) )
	{
		return FALSE;
	}
	if ( !m_PixelShader.CreateFromFile(L"../x64/debug/FontPS.cso") )
	{
		return FALSE;
	}


	// Define the input layout
	D3D12_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR"   , 0 , DXGI_FORMAT_R32G32B32A32_FLOAT , 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );

	if ( !m_InputLayout.Initialize(numElements,layout ) )
	{
		return FALSE;
	}

	/*
	if ( !m_VtxBuff.InitializeDynamic( pDevice , FontBuffSize ) )
	{
		GFX_ASSERT( 0 , "Font Buffer Allocation Failed!" );
		return FALSE;
	}
	*/


	//m_pD3DDevice=  pDevice;


	return TRUE;


}



void	FontSystem::Finalize()
{

	m_VertexShader.Finalize();
	m_PixelShader.Finalize();
	m_InputLayout.Finalize();
	m_DepthStencilState.Finalize();
	m_BlendState.Finalize();
	m_Sampler.Finalize();
}




/*

	デバイスステートの設定を行う
	DeviceContextが当該スレッドに所有権がある限り
	このメソッドはスレッドセーフである


*/

void	FontSystem::SetupStates(GraphicsCommandList &ctx )
{

	FLOAT BlendFactor[4] = { 1.f,1.f,1.f,1.f };
	ctx.PSSetShader( &m_PixelShader );
	//ctx.PSSetSampler( 0 , &m_SamplerState );
	ctx.VSSetShader( &m_VertexShader );
	ctx.IASetInputLayout( &m_InputLayout );
	ctx.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);

	/*
	UINT stride = sizeof( FontVertex );
	UINT offset = 0;
	ID3D11Buffer *pBuffer = m_VtxBuff; 
	ctx.IASetVertexBuffers( 0 , 1 , &pBuffer , &stride, &offset  );
	*/

	//test
  	ctx.OMSetBlendState( &m_BlendState );
  	ctx.OMSetDepthStencilState( &m_DepthStencilState );
 	ctx.RSSetState( &m_RasterState );

}








}
