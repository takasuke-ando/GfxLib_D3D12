#ifndef __INCLUDE_GFXSWAPCHAIN_H__
#define	__INCLUDE_GFXSWAPCHAIN_H__


/*
	スワップチェインをカプセル化
	
*/


#include "Resource/GfxDescriptorHeap.h"


namespace GfxLib
{

	class CoreSystem;

	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		bool	Initialize(HWND hwnd );
		void	Finalize();

		void	Present(uint32_t SyncInterval, uint32_t Flags);

		D3D12_CPU_DESCRIPTOR_HANDLE	GetCurrentRenderTargetHandle() const;

	private:


		D3DPtr< IDXGISwapChain3 >	m_GISwapChain;
		DescriptorHeap				m_RTDescHeap;		//	Render Target View

		uint32_t	m_nCurrentBackBufferIndex;

	};

	



}





#endif //__INCLUDE_GFXSWAPCHAIN_H__
