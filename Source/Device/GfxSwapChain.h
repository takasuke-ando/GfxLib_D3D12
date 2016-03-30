#ifndef __INCLUDE_GFXSWAPCHAIN_H__
#define	__INCLUDE_GFXSWAPCHAIN_H__


/*
	スワップチェインをカプセル化
	
*/


#include "Resource/GfxDescriptorHeap.h"


namespace GfxLib
{

	class CoreSystem;
	class CommandList;

	class SwapChain
	{
	public:
		SwapChain();
		~SwapChain();

		bool	Initialize(HWND hwnd );
		void	Finalize();


		/***************************************************************
		@brief		フレームの開始
		@par	[説明]
			レンダーターゲットへの書き込みを行う前に呼び出します
			呼び出しを忘れるとハザード制御が行われず、表示中のバックバッファを描画に使ってしまうことがあります
		@param[in]	cmdList:	コマンドリスト
		*/
		void	Begin(CommandList& cmdList);

		/***************************************************************
		@brief		フレームの終了
		@par	[説明]
			フレームの最後に呼び出します
			通常、この直後にコマンドリストのClose,
			ExecuteCommandList,Presentの順で呼び出します
		@param[in]	cmdList:	コマンドリスト
		*/
		void	End(CommandList& cmdList );

		void	Present(uint32_t SyncInterval, uint32_t Flags);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetCurrentRenderTargetHandle() const;

	private:


		D3DPtr< IDXGISwapChain3 >	m_GISwapChain;
		DescriptorHeap				m_RTDescHeap;		//	Render Target View

		uint32_t	m_nCurrentBackBufferIndex;

	};

	



}





#endif //__INCLUDE_GFXSWAPCHAIN_H__
