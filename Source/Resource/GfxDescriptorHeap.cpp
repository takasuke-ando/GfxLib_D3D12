




#include "stdafx.h"
#include "GfxDescriptorHeap.h"


#include "System/GfxCoreSystem.h"


using namespace GfxLib;




DescriptorHeap::DescriptorHeap()
	:m_descriptorSize(0)
	, m_maxBufferCount(0)
{
}

DescriptorHeap::~DescriptorHeap()
{
	Finalize();
}




/*

	RTV として初期化する

*/
bool	DescriptorHeap::InitializeRTV( uint32_t bufferCount)
{
	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = bufferCount;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	
	HRESULT hr;


	hr = coreSystem->GetD3DDevice()->CreateDescriptorHeap(&desc , IID_PPV_ARGS(m_descriptorHeap.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateDescriptorHeap Failed %08x", hr);
		return false;
	}

	m_maxBufferCount = bufferCount;

	// デスクリプタサイズを取得
	m_descriptorSize = coreSystem->GetD3DDevice()->GetDescriptorHandleIncrementSize(desc.Type);



	return true;
}



/*

	DSV として初期化する

*/
bool	DescriptorHeap::InitializeDSV(uint32_t bufferCount)
{
	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = bufferCount;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


	HRESULT hr;


	hr = coreSystem->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateDescriptorHeap Failed %08x", hr);
		return false;
	}

	m_maxBufferCount = bufferCount;

	// デスクリプタサイズを取得
	m_descriptorSize = coreSystem->GetD3DDevice()->GetDescriptorHandleIncrementSize(desc.Type);



	return true;
}





bool DescriptorHeap::InitializeCBV_SRV_UAV(uint32_t bufferCount)
{

	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = bufferCount;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;	//	コピー専用なら、これをつける必要はない


	HRESULT hr;

	hr = coreSystem->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateDescriptorHeap Failed %08x", hr);
		return false;
	}

	m_maxBufferCount = bufferCount;

	// デスクリプタサイズを取得
	m_descriptorSize = coreSystem->GetD3DDevice()->GetDescriptorHandleIncrementSize(desc.Type);



	return true;
}



//	Sampler
bool	DescriptorHeap::InitializeSampler(uint32_t bufferCount)
{


	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = bufferCount;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;


	HRESULT hr;

	hr = coreSystem->GetD3DDevice()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateDescriptorHeap Failed %08x", hr);
		return false;
	}

	m_maxBufferCount = bufferCount;

	// デスクリプタサイズを取得
	m_descriptorSize = coreSystem->GetD3DDevice()->GetDescriptorHandleIncrementSize(desc.Type);



	return true;

}




void	DescriptorHeap::Finalize(bool delayedDelete)
{
	if (delayedDelete) {
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_descriptorHeap);
	}
	m_descriptorHeap.Release();
	m_maxBufferCount = 0;

}


D3D12_CPU_DESCRIPTOR_HANDLE		DescriptorHeap::GetCPUDescriptorHandleByIndex(uint32_t idx) const
{

	if (m_maxBufferCount <= idx) {
		D3D12_CPU_DESCRIPTOR_HANDLE handle = {0};
		return handle;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();

	handle.ptr += (m_descriptorSize * idx);

	return handle;

}



