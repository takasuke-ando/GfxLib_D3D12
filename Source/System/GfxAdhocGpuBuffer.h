#ifndef __INCLUDE_GFXADHOCCONSTANTBUFFER_H__
#define __INCLUDE_GFXADHOCCONSTANTBUFFER_H__


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
		@param[in]	size:		要求サイズ
		@param[in]	alignment:	アライメント

		*/
		D3D12_GPU_VIRTUAL_ADDRESS	Require( const void *srcData , uint32_t size, uint32_t alignment);


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
		Buffer			*m_pCurrentBuffer;
		uint32_t		m_nCurrentBufferUsedSize;
		uint32_t		m_allocatedCount;


	};




}

#endif //__INCLUDE_GFXADHOCCONSTANTBUFFER_H__
