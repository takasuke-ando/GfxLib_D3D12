



#include "stdafx.h"


#include "Resource/GfxVertexBuffer.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;


VertexBuffer::VertexBuffer()
{
	m_vtxBuffView = D3D12_VERTEX_BUFFER_VIEW{};


}



VertexBuffer::~VertexBuffer()
{
	Finalize();
}



bool VertexBuffer::Initialize(const void* pVtxData, size_t strideSize, size_t vtxCount)
{
	Finalize();


	const size_t sizeInBytes = strideSize * vtxCount;


	bool b = _Initialize_Buffer(sizeInBytes);

	void* pAddr = nullptr;
	HRESULT hr = m_d3dRes->Map(0, nullptr, &pAddr);

	if (FAILED(hr)) {
		GFX_ERROR(L"Map Error ! (%08x)", hr);
		m_d3dRes.Release();
		return false;
	}

	// バイトコピー
	memcpy(pAddr, pVtxData, sizeInBytes);

	
	m_d3dRes->Unmap(0, nullptr);


	m_vtxBuffView.BufferLocation = m_d3dRes->GetGPUVirtualAddress();
	m_vtxBuffView.StrideInBytes = (UINT)strideSize;
	m_vtxBuffView.SizeInBytes = (UINT)sizeInBytes;


	return true;

}




void VertexBuffer::Finalize(bool delayed)
{
	/*
	if (delayed) {

		// 遅延開放の場合
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dRes);

	}

	m_d3dRes.Release();
	*/
	SuperClass::Finalize(delayed);

}






