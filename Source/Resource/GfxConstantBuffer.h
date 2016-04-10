#ifndef __INCLUDE_GFXCONSTANTBUFFER_H__
#define	__INCLUDE_GFXCONSTANTBUFFER_H__


#include "GfxDescriptorHeap.h"


namespace GfxLib
{

	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();


		bool	Initialize(uint32_t byteSize);

		void	SetData(const void*pData, uint32_t byteSize);


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
								GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize( bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE );


	private:

		D3DPtr<ID3D12Resource>		m_d3dRes;
		DescriptorHeap				m_descHeap;

		uint32_t					m_byteSize;
		void*						m_pMappedAddr;		//	GPU書き込み先アドレス

	};


}






#endif // !__INCLUDE_GFXCONSTANTBUFFER_H__
