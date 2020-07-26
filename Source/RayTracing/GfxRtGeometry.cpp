





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


