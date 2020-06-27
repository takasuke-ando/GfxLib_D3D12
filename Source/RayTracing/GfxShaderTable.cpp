

#include "stdafx.h"

#include "RayTracing/GfxShaderTable.h"
#include "Util/GfxMath.h"
#include "Device/GfxCommandList.h"




using namespace GfxLib;



ShaderTable::ShaderTable(CommandList& cmdList, uint32_t numShaderRecords, uint32_t shaderRecordSize)
{
    m_shaderRecordSize = UpperBounds(shaderRecordSize, D3D12_RAYTRACING_SHADER_RECORD_BYTE_ALIGNMENT);
    m_numShaderRecords = numShaderRecords;

    m_gpuBuffer = cmdList.AllocateGpuBuffer(m_shaderRecordSize*m_numShaderRecords, D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT);

    m_pNextCopyDest = (uint8_t*)m_gpuBuffer.GetCpuAddr();
}



ShaderTable::~ShaderTable()
{




}


void	ShaderTable::AddRecord(const void* shaderIdentifier, const void* localRootArguments, uint32_t localRootArgumentsSize)
{
    GFX_ASSERT(m_pNextCopyDest<(uint8_t*)m_gpuBuffer.GetCpuAddr()+m_numShaderRecords*m_shaderRecordSize , L"Shader Table Write Size Over" );


    // ShaderIdentifier と、LocalRootArgumentsをコピー
    memcpy(m_pNextCopyDest, shaderIdentifier, D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);

    if (localRootArgumentsSize) {
        memcpy(m_pNextCopyDest + D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES, localRootArguments, localRootArgumentsSize);
    }

    m_pNextCopyDest += m_shaderRecordSize;


}






