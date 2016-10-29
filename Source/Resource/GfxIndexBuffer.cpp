





#include "stdafx.h"

#include "Resource/GfxIndexBuffer.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;


IndexBuffer::IndexBuffer()
{
	m_idxBuffView = D3D12_INDEX_BUFFER_VIEW{};


}



IndexBuffer::~IndexBuffer()
{
	Finalize();
}



/***************************************************************
@brief	初期化
@par	[説明]
初期化データを指定して、初期化を行う
@param
*/
bool IndexBuffer::Initialize(const void* pVtxData, size_t byteSize, Format fmt)
{
	Finalize();

	const size_t sizeInBytes = byteSize;

	bool b = InitializeImmutable(pVtxData, sizeInBytes);

	if (b) {

		m_idxBuffView.BufferLocation = m_d3dRes->GetGPUVirtualAddress();
		m_idxBuffView.SizeInBytes = (UINT)sizeInBytes;
		m_idxBuffView.Format = (DXGI_FORMAT)fmt;
	}


	return b;

}




void IndexBuffer::Finalize(bool delayed)
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






















