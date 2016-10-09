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



/***************************************************************
@brief	初期化
@par	[説明]
サイズを指定して、初期化を行う
GPUからしかアクセスできない状態になる
@param
*/
bool Buffer::InitializeImmutable(const void* pData, size_t byteSize)
{
	Finalize();


	const size_t sizeInBytes = byteSize;


	bool b = _Initialize_Buffer(sizeInBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_COPY_DEST);

	if (!b) {
		return false;
	}


	{
		CommandList *initCmdList = CoreSystem::GetInstance()->GetResourceInitCommandList();
		initCmdList->InitializeResource(GetD3DResource(), pData, byteSize);
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






