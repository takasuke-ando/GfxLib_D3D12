﻿#ifndef __INCLUDE_GFXCOMMANDQUEUE_H__
#define __INCLUDE_GFXCOMMANDQUEUE_H__


#include "System/GfxDefines.h"

namespace GfxLib
{
	class Fence;
	class CommandAllocatorPool;
	class CommandList;
	class AdhocGpuBuffer;
	class AdhocDescriptorHeap;

	/***************************************************************
		@brief		コマンドキュークラス
		@par	[説明]
			D3D12CommandQueueをカプセル化
			GPUへ一連の描画コマンドを送信するためのインターフェースとなる
			また、フェンスを使用して、CPU/GPU間の同期をとることも可能
		@param
	*/
	class CommandQueue
	{
	public:
		CommandQueue();
		~CommandQueue();


		
		/***************************************************************
			@brief	初期化
			@par	[説明]
				コマンドキューの初期化を行う
				CoreSystemの初期化より後に呼び出す必要がある
			@param
		*/
		bool Initialize(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);


		/***************************************************************
			@brief	コマンドキューのファイナライズ
			@par	[説明]
			コマンドキューを終了する
			GPUの終了待機するため、このメソッドの呼び出しに時間がかかる可能性がある
			@param
		*/
		void Finalize();


		ID3D12CommandQueue*	GetD3DCommandQueue() const { return m_CmdQueue;  }

		/***************************************************************
			@brief	コマンドリストの実行
			@par	[説明]
			@param[in]	count:	cmdListsの配列数
			@param[in]	cmdLists:	コマンドリストのポインタ配列
			@return		コマンド実行を認識するフェンスの値
		*/
		uint64_t				ExecuteCommandLists(uint32_t count, CommandList* cmdLists[]);


		// フェンスを挿入 Fenceオブジェクトを使用して、GPUとの同期をとることが可能
		void					InsertFence(Fence *);

		// フェンスを挿入
		uint64_t				InsertFence();

		// フェンスを待機しているか
		bool					IsFencePending(uint64_t);

		//	コマンドキューにフェンスを登録し、新しい値を返す。1以上の値が返ることは保証される
		uint64_t				Signal(ID3D12Fence *fence);

		//	次にフェンスに書き込まれる予定の値を取得する
		uint64_t				GetNextFenceValue() const { return m_uFenceValue+1; }

		//	最後にフェンスに書き込まれた値を取得する
		uint64_t				GetLastFenceValue() const { return m_uFenceValue; }

		uint64_t				GetCompletedFence() const { return  m_d3dFence->GetCompletedValue(); }

		//! コマンドアロケータを要求する
		ID3D12CommandAllocator*	RequireCommandAllocator();

		//! コマンドアロケータを開放。通常は呼び出すことはない
		void ReleaseCommandAllocator(uint64_t fence , ID3D12CommandAllocator* allocator);


		AdhocGpuBuffer*		GetAdhocGpuBufferHost() const {
			return m_pAdhocGpuBuffer;
		}

		AdhocDescriptorHeap* GetAdhocDescriptorHeapHost(DescriptorHeapType type) const {
			return m_pAdhocDescriptorHeap[(uint32_t)type];
		}

	private:
		D3DPtr<ID3D12CommandQueue>		m_CmdQueue;
		D3D12_COMMAND_LIST_TYPE			m_CmdListType;
		CommandAllocatorPool*			m_pCmdAllocatorPool;
		AdhocGpuBuffer*			m_pAdhocGpuBuffer;
		AdhocDescriptorHeap*	m_pAdhocDescriptorHeap[DescriptorHeapType::NUM_TYPES];


		//	フェンス
		uint64_t				m_uFenceValue;
		D3DPtr<ID3D12Fence>		m_d3dFence;



	};






}










#endif // !__INCLUDE_GFXCOMMANDQUEUE_H__
