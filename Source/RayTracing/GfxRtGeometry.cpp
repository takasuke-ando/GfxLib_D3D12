





#include "stdafx.h"



#include "GfxRtGeometry.h"



using namespace GfxLib;



RtGeometry::RtGeometry()
{

    memset(&m_geomDesc, 0, sizeof(m_geomDesc));

}


RtGeometry::~RtGeometry()
{
    Finalize();
}



bool	RtGeometry::Initialize(const void* vtxData, size_t vtxStrideSize, size_t vtxCount,
	const void* idxData, Format idxFormat, size_t idxCount)
{

    /*
    
        インデックスバッファのステートは
         D3D12_RESOURCE_STATE_INDEX_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE.

         バーテックスバッファのステートは
        D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER | D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE

        にするのが正しい模様

    */


    if (!m_vtxBuffer.Initialize(vtxData, vtxStrideSize, vtxCount))   return false;
    if (!m_idxBuffer.Initialize(idxData, idxFormat, idxCount))       return false;

    if (!m_idxBufferSRV.Initialize(m_idxBuffer.GetD3DResource()) ) return false;
    

    D3D12_RAYTRACING_GEOMETRY_DESC geometryDesc = {};
    geometryDesc.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES;
    geometryDesc.Triangles.IndexBuffer = m_idxBuffer.GetGpuVirtualAddress();
    geometryDesc.Triangles.IndexCount = static_cast<UINT>(idxCount);
    geometryDesc.Triangles.IndexFormat = (DXGI_FORMAT)idxFormat;
    geometryDesc.Triangles.Transform3x4 = 0;
    geometryDesc.Triangles.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT;
    geometryDesc.Triangles.VertexCount = static_cast<UINT>(vtxCount);
    geometryDesc.Triangles.VertexBuffer.StartAddress = m_vtxBuffer.GetGpuVirtualAddress();
    geometryDesc.Triangles.VertexBuffer.StrideInBytes = vtxStrideSize;

    geometryDesc.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_OPAQUE;


    m_geomDesc = geometryDesc;



    return true;

}



void	RtGeometry::Finalize(bool delayed )
{

    m_vtxBuffer.Finalize(delayed);
    m_idxBuffer.Finalize(delayed);
    m_idxBufferSRV.Finalize(delayed);

}



void	RtGeometry::GetSubsetDesc(D3D12_RAYTRACING_GEOMETRY_DESC&outDesc, uint32_t indexOffset, uint32_t indexCount) const
{


    outDesc = m_geomDesc;


    uint32_t indexSize = 4;

    if (m_geomDesc.Triangles.IndexFormat == DXGI_FORMAT_R32_UINT) {
        indexSize = 4;
    } else if (m_geomDesc.Triangles.IndexFormat == DXGI_FORMAT_R16_UINT) {
        indexSize = 2;
    } else {
        // ?
        // Index無し未対応

        GFX_ASSERT(0,L"Unknown Index Format (%d)" , m_geomDesc.Triangles.IndexFormat);
        return;

    }


    auto& triangle = outDesc.Triangles;

    // 範囲を飛び出している
    GFX_ASSERT(indexCount+indexOffset<= triangle.IndexCount, L"Invalid SubSet");


    triangle.IndexBuffer += indexSize * indexOffset;
    triangle.IndexCount = indexCount;


}
