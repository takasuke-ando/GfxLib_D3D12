#ifndef __INCLUDE_GFXCOMMANDLIST_H__
#define __INCLUDE_GFXCOMMANDLIST_H__


#include "System/GfxDefines.h"
#include "System/GfxAdhocGpuBuffer.h"
#include "System/GfxAdhocDescriptorHeap.h"
#include "Resource/GfxDescriptorBuffer.h"

namespace GfxLib
{
	/*
	
		コマンドリストをカプセル化
		現状グラフィックスパイプのみ
	
	*/
	
	class CoreSystem;
	class CommandQueue;
	class Resource;


	class CommandList
	{
	public:
		CommandList();
		~CommandList();


		/***************************************************************
			@brief	初期化処理
			@par	[説明]
				コマンドリストオブジェクトの構築などを行う
				D3D12ではGraphics-CommandList も、Compute- も、Copy- も
				GraphicsCommandListになっている
			@param
		*/
		bool	Initialize(CommandQueue *cmdQueue , ID3D12GraphicsCommandList *cmdList );
		void	Finalize();

		//	コマンドの書き込みを可能な状態にする。フレームの最初に呼び出す
		void	Reset( bool frameBorder = true );

		/***************************************************************
			@brief	リソースバリアを設定する
			@par	[説明]
				トランジションリソースバリアを設定する
			@param[in]	
		*/
		void	ResourceTransitionBarrier(ID3D12Resource* resource, ResourceStates stateBefore, ResourceStates stateAfter);
		void	ResourceTransitionBarrier(Resource* resource, ResourceStates stateBefore, ResourceStates stateAfter);
		void	ResourceUavBarrier(ID3D12Resource* resource);
		void	ResourceUavBarrier(Resource* resource);

		ID3D12GraphicsCommandList*	GetD3DCommandList()		const {	return m_pCmdList;		}
		ID3D12GraphicsCommandList4* GetD3DCommandList4()	const { return m_pCmdList4;		}



		/***************************************************************
		@brief	バッファの確保を行います
		@par	[説明]
		このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
		数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント
		@return		GpuBufferRange:	GPUバッファリソースサブ領域を示す
		*/
		GpuBufferRange	AllocateGpuBuffer( uint32_t size, uint32_t alignment);



		/***************************************************************
		@brief	デスクリプタバッファの予約
		@par	[説明]
			CBV_SRV_UAV,  および、Sampler用デスクリプタの予約を行う
			0を指定する事も可能（その場合変更しない）
			SetDescriptorHeapsを呼び出すのはこの中からのみ

		@param[in]	cbv_srv_uav_size
		@param[in]	sampler_size
		*/

		bool	ReserveDescriptorBuffers(uint32_t cbv_srv_uav_size, uint32_t sampler_size);




		/***************************************************************
			@brief	デスクリプタバッファの確保
			@par	[説明]
				デスクリプタバッファの確保
				一時的なデスクリプタバッファの確保を行う
				利用者側は、
				DescriptorBufferクラスを経由して、デスクリプタの書き込み、コピーを行う

				AllocateDescriptorBuffer : CBV_SRV_UAV用

			@param[in]	size
		*/
		DescriptorBuffer AllocateDescriptorBuffer(uint32_t size);

		DescriptorBuffer AllocateDescriptorBuffer_Sampler( uint32_t size);


		/***************************************************************
		@brief		アサインされたコマンドキューの取得
		*/
		CommandQueue *		GetAssingedCommandQueue() const {
			return m_pCmdQueue;	
		}

		/***************************************************************
		@brief		アロケータをデタッチする
		*/
		ID3D12CommandAllocator*	DetachAllocator();


		/***************************************************************
			@brief	リソースの初期化コピーを行う
			@par	[説明]
				テクスチャリソースの初期化コピー
			@param
		*/
		bool	InitializeResource(
			ID3D12Resource* dstResource , 
			uint32_t subDataNum,
			const D3D12_SUBRESOURCE_DATA subData[]);

		/***************************************************************
		@brief	リソースの初期化コピーを行う
		@par	[説明]
			バッファーリソースの初期化コピー
		@param
		*/
		bool	InitializeResource(
			ID3D12Resource* dstResource,
			const void *srcData,
			const size_t byteSize );



		/***************************************************************
		@brief	ExecuteCommandListが呼び出される
		@par	[説明]
		ExecuteCommandListが呼び出される直前に呼ばれる
		@param
		*/
		void	PreExecute();


		/***************************************************************
			@brief	ExecuteCommandListが呼び出された
			@par	[説明]
				ExecuteCommandListが呼び出された後に呼ばれる
			@param
		*/
		void	PostExecute(uint64_t fence);


	protected:

		/***************************************************************
			@brief	コマンドリストオブジェクトの再利用時
			@par	[説明]
				コマンドリストオブジェクトの再利用時に呼び出される
				内部状態をリセットする
			@param
		*/
		virtual void	OnReset() {}

	private:

		D3DPtr<ID3D12GraphicsCommandList>			m_pCmdList;
		D3DPtr<ID3D12GraphicsCommandList4>			m_pCmdList4;
		ID3D12Device*						m_pd3dDev;

		CommandQueue *						m_pCmdQueue;		//!<	アサインするコマンドキュー


		AdhocGpuBufferClient				m_GpuBufferAllocator;
		AdhocDescriptorHeapClient			m_DescHeapAllocator;			//!<	CBV_SRV_UAV
		AdhocDescriptorHeapClient			m_DescHeapAllocator_Sampler;	//!<	Sampler


		DescriptorHeap *					m_pLastCbvSrvUavHeap;
		DescriptorHeap *					m_pLastSamplerHeap;

	protected:
		ID3D12CommandAllocator*				m_pCurCmdAllocator;	//!<	現在のコマンドアロケータ


	};




}


#endif // !__INCLUDE_GFXCOMMANDLIST_H__
