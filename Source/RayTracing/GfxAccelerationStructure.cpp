




#include "stdafx.h"

#include "GfxAccelerationStructure.h"
#include "GfxRtGeometry.h"
#include "../System/GfxCoreSystem.h"



using namespace GfxLib;





AccelerationStructure::AccelerationStructure()
{


	memset(&m_prebuildInfo, 0, sizeof(m_prebuildInfo));
	memset(&m_asInputs, 0, sizeof(m_asInputs));


}




AccelerationStructure::~AccelerationStructure()
{

	Finalize();

}


bool AccelerationStructure::InitializeBottomLevel(const RtGeometry* geometrys, size_t numGeom)
{

	ID3D12Device5* device = CoreSystem::GetInstance()->GetD3DDevice5();


	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_PREFER_FAST_TRACE;


	for (uint32_t i = 0; i < numGeom; ++i) {

		m_vecGeometry.push_back(geometrys + i);
		m_vecGeomDesc.push_back(geometrys[i].GetDesc());
	}



	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS bottomLevelInputs = {};
	bottomLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	bottomLevelInputs.Flags = buildFlags;
	bottomLevelInputs.NumDescs = (UINT)numGeom;
	bottomLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL;
	bottomLevelInputs.pGeometryDescs = &m_vecGeomDesc[0];


	// PreBuildInfoを取得

	device->GetRaytracingAccelerationStructurePrebuildInfo(&bottomLevelInputs, &m_prebuildInfo);


	if (m_prebuildInfo.ResultDataMaxSizeInBytes <= 0) {
		GFX_ERROR(L"Prebuild Info error");
		return false;
	}

	// GPUリソースを作成
	if (!_Initialize_Buffer(m_prebuildInfo.ResultDataMaxSizeInBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)) {

		return false;
	}


	// スクラッチバッファは別途作成
	GFX_INFO(L"BLAS init Scratch DataSize :%d", m_prebuildInfo.ScratchDataSizeInBytes);
	GFX_INFO(L"BLAS init Update Scratch DataSize :%d",m_prebuildInfo.UpdateScratchDataSizeInBytes);


	m_asInputs = bottomLevelInputs;


	return true;

}



void	AccelerationStructure::Finalize(bool delayed )
{

	SuperClass::Finalize(delayed);


	m_vecGeometry.clear();
	m_vecGeomDesc.clear();


}







