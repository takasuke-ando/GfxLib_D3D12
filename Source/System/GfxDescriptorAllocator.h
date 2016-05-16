#ifndef __INCLUDE_GFXDESCRIPTORALLOCATOR_H__
#define	__INCLUDE_GFXDESCRIPTORALLOCATOR_H__

#include "System/GfxDefines.h"


namespace GfxLib
{



	/***************************************************************
		@brief	デスクリプタアロケータ
		@par	[説明]
			デスクリプタアロケータ
			デスクリプタをメモリ確保、開放して作成できる
			一般的なアロケータの様に使える

			タイプは作成時に決めないといけない
			どのタイプであっても、CPUリードのみとして作成される

			CBV_SRV_UAV , SAMPLERは、GPU読み取り可能なように、
			別のデスクリプタヒープにコピーして使用しないといけない
	*/


	class DescriptorAllocator
	{
	public:
		DescriptorAllocator();
		~DescriptorAllocator();



		/***************************************************************
			@brief	確保	
			@par	[説明]
				確保を行う
				後ほど、Freeを呼び出す必要がある
		*/
		D3D12_CPU_DESCRIPTOR_HANDLE		Allocate();

		/***************************************************************
			@brief	開放
			@par	[説明]
				確保されたデスクリプタを開放する
				countパラメータは、Allocate呼び出し時と同じ値を指定する事
			@param	handle:	ハンドルパラメータ
		*/
		void							Free( D3D12_CPU_DESCRIPTOR_HANDLE handle );


	private:

		class Chunk
		{
		public:
			Chunk();
			~Chunk();

			/***************************************************************
				@brief	初期化
				@par	[説明]
					チャンクの初期化を行う

				@param
			*/
			bool	Initialize(ID3D12Device* , DescriptorHeapType , uint32_t size );


			D3D12_CPU_DESCRIPTOR_HANDLE		Allocate();
			void							Free(D3D12_CPU_DESCRIPTOR_HANDLE handle);

			typedef uint16_t	IndexType;
			enum {
				INVALID_INDEX = 0xFFFF,
			};
		private:
			ID3D12Device*	m_pDevice;
			D3DPtr<ID3D12DescriptorHeap>	m_descriptorHeap;
			DescriptorHeapType	m_Type;
			uint32_t			m_IncSize;		//!<	ハンドルサイズ
			uint32_t			m_Count;		//!<	ハンドル数

			D3D12_CPU_DESCRIPTOR_HANDLE	m_HandleFirst;	//	ハンドルレンジの最初
			D3D12_CPU_DESCRIPTOR_HANDLE	m_HandleLast;	//	ハンドルレンジの最後 （ "<" ではなく、"<=" ）


			//uint64_t			*m_paUseMask;	//!<	ハンドルブロックの、使用中マスク
			IndexType				*m_paFreeIndexChain;	//!<	フリーインデックスを格納するチェイン
			IndexType				m_nFirstFreeIndex;		//!<	空いている最初のインデックス

		};


		ID3D12Device*	m_pDevice;
		Chunk*			m_pFirstChunk;
		DescriptorHeapType	m_Type;

	};




}


#endif // !__INCLUDE_GFXDESCRIPTORALLOCATOR_H__
