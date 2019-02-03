#ifndef __INCLUDE_GRAPHCIS_COMMAND_LIST_H__
#define __INCLUDE_GRAPHCIS_COMMAND_LIST_H__

/***************************************************************
	@file	GfxGraphicsCommandList.h
	@brief
	@par	[説明]

		CommandListクラスの派生クラス
		ID3D12GraphicsCommandListをカプセル化する

		各種カプセル化やパイプラインステートの生成


		今のところステートのシャドウキャッシュは行わない


***************************************************************/


#include "Device/GfxCommandList.h"
#include "System/GfxDefines.h"


namespace GfxLib
{
	class CommandQueue;
	class DepthStencilState;
	class BlendState;
	class RasterizerState;
	class InputLayout;
	class RenderTarget;
	class DepthStencil;
	class RootSignature;
	class Shader;
	typedef Shader		PixelShader;
	typedef Shader		VertexShader;
	typedef Shader		GeometryShader;
	typedef Shader		DomainShader;
	typedef Shader		HullShader;

	class GraphicsCommandList : 
		public CommandList
	{

		typedef CommandList SuperClass;


	public:
		GraphicsCommandList();
		~GraphicsCommandList();


		bool	Initialize(CommandQueue *cmdQueue);
		void	Finalize();

		ID3D12GraphicsCommandList*	GetD3DCommandList() const { return m_pCmdList; }


		//	-----	State Setting -----
		//	この辺はゆくゆくinline化

		void	SetRootSignature(const RootSignature* sig);

		void	SetDepthStencilState(const DepthStencilState* state);

		void	SetBlendState(const BlendState* state);

		void	SetRasterizerState(const RasterizerState* state);

		void	SetInputLayout(const InputLayout* layout);

		void	SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);


		/***************************************************************
			@brief	シェーダ設定
			@par	[説明]
			@param
		*/
		void	VSSetShader(const VertexShader *vs);

		void	PSSetShader(const PixelShader *ps);


		/***************************************************************
			@brief	レンダーターゲットの設定
			@par	[説明]
				
			@param[in]	count:	レンダーターゲット配列の数
			@param[in]	rtArray:	レンダーターゲット配列
			@param[in]	depthStencil:	デプスステンシル。null可
		*/
		void	OMSetRenderTargets(uint32_t count, const RenderTarget* const * rtArray, const DepthStencil * depthStencil);


	protected:

		/***************************************************************
		@brief	コマンドリストオブジェクトの再利用時
		@par	[説明]
		コマンドリストオブジェクトの再利用時に呼び出される
		内部状態をリセットする
		@param
		*/
		virtual void	OnReset();

	private:
		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;


		//	state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_PipelineState;

		GRAPHICS_PSO_CACHE_IDENTIFIER		m_PipelineStateId;

		//	今のところGetを用意する予定なし

		const RootSignature*		m_pRootSignature;

		const DepthStencilState*	m_pDepthStencilState;
		const BlendState*			m_pBlendState;
		const RasterizerState*		m_pRasterizerState;
		const InputLayout*			m_pInputLayout;

		const VertexShader*			m_pVS;
		const PixelShader*			m_pPS;

		const GeometryShader*		m_pGS;
		const DomainShader*			m_pDS;
		const HullShader*			m_pHS;


		bool			m_bPipelineDirty;



	};








}


#include "GfxGraphicsCommandList.inl"


#endif // !__INCLUDE_GRAPHCIS_COMMAND_LIST_H__
