



#include "stdafx.h"
#include "System/GfxDescriptorAllocator.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;

namespace {


	int HANDLE_COUNT_IN_CHUNK = 256;


}





DescriptorAllocator::Chunk::Chunk()
	:m_pDevice(nullptr)
	,m_Type( DescriptorHeapType::CBV_SRV_UAV)
	,m_IncSize(0)
	,m_Count(0)
	, m_paFreeIndexChain(nullptr)
	, m_nFirstFreeIndex(INVALID_INDEX )
	, m_pNextChunk(nullptr)
{
	m_HandleFirst.ptr = 0;
	m_HandleLast.ptr = 0;
}

DescriptorAllocator::Chunk::~Chunk()
{
	if (m_paFreeIndexChain) {
		uint32_t foundInvalidIndex = 0;
		for (uint32_t i = 0; i < m_Count; ++i) {
			if (m_paFreeIndexChain[i] == INVALID_INDEX) {
				++foundInvalidIndex;
			}
		}
		if (foundInvalidIndex != 1) {
			GFX_ERROR("[Descriptor Allocator] not freed DESC_HANDLE found!");
		}

	}

	m_descriptorHeap = nullptr;
	delete[] m_paFreeIndexChain;
	m_paFreeIndexChain = nullptr;

	if (m_pNextChunk) {
		delete m_pNextChunk;
		m_pNextChunk = nullptr;
	}
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
	m_pDevice = device;

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


void	DescriptorAllocator::Chunk::InitializeDummy()
{

	m_HandleFirst.ptr = 0;
	m_HandleLast.ptr = 0;
	m_nFirstFreeIndex = INVALID_INDEX;

}




D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Chunk::Allocate()
{

	if (m_nFirstFreeIndex != INVALID_INDEX) {

		uint32_t useIndex = m_nFirstFreeIndex;

		m_nFirstFreeIndex = m_paFreeIndexChain[useIndex];	//	次に使うインデックス
		m_paFreeIndexChain[useIndex] = INVALID_INDEX;

		
		D3D12_CPU_DESCRIPTOR_HANDLE h = m_HandleFirst;

		h.ptr += (useIndex * m_IncSize);

		return h;
	}

	if (!m_pNextChunk) {

		//	次のチャンクを作成
		m_pNextChunk = new Chunk;
		if ( !m_pNextChunk || !m_pNextChunk->Initialize(m_pDevice, m_Type, m_Count)) {

			return D3D12_CPU_DESCRIPTOR_HANDLE{ 0 };
		}

	}

	//	次のチャンクにお任せする
	return m_pNextChunk->Allocate();


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

		if (m_pNextChunk) {
			m_pNextChunk->Free(handle);
			return;
		}

		// 対応するチャンクが見つからなかった
		GFX_ERROR(L"Invalid Handle Pointer  Chunk not found!");

		return;

	}

	uint16_t index = uint16_t((handle.ptr - m_HandleFirst.ptr) / m_IncSize);



	if (handle.ptr != m_HandleFirst.ptr + m_IncSize * index ||	//	中途半端なアドレスが指定されている
		m_paFreeIndexChain[index] != INVALID_INDEX) {

		//確保していない、あるいは多重開放
		GFX_ERROR(L"Invalid Handle Pointer")

		return;
	}

	m_paFreeIndexChain[index] = m_nFirstFreeIndex;
	m_nFirstFreeIndex = index;

	// 開放成功
}



DescriptorAllocator::DescriptorAllocator()
{
	m_pDevice = CoreSystem::GetInstance()->GetD3DDevice();
	m_paFirstChunk = new Chunk[DescriptorHeapType::NUM_TYPES];


	for (uint32_t i = 0; i < (uint32_t)DescriptorHeapType::NUM_TYPES; ++i) {
		m_paFirstChunk[i].Initialize(m_pDevice, (DescriptorHeapType)i, HANDLE_COUNT_IN_CHUNK);
	}
}



DescriptorAllocator::~DescriptorAllocator()
{
	delete[] m_paFirstChunk;
	m_paFirstChunk = nullptr;
}



/***************************************************************
@brief	確保
@par	[説明]
確保を行う
通常のメモリ確保同様、Freeを呼び出して開放する必要がある
*/
D3D12_CPU_DESCRIPTOR_HANDLE		DescriptorAllocator::Allocate(DescriptorHeapType type)
{

	return m_paFirstChunk[(uint32_t)type].Allocate();

}



/***************************************************************
@brief	開放
@par	[説明]
確保されたデスクリプタを開放する
countパラメータは、Allocate呼び出し時と同じ値を指定する事
@param	handle:	ハンドルパラメータ
*/
void							DescriptorAllocator::Free(DescriptorHeapType type, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{

	m_paFirstChunk[(uint32_t)type].Free(handle);
	
}



