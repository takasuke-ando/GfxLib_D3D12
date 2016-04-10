/***************************************************************
	@brief	定数バッファ
	@par	[説明]
		シンプルな定数バッファオブジェクトをカプセル化
	@param
***************************************************************/



#include "stdafx.h"
#include "GfxConstantBuffer.h"


#include "System/GfxCoreSystem.h"

using namespace GfxLib;




ConstantBuffer::ConstantBuffer()
	:m_byteSize(0)
	,m_pMappedAddr(nullptr)
{
}

ConstantBuffer::~ConstantBuffer()
{
	Finalize();
}


bool	ConstantBuffer::Initialize(uint32_t byteSize)
{
	Finalize();


	CoreSystem *coreSystem = CoreSystem::GetInstance();

	ID3D12Device *d3dDev = coreSystem->GetD3DDevice();


	//	ヒーププロパティの設定
	D3D12_HEAP_PROPERTIES prop = {};

	prop.Type = D3D12_HEAP_TYPE_UPLOAD;
	prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	prop.CreationNodeMask = 1;
	prop.VisibleNodeMask = 1;


	D3D12_RESOURCE_DESC resDesc = {};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Alignment = 0;
	resDesc.Width = (byteSize + 255) / 256 * 256;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc = DXGI_SAMPLE_DESC{ 1,0 };
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	HRESULT hr;
	hr = d3dDev->CreateCommittedResource(
		&prop, D3D12_HEAP_FLAG_NONE, &resDesc, 
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, 
		IID_PPV_ARGS(m_d3dRes.InitialAccept()));

	if (FAILED(hr)) {
		GFX_ERROR_LOG( L"Create Comitted Resource Failed = %08x", hr )
		return false;
	}



	bool bOk = m_descHeap.InitializeCBV_SRV_UAV(1);
	if (!bOk) {
		return false;
	}


	{
		D3D12_CONSTANT_BUFFER_VIEW_DESC viewDesc = {};

		viewDesc.SizeInBytes = resDesc.Width;
		viewDesc.BufferLocation = m_d3dRes->GetGPUVirtualAddress();

		d3dDev->CreateConstantBufferView(&viewDesc, m_descHeap.GetCPUDescriptorHandleByIndex(0) );
	}


	m_byteSize = (uint32_t)resDesc.Width;
	m_d3dRes->Map(0, nullptr, &m_pMappedAddr);



	return true;

}




void ConstantBuffer::Finalize(bool delayed)
{
	if (delayed) {
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dRes);
	}
	m_d3dRes.Release();
	m_descHeap.Finalize(delayed);
	m_pMappedAddr = nullptr;
}


void	ConstantBuffer::SetData(const void*pData, uint32_t byteSize)
{

	if (!m_pMappedAddr) {
		return;
	}
	
	GFX_ASSERT(m_byteSize >= byteSize,L"Data Size is bigger");

	memcpy(m_pMappedAddr, pData, byteSize);

}
