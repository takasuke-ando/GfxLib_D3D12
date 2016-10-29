






#include "stdafx.h"


//	自分自身のインクルード
#include "GfxGpuBufferRange.h"


//	その他のインクルード




using namespace GfxLib;





GpuBufferRange::GpuBufferRange()
	:m_pResource(nullptr)
	,m_StartOffset(0)
	,m_pStartAddr(nullptr)
	,m_pStartGpuAddr(0)
	,m_Size(0)
{
}


GpuBufferRange::GpuBufferRange(ID3D12Resource* res, size_t startOffset, void* cpuAddr, D3D12_GPU_VIRTUAL_ADDRESS gpuAddr, size_t Size)
	:m_pResource(res)
	,m_StartOffset(startOffset)
	,m_pStartAddr(cpuAddr)
	,m_pStartGpuAddr(gpuAddr)
	,m_Size(Size)
{


}


GpuBufferRange::~GpuBufferRange()
{
}


