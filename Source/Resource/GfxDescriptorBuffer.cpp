




#include "stdafx.h"

#include "Resource/GfxDescriptorBuffer.h"
#include "Device/GfxCommandList.h"



using namespace GfxLib;





/***************************************************************
@brief	デスクリプタバッファに定数を設定する
@par	[説明]
デスクリプタバッファに定数バッファを設定する
@param
*/
void	DescriptorBuffer::SetConstantBuffer(uint32_t index, const void* pData, uint32_t size)
{

	void* cpuAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = m_cmdList->AllocateGpuBuffer(cpuAddr,
		GfxLib::UpperBounds(size, 256), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);


	memcpy(cpuAddr, pData, size);

	SetConstantBuffer(index, gpuAddr, size);

}
