#ifndef __INCLUDE_GFXCORESYSTEM_H__
#define	__INCLUDE_GFXCORESYSTEM_H__


#include "GfxFence.h"
#include "GfxDelayDelete.h"
#include "Device/GfxCommandQueue.h"
#include "System/GfxDefines.h"
#include "Util/GfxUniqIdMaker.h"

#include <mutex>

namespace GfxLib
{

	class Fence;
	class DescriptorAllocator;
	class AdhocDescriptorHeap;
	class AdhocGpuBuffer;
	class DescriptorHeap;
	class CommandList;
	class GraphicsCommandList;
	class PipelineStatePool;
	class PipelineState;

	class CoreSystem
	{

	public:


	public:
		CoreSystem();
		~CoreSystem();




		bool	Initialize();
		void	Finalize();

		void	Update();

		ID3D12Device*			GetD3DDevice()			const { return m_pd3dDev; }
		
		float		GetFps()	const { return m_fFps; }

		// すべての描画処理は、Begin/End内部で呼び出す必要がある
		bool		Begin();
		void		End();


		// 現在のフレームのコマンドアロケータを取得 
		//	Begin/Endブラケット内部で呼び出す必要がある…と言いたいところだが、CommandListの作成時使用するため、いつでも呼び出すことができる
		//ID3D12CommandAllocator*	GetCurrentCommandAllocator() const { return m_aCmdAllocator[m_nCurrentCmdAllocatorIndex];	 }

		IDXGIFactory4*			GetDXGIFactory() const { return m_GIFactory; }
		//ID3D12CommandQueue*		GetCommandQueue() const { return m_CmdQueue; }

		//コマンドキューの取得。自動的作成される、唯一のコマンドキュー
		CommandQueue&			GetCommandQueue() {  return m_CommandQueue; }
		//CommandQueue&			GetCommandQueue() { GFX_ASSERT(m_bInsideBeginEnd==true,L"This fucntion should call inside Begin/End");	 return m_CommandQueue; }

		//デスクリプタアロケータの取得
		DescriptorAllocator*	GetDescriptorAllocator() const { return m_pDescriptorAllocator; }


		// インスタンスを取得する
		static CoreSystem*	GetInstance() { return s_pInstance; }

		//	遅延開放キューを取得
		DelayDelete&		GetDelayDelete() { return	m_DelayDelete; }

		/***************************************************************
			@brief	リソース初期化用コマンドリスト
			@par	[説明]
				リソース初期化時のコピー用コマンドリストを取得
				フレーム最初のBegin時にExechteされる

			@param
		*/
		GraphicsCommandList*		GetResourceInitCommandList()	const {
			return m_pResourceInitCmdList;		
		}


		// *** リソース ***

		// ユニークID作成

		uint32_t	MakeUniqId(const D3D12_BLEND_DESC &desc);
		uint32_t	MakeUniqId(const D3D12_DEPTH_STENCIL_DESC &desc);
		uint32_t	MakeUniqId(const D3D12_RASTERIZER_DESC &desc );
		uint32_t	MakeUniqId_InputLayout(const uint32_t inputLayoutHash);	//	D3D12_INPUT_LAYOUT_DESCにしようか…



		/***************************************************************
		@brief
		@par	[説明]
		@param
		*/
		PipelineState*		AcquireGraphicsPso(const GRAPHICS_PSO_CACHE_IDENTIFIER psoId, const D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc);

		/***************************************************************
		@brief	利用可能なデスクリプタヒープを取得する
		@par	[説明]
		このフレームの間だけ、利用可能なデスクリプタヒープを取得する
		@param[in]	size:		要求サイズ
		@param[out]	startIndex:	ヒープのこのインデックスから書き込める

		*/
		//DescriptorHeap*	RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex);



		/***************************************************************
		@brief	GPU待機
		@par	[説明]
			GPUに投入したキューがすべて完了するまで待機する
		*/
		void	WaitGpuFinish();


		/***************************************************************
		@brief	利用可能なデスクリプタヒープを取得する
		@par	[説明]
		このフレームの間だけ、利用可能なデスクリプタヒープを取得する
		同時に、ハンドルコピーも行う
		@param[in]	size:		要求サイズ
		@param[out]	startIndex:	ヒープのこのインデックスから書き込める
		@param[in]	srcHandle:	コピー元となるハンドルの配列
		*/
		//DescriptorHeap*	RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex , const D3D12_CPU_DESCRIPTOR_HANDLE *srcHandle );


		/***************************************************************
			@brief	GPUバッファを確保
			@par	[説明]
				定数バッファ、頂点バッファなどに利用可能なGPUアドレスの確保
				このフレームでしか利用できない
			@param
		*/
		//D3D12_GPU_VIRTUAL_ADDRESS	AllocateGpuBuffer( void* &cpuBuffer , uint32_t size , uint32_t alignment );


		HRESULT _CreateSwapChain(DXGI_SWAP_CHAIN_DESC& desc ,IDXGISwapChain* &swapChain);


		//AdhocGpuBuffer*		GetAdhocGpuBufferHost() const {
		//	return m_pAdhocGpuBuffer;
		//}

		//AdhocDescriptorHeap* GetAdhocDescriptorHeapHost() const {
		//	return m_pAdhocDescriptorHeap;
		//}

	private:

		//	Immutable
		ID3D12Device5*		m_pd3dDev;

		//D3DPtr<ID3D12CommandQueue>		m_CmdQueue;
		CommandQueue					m_CommandQueue;
		D3DPtr<IDXGIFactory4>			m_GIFactory;
		//D3DPtr<ID3D12CommandAllocator>	m_aCmdAllocator[MAX_FRAME_QUEUE];
		
		D3D_FEATURE_LEVEL	m_featureLevel;
		D3D_DRIVER_TYPE		m_driverType;

		// リソース管理
		DelayDelete			m_DelayDelete;

		DescriptorAllocator*	m_pDescriptorAllocator;
		//AdhocDescriptorHeap*	m_pAdhocDescriptorHeap;
		//AdhocGpuBuffer*			m_pAdhocGpuBuffer;

		PipelineStatePool*	m_pGraphicsPsoPool;
		std::mutex			m_GraphicsPsoMutex;

		//	描画管理
		bool				m_bInsideBeginEnd;
		UINT				m_nUpdateCount;
		UINT				m_nFrameCount;
		LARGE_INTEGER		m_lastFpsUpdateTime;
		float				m_fFps;

		Fence				m_aFence[MAX_FRAME_QUEUE];
		Fence				m_WaitGpuFinishFence;

		uint32_t			m_nCurrentCmdAllocatorIndex;

		GraphicsCommandList*	m_pResourceInitCmdList;		//!<	リソース初期化用コマンドリスト。フレームの最初にFlushされる
		


		// State / Resource
		UniqIdMaker<D3D12_BLEND_DESC>			m_uniqBlendState;
		UniqIdMaker<D3D12_DEPTH_STENCIL_DESC>	m_uniqDepthStencilState;
		UniqIdMaker<D3D12_RASTERIZER_DESC>		m_uniqRastelizerState;
		UniqIdMaker<uint32_t>					m_uniqInputLayout;



		//	Singleton
		static CoreSystem*			s_pInstance;

	};



	D3D12_CPU_DESCRIPTOR_HANDLE		AllocateDescriptorHandle(DescriptorHeapType);
	void							FreeDescriptorHandle(DescriptorHeapType , D3D12_CPU_DESCRIPTOR_HANDLE );



}



#endif // !__INCLUDE_GFXCORESYSTEM_H__
