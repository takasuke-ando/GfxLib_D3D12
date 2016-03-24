#ifndef __INCLUDE_GFXSWAPCHAIN_H__
#define	__INCLUDE_GFXSWAPCHAIN_H__



namespace GfxLib
{

	class CoreSystem;

	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		bool	Initialize(CoreSystem *coreSystem, HWND hwnd );
		void	Finalize();

	private:


		D3DPtr< IDXGISwapChain3 >	m_GISwapChain;

		uint32_t	m_nCurrentBackBufferIndex;

	};

	



}





#endif //__INCLUDE_GFXSWAPCHAIN_H__
