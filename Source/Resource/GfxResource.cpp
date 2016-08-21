








#include "stdafx.h"

#include "GfxResource.h"
#include "System/GfxCoreSystem.h"
#include "Util/GfxFormatUtil.h"



using namespace GfxLib;


Resource::Resource()
{



}


Resource::~Resource()
{

	Finalize();

}



void Resource::Finalize(bool delayed)
{
	if (delayed) {

		// 遅延開放の場合
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dRes);

	}

	m_d3dRes.Release();

}





bool		Resource::_Initialize_Buffer(size_t sizeInBytes, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initState)
{


	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = heapType;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = sizeInBytes;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		initState,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;

}



bool		Resource::_Initialize_RenderTarget( Format format, uint32_t width, uint32_t height, uint32_t mipLevls , bool EnableShaderResource)
{


	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = mipLevls;
	resDesc.Format = (DXGI_FORMAT)format;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//resDesc.Flags = D3D12_RESOURCE_FLAGS(D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | (EnableShaderResource ? 0 : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE));
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	// DENY_SHADER_RESOURCEは、DEPTH_STENCILの時しか使えない



	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;



}



bool		Resource::_Initialize_DepthStencil(Format format, uint32_t width, uint32_t height, uint32_t mipLevls, bool EnableShaderResource, const D3D12_CLEAR_VALUE *clearValue)
{



	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 1;
	heapProp.VisibleNodeMask = 1;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = mipLevls;
	resDesc.Format = (DXGI_FORMAT)format;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAGS(D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL | (EnableShaderResource ? 0 : D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE));



	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		clearValue,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;




}






/*
	mutable なTexture2Dとして初期化

	http://blog.techlab-xe.net/archives/3767
	http://qiita.com/em7dfggbcadd9/items/b5a9b71a6ae29d86da50
	さんこう
*/
bool		Resource::_Initialize_Texture2D_Test(Format format, uint32_t width, uint32_t height, uint32_t mipLevls)
{


	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	//	ひとまずCPUアクセス可能なHeapにするが
	//	本来はD3D12_HEAP_TYPE_DEFAULTにして、UPLOADヒープからコピーするだけにしたい

	heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;		//@TODO		D3D12_HEAP_TYPE_DEFAULTにして、UPLOADヒープからコピーするだけにしたい
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE;	//	WRITE_COMBINEだと思うのだけれど、上記サイトではWRITE_BACKとしてた
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;	//	CPUアクセス可能。GPUからは低速
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = mipLevls;
	resDesc.Format = (DXGI_FORMAT)format;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
		);


	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;




}


/***************************************************************
	@brief	Immutableなリソースとして初期化
	@par	[説明]
		HEAP_TYPE_DEFAULTで作成するので
		UPLOADヒープを経由して初期化を行う必要がある

		リソースステートはCOPY_DESTになる
	@param
*/
bool		Resource::_Initialize_Texture2D(Format format, uint32_t width, uint32_t height, uint32_t mipLevls)
{

	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	//	ひとまずCPUアクセス可能なHeapにするが
	//	本来はD3D12_HEAP_TYPE_DEFAULTにして、UPLOADヒープからコピーするだけにしたい

	heapProp.Type = D3D12_HEAP_TYPE_DEFAULT;		
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;	
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Alignment = 0;
	resDesc.Width = width;
	resDesc.Height = height;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = mipLevls;
	resDesc.Format = (DXGI_FORMAT)format;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
		);


	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;



}
