#ifndef __INCLUDE_GFXADHOCCONSTANTBUFFER_H__
#define __INCLUDE_GFXADHOCCONSTANTBUFFER_H__

/***************************************************************
@file	GfxAdhocGpuBuffer
@par	[説明]

	フレーム中に一時的に確保される定数・頂点・インデックスバッファを作成する

/***************************************************************/




#include <vector>

#include "Resource/GfxBuffer.h"

namespace GfxLib
{

	class ConstantBuffer;



	class AdhocGpuBuffer
	{
	public:
		AdhocGpuBuffer();
		~AdhocGpuBuffer();



		/***************************************************************
		@brief	バッファの確保を行います
		@par	[説明]
			このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
			数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
		@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント

		*/
		//D3D12_GPU_VIRTUAL_ADDRESS	Require( void * &cpuAddress , uint32_t size, uint32_t alignment);


		/***************************************************************
		@brief	AdhocGpuBufferより呼び出される。Bufferオブジェクトの確保
		@par	[説明]
		@param
		*/
		Buffer*	RequireBuffer();

		/***************************************************************
		@brief	次のフレーム
		@par	[説明]
		@param
		*/
		void NextFrame();


	private:

		
		typedef std::vector< GfxLib::Buffer* >	BufferVec;
		BufferVec		m_aUsingBuffer[MAX_FRAME_QUEUE];		//!<	使用中のデスクリプタヒープのベクタ
		BufferVec		m_FreeBuffer;							//!<	未使用のデスクリプタヒープのプール
		uint32_t		m_nCurrentIndex;
		//Buffer			*m_pCurrentBuffer;
		//uint32_t		m_nCurrentBufferUsedSize;
		uint32_t		m_allocatedCount;


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
		@brief	フレームの最初に呼び出す
		@par	[説明]
		@param
		*/
		void	Reset();


		/***************************************************************
		@brief	バッファの確保を行います
		@par	[説明]
		このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
		数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
		@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント

		*/
		D3D12_GPU_VIRTUAL_ADDRESS	Require(void * &cpuAddress, uint32_t size, uint32_t alignment);



	private:
		AdhocGpuBuffer	*m_pHost;
		Buffer			*m_pCurrentBuffer;
		uint32_t		m_nCurrentBufferUsedSize;
	};




}

#endif //__INCLUDE_GFXADHOCCONSTANTBUFFER_H__
