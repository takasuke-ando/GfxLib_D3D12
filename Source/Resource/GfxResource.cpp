








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





bool		Resource::_Initialize_Buffer(size_t sizeInBytes)
{


	ID3D12Device *pDev = CoreSystem::GetInstance()->GetD3DDevice();

	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
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
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateCommittedResource Error ! (%08x)", hr);
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
	resDesc.Flags = D3D12_RESOURCE_FLAGS( D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET | ( EnableShaderResource ? 0: D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) );


	HRESULT hr = pDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;



}



bool		Resource::_Initialize_DepthStencil(Format format, uint32_t width, uint32_t height, uint32_t mipLevls, bool EnableShaderResource)
{

	if ( !FormatUtil::IsDepthStencil(format)) {

		GFX_ERROR_LOG(L"This format is not DepthStencil ! (%d)", format);

		return false;
	}


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
		nullptr,
		IID_PPV_ARGS(m_d3dRes.InitialAccept())
	);


	if (FAILED(hr)) {
		GFX_ERROR_LOG(L"CreateCommittedResource Error ! (%08x)", hr);
		return false;
	}


	return true;




}


