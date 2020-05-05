#ifndef __INCLUDE_GFXADHOCDESCRIPTORHEAP_H__
#define __INCLUDE_GFXADHOCDESCRIPTORHEAP_H__

/***************************************************************
	@file	GfxAdhocDescriptorHeap.h
	@brief
	@par	[説明]
		一時的なデスクリプタヒープを取得、作成する
		ここで作成されたヒープは数フレーム後に使いまわされる

		取得したヒープには、
		ID3D12Device::CopyDescriptorsSimple などを使って
		コピーを行う

/***************************************************************/



#include "System/GfxDefines.h"


#include <deque>
#include <mutex>


namespace GfxLib
{

	class DescriptorHeap;
	class CommandQueue;

	class AdhocDescriptorHeap
	{
	public:

		AdhocDescriptorHeap( DescriptorHeapType type );
		~AdhocDescriptorHeap();


		/***************************************************************
		@brief	利用可能なデスクリプタヒープを取得する
		@par	[説明]
			AdhocDescriptorHeapClientから呼び出される
		@param[in]	completedFence:	バッファ再利用を行うための、フェンス値
		*/
		DescriptorHeap*	Require(uint64_t completedFence );



		/***************************************************************
		@brief	DescHeap回収を行う
		@par	[説明]
			FenceValueを0にすると、使わなかったバッファということで待機なしでの回収になる
		@param[in]	FenceValue:	使用中かどうかを識別するためのフェンス値
		*/
		void	Release(uint64_t FenceValue, DescriptorHeap* heap);



	private:


		DescriptorHeapType m_heapType;

		typedef std::deque< std::pair< uint64_t, DescriptorHeap* > >	FenceAndDescHeapVec;
		FenceAndDescHeapVec		m_FreeDescHeap;							//!<	未使用のデスクリプタヒープのプール
		uint32_t		m_allocatedCount;

		std::mutex		m_Mutex;

	};



	class AdhocDescriptorHeapClient
	{
	public:

		AdhocDescriptorHeapClient();
		~AdhocDescriptorHeapClient();


		/***************************************************************
		@brief	初期化
		@par	[説明]
		@param
		*/
		void	Initialize(AdhocDescriptorHeap* host);

		/***************************************************************
		@brief	リセット
		@par	[説明]
			再利用する場合、フレームの最初に呼び出す
		@param
		*/
		void Reset(uint64_t fence);



		/***************************************************************
		@brief	利用可能なデスクリプタヒープを取得する
		@par	[説明]
		このフレームの間だけ、利用可能なデスクリプタヒープを取得する
		@param[out]	startIndex:	ヒープのこのインデックスから書き込める
		@param[in]  fenceOwnQueue:	フェンスを識別するためのキュー
		@param[in]	size:		要求サイズ
		@param[in]  offsetCurrent:	現在のポインタを移動する。通常true。falseとすると、reserveのみ行う

		*/
		DescriptorHeap*	Require(uint32_t &startIndex, CommandQueue *fenceQueue,uint32_t size,bool offsetCurrent = true);


	private:
		AdhocDescriptorHeap*	m_pHost;
		DescriptorHeap*			m_pCurrentHeap;
		uint32_t				m_nCurrentHeapUsedSize;

		std::vector<DescriptorHeap*> m_vecUsingBuffer;

	};




}


#endif //__INCLUDE_GFXADHOCDESCRIPTORHEAP_H__