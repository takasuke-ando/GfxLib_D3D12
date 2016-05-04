








#include "stdafx.h"

#include "GfxResource.h"
#include "System/GfxCoreSystem.h"



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








