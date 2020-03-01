#pragma		once
#include <Util\GfxD3DPtr.h>
#include <Shader\GfxShader.h>
#include <Resource\GfxVertexBuffer.h>
#include <Resource\GfxSampler.h>
#include <State\GfxBlendState.h>
#include <State\GfxInputLayout.h>
#include <State\GfxDepthStencilState.h>
#include <State\GfxRasterizerState.h>




namespace GfxLib
{


	struct FontVertex
	{
		DirectX::XMFLOAT3	vPosition;
		DirectX::XMFLOAT4	vColor;
		DirectX::XMFLOAT2	vTexCoord;
	};

	class CommandList;
	class GraphicsCommandList;

	class	FontSystem
	{

	public:
		FontSystem();
		~FontSystem();

		BOOL	Initialize();
		void	Finalize();


		//	フォント書き込み用動的頂点バッファの取得
		//VertexBuffer&	GetFontVertexBuffer()		{		return m_VtxBuff;	}


		void			SetupStates(GraphicsCommandList &ctx );

		VertexShader&	GetVertexShader()	{	return m_VertexShader;	}
		PixelShader&	GetPixelShader()	{	return m_PixelShader;	}
		BlendState&		GetBlendState()		{	return m_BlendState;	}	//!<	通常のブレンドステート
		InputLayout&	GetInputLayout()	{	return m_InputLayout;	}	//!<	入力レイアウト


	private:


		VertexShader			m_VertexShader;
		PixelShader				m_PixelShader;
		BlendState				m_BlendState;		//!<	通常のブレンドステート
		DepthStencilState		m_DepthStencilState;
		RasterizerState			m_RasterState;
		InputLayout				m_InputLayout;		//!<	入力レイアウト
		Sampler					m_Sampler;

		//VertexBuffer			m_VtxBuff;			//!<	フォント書き込み用動的頂点バッファ

	};




}


