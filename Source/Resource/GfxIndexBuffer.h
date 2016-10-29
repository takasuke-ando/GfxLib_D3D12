#ifndef __INCLUDE_GFXINDEXBUFFER_H__
#define	__INCLUDE_GFXINDEXBUFFER_H__


/***************************************************************
	@brief	GfxIndexBuffer
	@par	[説明]
		インデックスバッファをカプセル化
	@param
/****************************************************************/





#include "GfxBuffer.h"


namespace GfxLib
{

	class IndexBuffer:
		public Buffer
	{

		typedef Buffer SuperClass;

	public:
		IndexBuffer();
		~IndexBuffer();



		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
		@brief	初期化
		@par	[説明]
		初期化データを指定して、初期化を行う
		@param
		*/
		bool Initialize(const void* pVBData, size_t byteSize , Format fmt);

		const D3D12_INDEX_BUFFER_VIEW&		GetIndexBufferView() const { return m_idxBuffView; }


	private:
		D3D12_INDEX_BUFFER_VIEW	m_idxBuffView;
	};


}





#endif // !__INCLUDE_GFXINDEXBUFFER_H__
