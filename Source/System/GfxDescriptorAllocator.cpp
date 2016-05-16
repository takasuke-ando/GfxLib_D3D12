



#include "stdafx.h"
#include "System/GfxDescriptorAllocator.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;







DescriptorAllocator::Chunk::Chunk()
	:m_pDevice(nullptr)
	,m_Type( DescriptorHeapType::CBV_SRV_UAV)
	,m_IncSize(0)
	,m_Count(0)
	, m_paFreeIndexChain(nullptr)
	, m_nFirstFreeIndex(INVALID_INDEX )
{
	m_HandleFirst.ptr = 0;
	m_HandleLast.ptr = 0;
}

DescriptorAllocator::Chunk::~Chunk()
{
	if (m_paFreeIndexChain) {
		for (uint32_t i = 0; i < m_Count; ++i) {
			if (m_paFreeIndexChain[i] == INVALID_INDEX) {
				GFX_ERROR("[Descriptor Allocator] not freed DESC_HANDLE found!");
			}
		}

	}

	m_descriptorHeap = nullptr;
	delete[] m_paFreeIndexChain;
	m_paFreeIndexChain = nullptr;
}


bool DescriptorAllocator::Chunk::Initialize( ID3D12Device* device, DescriptorHeapType type, uint32_t count )
{

	if (count < 1) {
		GFX_ERROR(L"Invalid Parameter")

		return false;
	}


	D3D12_DESCRIPTOR_HEAP_DESC desc = {};

	desc.NumDescriptors = count;
	desc.Type = (D3D12_DESCRIPTOR_HEAP_TYPE)type;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;


	HRESULT hr;


	hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(m_descriptorHeap.InitialAccept()));
	if (FAILED(hr)) {
		GFX_ERROR(L"Create Descriptor Heap Error 0x%08x",hr)
		return false;
	}

	
	m_Type = type;
	m_IncSize = device->GetDescriptorHandleIncrementSize(desc.Type);
	m_Count = count;

	m_HandleFirst = m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_HandleLast.ptr = m_HandleFirst.ptr + m_IncSize * (count - 1);


	//m_paUseMask = new uint64_t[(m_Count+63)/64];
	m_paFreeIndexChain = new IndexType[count];
	for (uint32_t i = 0; i < count-1; ++i) {

		m_paFreeIndexChain[i] = i + 1;

	}
	m_paFreeIndexChain[count - 1] = INVALID_INDEX;
	m_nFirstFreeIndex = 0;


	return true;
}



D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Chunk::Allocate()
{

	if (m_nFirstFreeIndex != INVALID_INDEX) {

		uint32_t useIndex = m_nFirstFreeIndex;

		m_nFirstFreeIndex = m_paFreeIndexChain[useIndex];	//	次に使うインデックス
		m_paFreeIndexChain[useIndex] = INVALID_INDEX;

		
		D3D12_CPU_DESCRIPTOR_HANDLE h = m_HandleFirst;

		h.ptr += (useIndex + m_IncSize);

		return h;
	}


	return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };

}


/***************************************************************
@brief	開放
@par	[説明]
確保されたデスクリプタを開放する
countパラメータは、Allocate呼び出し時と同じ値を指定する事
@param	handle:	Allocateで確保されたハンドルポインタ
*/
void			DescriptorAllocator::Chunk::Free(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{

	if (handle.ptr == 0) {
		return;
	}

	if (handle.ptr < m_HandleFirst.ptr || m_HandleLast.ptr < handle.ptr) {
		// このチャンクが管理しているアドレスではない
		GFX_ERROR(L"Invalid Handle Pointer")
		return;

	}

	uint16_t index = uint16_t((handle.ptr - m_HandleFirst.ptr) / m_IncSize);

	if (m_paFreeIndexChain[index] != INVALID_INDEX) {

		//確保していない、あるいは多重開放
		GFX_ERROR(L"Invalid Handle Pointer")

		return;
	}

	m_paFreeIndexChain[index] = m_nFirstFreeIndex;
	m_nFirstFreeIndex = index;


}



DescriptorAllocator::DescriptorAllocator()
{
	m_pDevice = CoreSystem::GetInstance()->GetD3DDevice();
	m_pFirstChunk = nullptr;
}

DescriptorAllocator::~DescriptorAllocator()
{
}







