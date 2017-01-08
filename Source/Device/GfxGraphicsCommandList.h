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


		void	SetDepthStencilState(const DepthStencilState* state);

		void	SetBlendState(const BlendState* state);

		void	SetRasterizerState(const RasterizerState* state);

		void	SetInputLayout(const InputLayout* layout);

		void	SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE topology);

		/***************************************************************
			@brief	レンダーターゲットの設定
			@par	[説明]
				
			@param[in]	count:	レンダーターゲット配列の数
			@param[in]	rtArray:	レンダーターゲット配列
			@param[in]	depthStencil:	デプスステンシル。null可
		*/
		void	OMSetRenderTargets(uint32_t count, const RenderTarget* const * rtArray, const DepthStencil * depthStencil);


	private:
		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;


		//	state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC	m_PipelineState;

		GRAPHICS_PSO_CACHE_IDENTIFIER		m_PipelineStateId;

		//	今のところGetを用意する予定なし
		const DepthStencilState*	m_pDepthStencilState;
		const BlendState*			m_pBlendState;
		const RasterizerState*		m_pRasterizerState;
		const InputLayout*			m_pInputLayout;


		bool			m_bPipelineDirty;



	};








}


#include "GfxGraphicsCommandList.inl"


#endif // !__INCLUDE_GRAPHCIS_COMMAND_LIST_H__
