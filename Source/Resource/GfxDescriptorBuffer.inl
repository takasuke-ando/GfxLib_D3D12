﻿#ifndef __INCLUDE_GFXDESCRIPTORBUFFER_INL__
#define	__INCLUDE_GFXDESCRIPTORBUFFER_INL__



#include "System/GfxCoreSystem.h"
#include "Resource/GfxDescriptorHeap.h"
#include "Util/GfxMath.h"


namespace GfxLib
{




	inline 		DescriptorBuffer::DescriptorBuffer(DescriptorHeap*heap, uint32_t startIndex, uint32_t size, CommandList *cmdList)
		:m_heap(heap)
		, m_d3dDev(CoreSystem::GetInstance()->GetD3DDevice())
		, m_startIndex(startIndex)
		, m_size(size)
		, m_cmdList(cmdList)
	{


	}

	inline		DescriptorBuffer::DescriptorBuffer()
		:m_heap(nullptr)
		, m_d3dDev(CoreSystem::GetInstance()->GetD3DDevice())
		, m_startIndex(0)
		, m_size(0)
		, m_cmdList(nullptr)
	{
	}

	inline DescriptorBuffer::~DescriptorBuffer()
	{
	}




	inline ID3D12DescriptorHeap*	DescriptorBuffer::GetD3DDescriptorHeap() const {
		return m_heap->GetD3DDescriptorHeap();
	}


	inline D3D12_GPU_DESCRIPTOR_HANDLE		DescriptorBuffer::GetGPUDescriptorHandle(uint32_t index) const {
		GFX_ASSERT(index < m_size, L"Invalid Index");
		return m_heap->GetGPUDescriptorHandleByIndex(m_startIndex+ index);
	}


	/***************************************************************
	@brief	指定されたハンドルをコピー
	@par	[説明]
	@param
	*/
	inline void	DescriptorBuffer::CopyHandle(uint32_t index, const D3D12_CPU_DESCRIPTOR_HANDLE &handle)
	{
		if (index >= m_size) {
			GFX_ERROR(L"Index over");
		}

		auto heapType = m_heap->GetHeapType();

		m_d3dDev->CopyDescriptorsSimple(1,
			m_heap->GetCPUDescriptorHandleByIndex(m_startIndex+ index ),
			handle,
			heapType);
	}

	/***************************************************************
	@brief	指定されたハンドルをコピー
	@par	[説明]
	@param
	*/
	inline void	DescriptorBuffer::CopyHandle(uint32_t index, const D3D12_CPU_DESCRIPTOR_HANDLE *srcHandle, uint32_t count)
	{
		if (index + count > m_size) {
			GFX_ERROR(L"Index over");
		}

		auto heapType = m_heap->GetHeapType();

		for (uint32_t i = 0; i < count; ++i) {

			m_d3dDev->CopyDescriptorsSimple(1,
				m_heap->GetCPUDescriptorHandleByIndex(m_startIndex + index + i),
				srcHandle[i],
				heapType);
		}


	}



	/***************************************************************
	@brief	定数バッファのセット
	@par	[説明]
	GPUアドレスからCBVをセットする
	@param
	*/
	inline void	DescriptorBuffer::SetConstantBuffer(uint32_t index, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress, uint32_t size)
	{
		if (index  > m_size) {
			GFX_ERROR(L"Index over");
		}

		D3D12_CONSTANT_BUFFER_VIEW_DESC	cbvDesc = { 0 };

		cbvDesc.BufferLocation = gpuAddress;
		cbvDesc.SizeInBytes = UpperBounds( size , D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT );

		m_d3dDev->CreateConstantBufferView(&cbvDesc, m_heap->GetCPUDescriptorHandleByIndex(m_startIndex + index));



	}



}


#endif // !__INCLUDE_GFXDESCRIPTORBUFFER_INL__
