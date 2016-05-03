#ifndef __INCLUDE_GFXVERTEXBUFFER_H__
#define	__INCLUDE_GFXVERTEXBUFFER_H__


#include "GfxResource.h"


namespace GfxLib
{

	class VertexBuffer : 
		public Resource
	{

		typedef Resource SuperClass;

	public:
		VertexBuffer();
		~VertexBuffer();



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
		bool Initialize(const void* pVBData, size_t strideSize , size_t vtxCount);



	private:
		D3D12_VERTEX_BUFFER_VIEW	m_vtxBuffView;
	};


}


#endif // !__INCLUDE_GFXVERTEXBUFFER_H__
