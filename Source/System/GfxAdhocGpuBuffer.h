#ifndef __INCLUDE_GFXADHOCCONSTANTBUFFER_H__
#define __INCLUDE_GFXADHOCCONSTANTBUFFER_H__

/***************************************************************
@file	GfxAdhocGpuBuffer
@par	[説明]

	フレーム中に一時的に確保される定数・頂点・インデックスバッファを作成する

/***************************************************************/




#include <vector>
#include <deque>

#include "Resource/GfxBuffer.h"
#include <mutex>



namespace GfxLib
{

	class ConstantBuffer;
	class CommandQueue;


	class AdhocGpuBuffer
	{
	public:
		AdhocGpuBuffer();
		~AdhocGpuBuffer();




		/***************************************************************
		@brief	AdhocGpuBufferより呼び出される。Bufferオブジェクトの確保
		@par	[説明]
			次のバッファを要求する。
			このメソッドはマルチスレッドセーフ

		@param[in]	fence:	再利用タイミングを識別するための、現在のフェンス値
		*/
		Buffer*	RequireBuffer(uint64_t fence);


		/***************************************************************
			@brief	バッファの回収を行う
			@par	[説明]
				FenceValueを0にすると、使わなかったバッファということで待機なしでの回収になる
			@param
		*/
		void	ReleaseBuffer(uint64_t FenceValue, Buffer*);



	private:

		
		typedef std::deque< std::pair<uint64_t,GfxLib::Buffer*> >	FenceAndBufferVec;
		FenceAndBufferVec		m_FreeBuffer;					//!<	再利用街のデスクリプタヒープのプール

		uint32_t		m_allocatedCount;

		std::mutex		m_Mutex;


	};



	/***************************************************************
		@brief	GPUバッファ確保クライアント
		@par	[説明]
			スレッドローカルに、このクラスを作り
			バッファ確保を行う
			Bufferクラスの作成はAdhocGpuBufferにゆだね、
			確保したBufferからオフセットでメモリを確保して利用者側に受け渡す
			AdhocGpuBufferへ要求する以外では同期オブジェクトの確保は行わない
		@param
	*/
	class AdhocGpuBufferClient
	{
	public:
		AdhocGpuBufferClient();
		~AdhocGpuBufferClient();



		void	Initialize(AdhocGpuBuffer *);


		/***************************************************************
		@brief	内部状態のリセット
		@par	[説明]
			再利用を可能にするため、
			所持していたバッファの返却などを行います
		@param
		*/
		void	Reset(uint64_t fence);


		/***************************************************************
		@brief	バッファの確保を行います
		@par	[説明]
		このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
		数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
		@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
		@param[in]	fenceOwnQueue:		再利用タイミングを識別するフェンス値を取得するためのキュー
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント

		*/
		D3D12_GPU_VIRTUAL_ADDRESS	Require(void * &cpuAddress, CommandQueue *fenceOwnQueue,  uint32_t size, uint32_t alignment);



	private:

		// immutable 
		AdhocGpuBuffer	*m_pHost;

		// work
		std::vector<Buffer*> m_vecUsingBuffer;
		Buffer			*m_pCurrentBuffer;
		uint32_t		m_nCurrentBufferUsedSize;
	};




}

#endif //__INCLUDE_GFXADHOCCONSTANTBUFFER_H__
