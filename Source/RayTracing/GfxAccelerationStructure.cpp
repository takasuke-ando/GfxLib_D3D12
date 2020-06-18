




#include "stdafx.h"

#include "GfxAccelerationStructure.h"
#include "GfxRtGeometry.h"
#include "../System/GfxCoreSystem.h"
#include "../Device/GfxGraphicsCommandList.h"



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



void	AccelerationStructure::Finalize(bool delayed )
{

	SuperClass::Finalize(delayed);



}



BottomLevelAccelerationStructure::BottomLevelAccelerationStructure()
{

	m_Transform = DirectX::XMMatrixIdentity();

}


BottomLevelAccelerationStructure::~BottomLevelAccelerationStructure()
{

	Finalize();

}





bool BottomLevelAccelerationStructure::Initialize(const RtGeometry* geometrys, size_t numGeom)
{

	Finalize();


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

	GFX_ASSERT(((uint64_t)GetD3DResource()->GetGPUVirtualAddress()) % D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT == 0, L"Invalid BLAS Alignment");

	// スクラッチバッファは別途作成
	GFX_INFO(L"BLAS init Scratch DataSize :%d", m_prebuildInfo.ScratchDataSizeInBytes);
	GFX_INFO(L"BLAS init Update Scratch DataSize :%d", m_prebuildInfo.UpdateScratchDataSizeInBytes);


	m_asInputs = bottomLevelInputs;


	return true;

}





void	BottomLevelAccelerationStructure::Finalize(bool delayed)
{

	SuperClass::Finalize(delayed);


	m_vecGeometry.clear();
	m_vecGeomDesc.clear();


}




void	BottomLevelAccelerationStructure::Build(GraphicsCommandList& cmdList, Buffer* scratchBuffer)
{

	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC bottomLevelBuildDesc = {};

	bottomLevelBuildDesc.DestAccelerationStructureData = GetD3DResource()->GetGPUVirtualAddress();
	bottomLevelBuildDesc.ScratchAccelerationStructureData = scratchBuffer->GetGpuVirtualAddress();


	bottomLevelBuildDesc.Inputs = m_asInputs;
	


	cmdList.GetD3DCommandList4()->BuildRaytracingAccelerationStructure(&bottomLevelBuildDesc, 0, nullptr);


}



TopLevelAccelerationStructure::TopLevelAccelerationStructure()
{



}


TopLevelAccelerationStructure::~TopLevelAccelerationStructure()
{

	Finalize();

}



bool	TopLevelAccelerationStructure::Initialize(const BottomLevelAccelerationStructure* const * blas, size_t numBlas)
{
	Finalize();


	for (uint32_t i = 0; i < numBlas; ++i ) {

		m_vecBLAS.push_back(blas[i]);

	}




	ID3D12Device5* device = CoreSystem::GetInstance()->GetD3DDevice5();


	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAGS buildFlags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_ALLOW_UPDATE;





	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS topLevelInputs = {};
	topLevelInputs.DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY;
	topLevelInputs.Flags = buildFlags;
	topLevelInputs.NumDescs = (UINT)numBlas;	//	この段階では最大数のはず
	topLevelInputs.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL;


	// PreBuildInfoを取得

	device->GetRaytracingAccelerationStructurePrebuildInfo(&topLevelInputs, &m_prebuildInfo);


	if (m_prebuildInfo.ResultDataMaxSizeInBytes <= 0) {
		GFX_ERROR(L"Prebuild Info error");
		return false;
	}

	// GPUリソースを作成
	if (!_Initialize_Buffer(m_prebuildInfo.ResultDataMaxSizeInBytes, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)) {

		return false;
	}

	GFX_ASSERT(((uint64_t)GetD3DResource()->GetGPUVirtualAddress()) % D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BYTE_ALIGNMENT == 0, L"Invalid TLAS Alignment");

	// スクラッチバッファは別途作成
	GFX_INFO(L"TLAS init Scratch DataSize :%d", m_prebuildInfo.ScratchDataSizeInBytes);
	GFX_INFO(L"TLAS init Update Scratch DataSize :%d", m_prebuildInfo.UpdateScratchDataSizeInBytes);


	m_asInputs = topLevelInputs;


	return true;

}



void	TopLevelAccelerationStructure::Finalize(bool delayed )
{

	SuperClass::Finalize(delayed);

	m_vecBLAS.clear();

}



void	TopLevelAccelerationStructure::Build(GraphicsCommandList& cmdList, Buffer* scratchBuffer )
{
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC topLevelBuildDesc = {};
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS &topLevelInputs = topLevelBuildDesc.Inputs; // 

	topLevelBuildDesc.DestAccelerationStructureData = GetD3DResource()->GetGPUVirtualAddress();
	topLevelBuildDesc.ScratchAccelerationStructureData = scratchBuffer->GetGpuVirtualAddress();


	topLevelInputs = m_asInputs;

	//topLevelInputs.InstanceDescsはここで構築しないといけない


	uint32_t numBLAS = (uint32_t)m_vecBLAS.size();
	topLevelInputs.NumDescs = numBLAS;

	if (numBLAS > 0) {

		/*
		
			InstanceDescs
			If Type is D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TOP_LEVEL, this refers to NumDescsD3D12_RAYTRACING_INSTANCE_DESC structures in GPU memory describing instances. Each instance must be aligned to 16 bytes, defined as D3D12_RAYTRACING_INSTANCE_DESC_BYTE_ALIGNMENT.
			If Type is not D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TOP_LEVEL, this parameter is unused.
			If DescLayout is D3D12_ELEMENTS_LAYOUT_ARRAY, InstanceDescs points to an array of instance descriptions in GPU memory.
			If DescLayout is D3D12_ELEMENTS_LAYOUT_ARRAY_OF_POINTERS, InstanceDescs points to an array in GPU memory of D3D12_GPU_VIRTUAL_ADDRESS pointers to instance descriptions.
			The memory pointed to must be in state D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE.
		
		*/



		/*
			> The memory pointed to must be in state D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE.
			GfxLibの実装ではリソースステートはGENERIC_READだが、
			MSのサンプルを見る限りそれでもかまわない模様
		*/

		/*
			GPUメモリを確保し、コピー
		*/

		GpuBufferRange gpubuffer = cmdList.AllocateGpuBuffer(sizeof(D3D12_RAYTRACING_INSTANCE_DESC)*numBLAS, D3D12_RAYTRACING_INSTANCE_DESCS_BYTE_ALIGNMENT);


		for (uint32_t i = 0; i < numBLAS; ++i) {
			D3D12_RAYTRACING_INSTANCE_DESC instanceDesc = {};

			//	転置
			DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose( m_vecBLAS[i]->GetTransform() );

			//instanceDesc.Transform[0][0] = instanceDesc.Transform[1][1] = instanceDesc.Transform[2][2] = 1;

			memcpy(instanceDesc.Transform, &transform, sizeof(float)*3*4);
			instanceDesc.InstanceMask = 1;
			instanceDesc.AccelerationStructure = m_vecBLAS[i]->GetD3DResource()->GetGPUVirtualAddress();

			// まとめてコピーしたほうがよいだろうか
			memcpy( ((D3D12_RAYTRACING_INSTANCE_DESC*)gpubuffer.GetCpuAddr()) + i , &instanceDesc,sizeof(instanceDesc));

		}


	}


	cmdList.GetD3DCommandList4()->BuildRaytracingAccelerationStructure(&topLevelBuildDesc, 0, nullptr);


}