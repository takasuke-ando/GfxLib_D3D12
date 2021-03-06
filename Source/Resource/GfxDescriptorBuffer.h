﻿#ifndef __INCLUDE_GFXDESCRIPTORBUFFER_H__
#define __INCLUDE_GFXDESCRIPTORBUFFER_H__


/***************************************************************
	@file	DescriptorBuffer
	@par	[説明]
		デスクリプタバッファ
		DescriptorHeapのサブ領域に対してこのクラスを経由してアクセスする

		コピーや、ハンドル作成などユーティリティ的なインターフェースも提供する
*/


namespace GfxLib
{

	class DescriptorHeap;
	class CommandList;

	class DescriptorBuffer
	{
	public:
		DescriptorBuffer(DescriptorHeap*heap, uint32_t startIndex, uint32_t size, CommandList *cmdList);
		DescriptorBuffer();
		~DescriptorBuffer();


		/***************************************************************
			@brief	指定されたハンドルをコピー
			@par	[説明]
			@param
		*/
		void	CopyHandle(uint32_t index, const D3D12_CPU_DESCRIPTOR_HANDLE &handle);

		/***************************************************************
		@brief	指定されたハンドルをコピー
		@par	[説明]
		@param
		*/
		void	CopyHandle(uint32_t index, const D3D12_CPU_DESCRIPTOR_HANDLE *handle , uint32_t count );
		

		/***************************************************************
		@brief	定数バッファのセット
		@par	[説明]
			GPUアドレスからCBVをセットする
		@param
		*/
		void	SetConstantBuffer(uint32_t index, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress, uint32_t size);


		void	SetConstantBuffer(uint32_t index, const void* pData, uint32_t size);


		D3D12_GPU_DESCRIPTOR_HANDLE		GetGPUDescriptorHandle(uint32_t index=0) const;


		ID3D12DescriptorHeap*	GetD3DDescriptorHeap() const;

	private:

		DescriptorHeap*	const m_heap;
		ID3D12Device*	const m_d3dDev;
		uint32_t		const m_startIndex;
		uint32_t		const m_size;
		CommandList *	const m_cmdList;

	};





}


#include "GfxDescriptorBuffer.inl"


#endif // __INCLUDE_GFXDESCRIPTORBUFFER_H__
