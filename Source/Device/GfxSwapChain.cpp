﻿




#include "stdafx.h"

#include "GfxSwapChain.h"

#include "System/GfxCoreSystem.h"
#include "Device/GfxCommandList.h"
#include "Device/GfxGraphicsCommandList.h"
#include "Util/GfxAutoDescriptorHandle.h"
#include "Resource/GfxRenderTarget.h"



using namespace GfxLib;




SwapChain::SwapChain()
	//:m_paRTDescHandle(nullptr)
	:m_paRenderTargets(nullptr)
	,m_Width(0)
	,m_Height(0)
	,m_nCurrentBackBufferIndex(0)
{
}

SwapChain::~SwapChain()
{
	Finalize();
}


bool	SwapChain::Initialize( HWND hwnd)
{

	CoreSystem *coreSystem = CoreSystem::GetInstance();
	const uint32_t BufferCount = 3;


	RECT crect;
	if (!::GetClientRect(hwnd, &crect)) {

		return false;
	}


	int32_t w = crect.right - crect.left;
	int32_t h = crect.bottom - crect.top;

	if (w < 16)	w = 16;
	if (h < 16) h = 16;

	m_Width = w;
	m_Height = h;

	IDXGIFactory4 *factory = coreSystem->GetDXGIFactory();


	{
		DXGI_SWAP_CHAIN_DESC desc = {};


		desc.BufferCount = BufferCount;
		desc.BufferDesc.Width = w;
		desc.BufferDesc.Height = h;
		desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.OutputWindow = hwnd;
		desc.SampleDesc.Count = 1;
		desc.Windowed = TRUE;

		IDXGISwapChain* swapChain = nullptr;

		HRESULT hr;
		hr = coreSystem->_CreateSwapChain(desc, swapChain);

		if (FAILED(hr)) {
			GFX_ERROR(L"CreateSwapChain Failed %08x",hr);
			return false;
		}

		// SwapChain3に変換
		hr  = swapChain->QueryInterface(IID_PPV_ARGS(m_GISwapChain.InitialAccept()));
		if (FAILED(hr)) {
			GFX_ERROR(L"QueryInterface Failed %08x", hr);
			swapChain->Release();
			return false;
		}


		swapChain->Release();

	}

	//if (!m_RTDescHeap.InitializeRTV(BufferCount)) {

	//	return false;		
	//}

	//m_paRTDescHandle = new AutoDescriptorHandle< DescriptorHeapType::RTV >[BufferCount];

	//m_paRenderTargets = new D3DPtr<ID3D12Resource>[BufferCount];
	m_paRenderTargets = new RenderTarget[BufferCount];


	//	スワップチェーンから、レンダーターゲットを作成する
	for (uint32_t i = 0; i < BufferCount; ++i ) {
		D3DPtr<ID3D12Resource>	  renderTarget;
		HRESULT hr = m_GISwapChain->GetBuffer(i, IID_PPV_ARGS(renderTarget.InitialAccept()));
		//D3D12_CPU_DESCRIPTOR_HANDLE	handle = m_RTDescHeap.GetCPUDescriptorHandleByIndex(i);
		///D3D12_CPU_DESCRIPTOR_HANDLE	handle = m_paRTDescHandle[i];

		if (FAILED(hr)) {
			GFX_ERROR(L"SwapChain3::GetBuffer Failed %08x", hr);

		}
		else {
			// OK!

			//D3D12_RESOURCE_DESC desc = resource->GetDesc();

			//coreSystem->GetD3DDevice()->CreateRenderTargetView(m_paRenderTargets[i], nullptr, handle );
			m_paRenderTargets[i].Initialize(renderTarget);
			
		}
		
	}

		

	m_nCurrentBackBufferIndex = m_GISwapChain->GetCurrentBackBufferIndex();


	return true;
}


void	SwapChain::Finalize()
{
	delete[] m_paRenderTargets;
	m_paRenderTargets = nullptr;
	m_GISwapChain.Release();
	//delete[] m_paRTDescHandle;
	//m_paRTDescHandle = nullptr;
}



void	SwapChain::Begin(GraphicsCommandList& cmdList)
{

	cmdList.ResourceTransitionBarrier(GetCurrentRenderTarget(), ResourceStates::Present, ResourceStates::RenderTarget);


}


void	SwapChain::End(GraphicsCommandList& cmdList)
{

	cmdList.ResourceTransitionBarrier(GetCurrentRenderTarget(), ResourceStates::RenderTarget, ResourceStates::Present);


}


void	SwapChain::Present(uint32_t SyncInterval, uint32_t Flags)
{

	m_GISwapChain->Present(SyncInterval, Flags);
	m_nCurrentBackBufferIndex = m_GISwapChain->GetCurrentBackBufferIndex();
	
}



D3D12_CPU_DESCRIPTOR_HANDLE	SwapChain::GetCurrentRenderTargetHandle() const
{


	//D3D12_CPU_DESCRIPTOR_HANDLE handle = m_paRTDescHandle[m_nCurrentBackBufferIndex];
	//return handle;
	
	return m_paRenderTargets[m_nCurrentBackBufferIndex].GetRTVDescriptorHandle();



}



RenderTarget*				SwapChain::GetCurrentRenderTarget() const
{


	return &m_paRenderTargets[m_nCurrentBackBufferIndex];


}
