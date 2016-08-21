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

	//	CopyQueueを使って転送する
	{
		ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();

		CommandList *initCmdList = CoreSystem::GetInstance()->GetResourceInitCommandList();

		// @TODO	マルチスレッド処理の場合はどうするのか


		D3DPtr<ID3D12Resource>	d3dResForUpload;

		D3D12_HEAP_PROPERTIES heapProp = {};


		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;


		D3D12_RESOURCE_DESC resDesc = {};

		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//	TextureへのUpload中間バッファは、Bufferとして作る必要がある
		resDesc.Alignment = 0;
		resDesc.Width = byteSize;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//	RowMajor→UNKNOWN へのコピー可能？
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



		HRESULT hr = d3dDev->CreateCommittedResource(
			&heapProp,
			D3D12_HEAP_FLAG_NONE,
			&resDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,		//	変更不可
			nullptr,
			IID_PPV_ARGS(d3dResForUpload.InitialAccept())
			);


		if (FAILED(hr)) {
			GFX_ERROR(L"Create Committed Resource Failed (%08x)", hr);
			return false;
		}

		void *pInterBuff;
		hr = d3dResForUpload->Map(0, nullptr, &pInterBuff);
		if (FAILED(hr)) {
			GFX_ERROR(L"Intermediate Resouce Map Failed (%08x)", hr);
			return false;
		}

		// CPU Copy
		memcpy(pInterBuff, pData, byteSize);

		d3dResForUpload->Unmap(0,nullptr);

		// Defaultバッファにコピーを行う
		initCmdList->GetD3DCommandList()->CopyBufferRegion(GetD3DResource(), 0, d3dResForUpload, 0, byteSize );
		initCmdList->ResourceTransitionBarrier(GetD3DResource(), ResourceStates::CopyDest, ResourceStates::GenericRead);


		// 遅延開放に登録
		CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);


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






