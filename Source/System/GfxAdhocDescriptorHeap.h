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



namespace GfxLib
{

	class DescriptorHeap;


	class AdhocDescriptorHeap
	{
	public:

		AdhocDescriptorHeap( DescriptorHeapType type );
		~AdhocDescriptorHeap();


		/***************************************************************
			@brief	利用可能なデスクリプタヒープを取得する
			@par	[説明]
				このフレームの間だけ、利用可能なデスクリプタヒープを取得する
			@param[in]	size:		要求サイズ
			@param[out]	startIndex:	ヒープのこのインデックスから書き込める
			
		*/
		DescriptorHeap*	Require( uint32_t size , uint32_t &startIndex );
		

		/***************************************************************
		@brief	次のフレーム
		@par	[説明]
		@param
		*/
		void NextFrame();

	private:


		DescriptorHeapType m_heapType;

		typedef std::vector< DescriptorHeap* >	DescHeapVec;
		DescHeapVec		m_aUsingDescHeap[MAX_FRAME_QUEUE];		//!<	使用中のデスクリプタヒープのベクタ
		DescHeapVec		m_FreeDescHeap;							//!<	未使用のデスクリプタヒープのプール
		uint32_t		m_nCurrentIndex;
		DescriptorHeap	*m_pCurrentHeap;
		uint32_t		m_nCurrentHeapUsedSize;
		uint32_t		m_allocatedCount;


	};



}


#endif //__INCLUDE_GFXADHOCDESCRIPTORHEAP_H__