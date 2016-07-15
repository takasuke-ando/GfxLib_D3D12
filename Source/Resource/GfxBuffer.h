#ifndef __INCLUDE_GFXBUFFER_H__
#define	__INCLUDE_GFXBUFFER_H__


#include "GfxResource.h"


namespace GfxLib
{

	class Buffer : 
		public Resource
	{

		typedef Resource SuperClass;

	public:
		Buffer();
		~Buffer();



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
			サイズを指定して、初期化を行う
		@param
		*/
		bool Initialize( size_t byteSize );


		uint32_t	GetBufferSize() const { return m_BuffSize; }

		D3D12_GPU_VIRTUAL_ADDRESS	GetGpuVirtualAddress(void) const { return m_pGpuAddress; }
		void*						GetCpuAddress(void) const { return m_pCpuAddress; }

	private:

		D3D12_GPU_VIRTUAL_ADDRESS	m_pGpuAddress;
		void*						m_pCpuAddress;
		uint32_t					m_BuffSize;
	};


}


#endif // !__INCLUDE_GFXVERTEXBUFFER_H__
