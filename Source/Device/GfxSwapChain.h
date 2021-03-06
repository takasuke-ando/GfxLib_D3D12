﻿#ifndef __INCLUDE_GFXSWAPCHAIN_H__
#define	__INCLUDE_GFXSWAPCHAIN_H__


/*
	スワップチェインをカプセル化
	
*/


#include "Resource/GfxDescriptorHeap.h"
#include "System/GfxDefines.h"


namespace GfxLib
{

	class CoreSystem;
	class CommandList;
	class GraphicsCommandList;
	class RenderTarget;

	template< DescriptorHeapType heapType >
	class AutoDescriptorHandle;


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
		void	Begin(GraphicsCommandList& cmdList);

		/***************************************************************
		@brief		フレームの終了
		@par	[説明]
			フレームの最後に呼び出します
			通常、この直後にコマンドリストのClose,
			ExecuteCommandList,Presentの順で呼び出します
		@param[in]	cmdList:	コマンドリスト
		*/
		void	End(GraphicsCommandList& cmdList );

		void	Present(uint32_t SyncInterval, uint32_t Flags);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetCurrentRenderTargetHandle() const;
		RenderTarget*				GetCurrentRenderTarget() const;


		uint32_t		GetWidth() const { return m_Width; }
		uint32_t		GetHeight() const { return m_Height; }

	private:


		D3DPtr< IDXGISwapChain3 >	m_GISwapChain;
		//D3DPtr<ID3D12Resource>*		m_paRenderTargets;	//	Render Target Array
		RenderTarget*				m_paRenderTargets;	//	Render Target Array
		//AutoDescriptorHandle< DescriptorHeapType::RTV >	*m_paRTDescHandle;

		uint32_t		m_Width;
		uint32_t		m_Height;

		uint32_t	m_nCurrentBackBufferIndex;

	};

	



}





#endif //__INCLUDE_GFXSWAPCHAIN_H__
