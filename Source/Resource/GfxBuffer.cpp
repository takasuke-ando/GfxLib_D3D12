/***************************************************************
		@file	GpuBuffer
		@par	[説明]
			GPUアクセス可能なバッファを作成する
			定数バッファ、頂点バッファ、インデックスバッファとして使用できる

/****************************************************************/



#include "stdafx.h"


#include "Resource/GfxBuffer.h"
#include "System/GfxCoreSystem.h"
#include "Device/GfxCommandList.h"
#include "Device/GfxGraphicsCommandList.h"

#include "External/DirectX-Graphics-Samples/d3dx12.h"

using namespace GfxLib;


Buffer::Buffer()
	:m_pGpuAddress(0)
	,m_pCpuAddress(nullptr)
	, m_BuffSize(0)
{


}



Buffer::~Buffer()
{
	Finalize();
}



bool Buffer::Initialize( size_t byteSize)
{
	Finalize();


	const size_t sizeInBytes = byteSize;


	bool b = _Initialize_Buffer(sizeInBytes,D3D12_HEAP_TYPE_UPLOAD,D3D12_RESOURCE_STATE_GENERIC_READ);

	if (!b) {
		return false;
	}

	void* pAddr = nullptr;
	HRESULT hr = m_d3dRes->Map(0, nullptr, &pAddr);

	if (FAILED(hr)) {
		GFX_ERROR(L"Map Error ! (%08x)", hr);
		m_d3dRes.Release();
		return false;
	}

	m_pCpuAddress = pAddr;
	m_pGpuAddress = m_d3dRes->GetGPUVirtualAddress();
	m_BuffSize = (uint32_t)byteSize;

	return true;

}




bool Buffer::Initialize(size_t byteSize, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states)
{
	Finalize();



	const size_t sizeInBytes = byteSize;


	bool b = _Initialize_Buffer(sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, states , flags );

	if (!b) {
		return false;
	}

	void* pAddr = nullptr;

	m_pCpuAddress = pAddr;
	m_pGpuAddress = m_d3dRes->GetGPUVirtualAddress();
	m_BuffSize = (uint32_t)byteSize;

	return true;


}


/***************************************************************
@brief	初期化
@par	[説明]
サイズを指定して、初期化を行う
GPUからしかアクセスできない状態になる
@param
*/
bool Buffer::InitializeImmutable(const void* pData, size_t byteSize, ResourceStates states)
{
	Finalize();


	const size_t sizeInBytes = byteSize;


	bool b = _Initialize_Buffer(sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

	if (!b) {
		return false;
	}


	{
		auto *initCmdList = CoreSystem::GetInstance()->GetResourceInitCommandList();
		initCmdList->InitializeResource(GetD3DResource(), pData, byteSize, states);
	}


	
	m_pGpuAddress = m_d3dRes->GetGPUVirtualAddress();
	m_BuffSize = (uint32_t)byteSize;

	return true;

}



void Buffer::Finalize(bool delayed)
{
	/*
	if (delayed) {

		// 遅延開放の場合
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dRes);

	}

	m_d3dRes.Release();
	*/
	SuperClass::Finalize(delayed);

	m_pCpuAddress = nullptr;
	m_pGpuAddress = 0;
	m_BuffSize = 0;

}



ByteAddressBuffer::ByteAddressBuffer()
{

	m_SrvHandle.ptr = 0;

}



ByteAddressBuffer::~ByteAddressBuffer()
{

	Finalize();

}




/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void ByteAddressBuffer::Finalize(bool delayed )
{

	if (m_SrvHandle.ptr != 0) {

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_SrvHandle);
		m_SrvHandle.ptr = 0;

	}

	SuperClass::Finalize(delayed);

}



/***************************************************************
@brief	初期化
@par	[説明]
	任意のステート、フラグを指定してリソースを初期化
	CPUアクセスはできない
@param
*/
bool ByteAddressBuffer::Initialize(size_t byteSize, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states)
{

	if (!SuperClass::Initialize(byteSize, flags, states)) {

		return false;
	}


	_InitSRV();


	return true;
}


/***************************************************************
@brief	初期化
@par	[説明]
	サイズを指定して、初期化を行う
	GPUからしかアクセスできない状態になる
@param
*/
bool ByteAddressBuffer::InitializeImmutable(const void* pData, size_t byteSize, ResourceStates states)
{

	if (!SuperClass::InitializeImmutable(pData,byteSize,states)) {


		return false;

	}

	_InitSRV();


	return true;

}



void 	ByteAddressBuffer::_InitSRV()
{


	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Buffer.NumElements = (UINT)GetBufferSize() / 4;
	SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_RAW;


	ID3D12Device* d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
	d3dDev->CreateShaderResourceView(GetD3DResource(), &SRVDesc, m_SrvHandle);


}





StructuredBuffer::StructuredBuffer()
	:m_ElementSize(0)
	,m_ElementCount(0)
{

	m_SrvHandle.ptr = 0;

}



StructuredBuffer::~StructuredBuffer()
{

	Finalize();

}




/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void StructuredBuffer::Finalize(bool delayed)
{

	if (m_SrvHandle.ptr != 0) {

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_SrvHandle);
		m_SrvHandle.ptr = 0;

	}

	SuperClass::Finalize(delayed);

}



/***************************************************************
@brief	初期化
@par	[説明]
	任意のステート、フラグを指定してリソースを初期化
	CPUアクセスはできない
@param
*/
bool StructuredBuffer::Initialize(uint32_t elementSize, uint32_t elementCount, D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES states)
{

	if (!SuperClass::Initialize(elementSize*elementCount, flags, states)) {

		return false;
	}

	m_ElementSize = elementSize;
	m_ElementCount = elementCount;

	_InitSRV();


	return true;
}


/***************************************************************
@brief	初期化
@par	[説明]
	サイズを指定して、初期化を行う
	GPUからしかアクセスできない状態になる
@param
*/
bool StructuredBuffer::InitializeImmutable(const void* pData, uint32_t elementSize, uint32_t elementCount, ResourceStates states)
{

	if (!SuperClass::InitializeImmutable(pData, elementSize*elementCount, states)) {


		return false;

	}


	m_ElementSize = elementSize;
	m_ElementCount = elementCount;

	_InitSRV();


	return true;

}



void 	StructuredBuffer::_InitSRV()
{


	D3D12_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
	SRVDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	SRVDesc.Buffer.NumElements = m_ElementCount;
	SRVDesc.Buffer.StructureByteStride = m_ElementSize;
	SRVDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;


	ID3D12Device* d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
	d3dDev->CreateShaderResourceView(GetD3DResource(), &SRVDesc, m_SrvHandle);


}

