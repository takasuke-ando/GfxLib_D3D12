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
	class DescriptorBuffer;
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

		void	OMSetDepthStencilState(const DepthStencilState* state);

		void	OMSetBlendState(const BlendState* state);

		void	RSSetState(const RasterizerState* state);

		void	IASetInputLayout(const InputLayout* layout);

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



		/***************************************************************
		@brief	レンダーターゲットのクリア
		@par	[説明]
			D3DのAPIと同じ
		*/
		void ClearRenderTargetView(
			RenderTarget&,
			const float ColorRGBA[4],
			uint32_t NumRects,
			const D3D12_RECT *pRects);


		/***************************************************************
		@brief	デプスステンシルのクリア
		@par	[説明]
			D3DのAPIと同じ
		*/
		void ClearDepthStencilView(
			DepthStencil&,
			D3D12_CLEAR_FLAGS ClearFlags,
			float Depth,
			uint8_t Stencil,
			uint32_t NumRects,
			const D3D12_RECT *pRects);


		/***************************************************************
		@brief	RootParameterを設定する
		@par	[説明]
			D3DのAPIと同じ
		*/
		void SetGraphicsRootDescriptorTable(
			uint32_t RootParameterIndex,
			const DescriptorBuffer &BaseDescriptor);




		void IASetIndexBuffer(
			const D3D12_INDEX_BUFFER_VIEW *pView);

		void IASetVertexBuffers(
			UINT StartSlot,
			UINT NumViews,
			const D3D12_VERTEX_BUFFER_VIEW *pViews);


		void IASetPrimitiveTopology(
			D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology);

		
		/***************************************************************
		@brief	PSOのフラッシュ
		@par	[説明]
			PSOのフラッシュを行う
			描画直前に呼び出す必要がある
		@param
		*/
		void	FlushPipeline();

		/***************************************************************
		@brief	D3Dのラッパ
		@par	[説明]
			
		@param
		*/
		void DrawInstanced(
			uint32_t VertexCountPerInstance,
			uint32_t InstanceCount,
			uint32_t StartVertexLocation,
			uint32_t StartInstanceLocation);

		void DrawIndexedInstanced(
			uint32_t IndexCountPerInstance,
			uint32_t InstanceCount,
			uint32_t StartIndexLocation,
			int32_t BaseVertexLocation,
			uint32_t StartInstanceLocation);



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
		D3DPtr<ID3D12GraphicsCommandList4>	m_pCmdList;


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
