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

	class CommandList
	{
	public:
		CommandList();
		~CommandList();


		bool	Initialize(CommandQueue *cmdQueue );
		void	Finalize();

		//	コマンドの書き込みを可能な状態にする。フレームの最初に呼び出す
		void	Reset( bool frameBorder = true );

		/***************************************************************
			@brief	リソースバリアを設定する
			@par	[説明]
				トランジションリソースバリアを設定する
			@param[in]	
		*/
		void	ResourceTransitionBarrier(ID3D12Resource* resource,ResourceStates stateBefore , ResourceStates stateAfter );

		ID3D12GraphicsCommandList*	GetD3DCommandList() const {		return m_pCmdList;		}



		/***************************************************************
		@brief	バッファの確保を行います
		@par	[説明]
		このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
		数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
		@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント

		*/
		D3D12_GPU_VIRTUAL_ADDRESS	AllocateGpuBuffer(void * &cpuAddress, uint32_t size, uint32_t alignment);


		/***************************************************************
			@brief	デスクリプタバッファの確保
			@par	[説明]
				デスクリプタバッファの確保
				一時的なデスクリプタバッファの確保を行う
				利用者側は、
				DescriptorBufferクラスを経由して、デスクリプタの書き込み、コピーを行う
			@param[in]	size
		*/
		DescriptorBuffer AllocateDescriptorBuffer(uint32_t size);



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

	private:

		D3DPtr<ID3D12GraphicsCommandList>	m_pCmdList;
		ID3D12Device*						m_pd3dDev;

		CommandQueue *						m_pCmdQueue;		//!<	アサインするコマンドキュー

		ID3D12CommandAllocator*				m_pCurCmdAllocator;	//!<	現在のコマンドアロケータ

		AdhocGpuBufferClient				m_GpuBufferAllocator;
		AdhocDescriptorHeapClient			m_DescHeapAllocator;

	};




}


#endif // !__INCLUDE_GFXCOMMANDLIST_H__
