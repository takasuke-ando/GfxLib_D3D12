

#include "stdafx.h"

#include "GfxRayTracingRenderer.h"

#include "GfxRtSceneTargets.h"

#include "Resource/GfxRootSignatureDesc.h"
#include "RayTracing/GfxRtPipelineStateDesc.h"
#include "RayTracing/GfxPipelineStateSubobject.h"
#include "System/GfxDefines.h"
#include "System/GfxCoreSystem.h"
#include "Resource/GfxGpuBufferRange.h"
#include "Device/GfxGraphicsCommandList.h"
#include "RayTracing/GfxShaderTable.h"
#include "Resource/GfxTexture2D.h"
#include "RayTracing/GfxAccelerationStructure.h"
#include "RayTracing/GfxRtModel.h"




using namespace GfxLib;
using namespace DirectX;



namespace GlobalRootSignatureParams {
	enum Value {
		OutputViewSlot = 0,
		AccelerationStructureSlot,
		SamplerStateSlot,
		SkyTextures,
		SceneCb,
		Count
	};
}

namespace LocalRootSignatureParams {
	enum Value {
		ViewportConstantSlot = 0,
		Count
	};
}


namespace {



	namespace Config
	{

		enum {

			MaxRecursionDepth = 4,

		};

	}

	enum {

		TRACE_TYPE_NUM = 3,

	};

	const wchar_t* c_hitGroupName[] = {
		L"MyHitGroup",
		L"MyHitGroup_Low",
		L"MyHitGroup_Shadow",
	};
	const wchar_t* c_raygenShaderName = L"MyRaygenShader";
	const wchar_t* c_closestHitShaderName[] = {
		L"MyClosestHitShader",
		L"MyClosestHitShader_Low",
		L"MyClosestHitShader_Shadow",
	};


	_STATIC_ASSERT(TRACE_TYPE_NUM == _countof(c_hitGroupName));
	_STATIC_ASSERT(TRACE_TYPE_NUM == _countof(c_closestHitShaderName));


	const wchar_t* c_missShaderName[] = {
		L"MyMissShader",
		L"MyMissShader_Shadow",
	};


}



RayTracingRenderer::RayTracingRenderer()
{



}



RayTracingRenderer::~RayTracingRenderer()
{

	Finalize();


}



bool	RayTracingRenderer::Initialize()
{

	if (CoreSystem::GetInstance()->GetRaytracingTier() < D3D12_RAYTRACING_TIER_1_0) {

		GFX_WARN(L"This Device Does not support RayTracing...");

		return false;

	}

	m_rtShaderLib.CreateFromFile(L"Media/Shader/GfxRayTracing.cso");
	m_rtShaderLibModel.CreateFromFile(L"Media/Shader/GfxRayTracingModel.cso");
	m_rtShaderLibMiss.CreateFromFile(L"Media/Shader/GfxRayTracingMiss.cso");

	{
		D3D12_SAMPLER_DESC desc = GfxLib::Sampler::GetDefaultDesc();
		m_sampsLinear.Initialize(desc);
	}


	_CreateRayTracingRootSignature();

	_CreateRayTracingPipelineStateObject();



	uint32_t pixelColor = 0xffffffff;

	D3D12_SUBRESOURCE_DATA	subData = {};
	subData.pData = &pixelColor;
	subData.RowPitch = sizeof(pixelColor);

	m_texDummy[DUMMY_TEX_WHITE].Initialize(Format::R8G8B8A8_UNORM, 1, 1, 1, 1, &subData);

	return true;


}



void	RayTracingRenderer::Finalize(bool bDelayed)
{

	m_globalRootSig.Finalize(bDelayed);
	m_localRootSig.Finalize(bDelayed);
	m_localRootSigMiss.Finalize(bDelayed);
	m_localRootSigRayGen.Finalize(bDelayed);
	m_rtShaderLib.Finalize(bDelayed);
	m_rtShaderLibModel.Finalize(bDelayed);
	m_rtShaderLibMiss.Finalize(bDelayed);
	m_rtStateObject.Finalize(bDelayed);


	m_sampsLinear.Finalize();


	for (auto& tex : m_texDummy) {
		tex.Finalize(bDelayed);
	}

}




void	RayTracingRenderer::Render(GfxLib::GraphicsCommandList& cmdList, RtSceneTargets& rtTargets , const RayTracing::SceneInfo& sceneInfo )
{

	rtTargets.SwapIdx();



	if ( rtTargets.m_bFirstDraw ){

		cmdList.ResourceTransitionBarrier(&rtTargets.GetReadSceneHDR(), GfxLib::ResourceStates::ShaderResource, GfxLib::ResourceStates::UnorderedAccess);
		cmdList.ResourceTransitionBarrier(&rtTargets.GetWriteSceneHDR(), GfxLib::ResourceStates::ShaderResource, GfxLib::ResourceStates::UnorderedAccess);


		auto& tex0 = rtTargets.GetReadSceneHDR();
		auto& tex1 = rtTargets.GetWriteSceneHDR();

		float zeroValue[4] = {};

		auto descBuff = cmdList.AllocateDescriptorBuffer(2);
		descBuff.CopyHandle(0, tex0.GetUavDescHandle());
		descBuff.CopyHandle(1, tex1.GetUavDescHandle());

		cmdList.GetD3DCommandList()->ClearUnorderedAccessViewFloat(descBuff.GetGPUDescriptorHandle(0), tex0.GetUavDescHandle(), tex0.GetD3DResource(), zeroValue, 0, nullptr);
		cmdList.GetD3DCommandList()->ClearUnorderedAccessViewFloat(descBuff.GetGPUDescriptorHandle(1), tex1.GetUavDescHandle(), tex1.GetD3DResource(), zeroValue, 0, nullptr);



		cmdList.ResourceTransitionBarrier(&rtTargets.GetReadSceneHDR(), GfxLib::ResourceStates::UnorderedAccess, GfxLib::ResourceStates::ShaderResource);
		cmdList.ResourceTransitionBarrier(&rtTargets.GetWriteSceneHDR(), GfxLib::ResourceStates::UnorderedAccess, GfxLib::ResourceStates::ShaderResource);

		rtTargets.m_bFirstDraw = false;
	}




	D3D12_CPU_DESCRIPTOR_HANDLE outputUAV = rtTargets.GetOutputTexture().GetUavDescHandle();

	cmdList.ResourceTransitionBarrier(&rtTargets.GetOutputTexture(), GfxLib::ResourceStates::ShaderResource, GfxLib::ResourceStates::UnorderedAccess);
	cmdList.ResourceTransitionBarrier(&rtTargets.GetWriteSceneHDR(), GfxLib::ResourceStates::ShaderResource, GfxLib::ResourceStates::UnorderedAccess);


	const void* rayGenShaderIdentifier = m_rtStateObject.GetShaderIdentifier(c_raygenShaderName);
	const void* missShaderIdentifier0 = m_rtStateObject.GetShaderIdentifier(c_missShaderName[0]);
	const void* missShaderIdentifier1 = m_rtStateObject.GetShaderIdentifier(c_missShaderName[1]);

	const uint32_t shaderIdentifierSize = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES;

	const uint32_t instanceCount = 1;

	RayTracing::RayGenConstantBuffer	rayGenCB = {};
	rayGenCB.mtxCamera = XMMatrixTranspose(sceneInfo.mtxCamera);
	rayGenCB.viewport = sceneInfo.vp;
	rayGenCB.stencil = sceneInfo.vp;
	rayGenCB.globalTime = (float)sceneInfo.globalTime;
	rayGenCB.sceneRandom = float(sceneInfo.globalTime - floor(sceneInfo.globalTime));


	if (rtTargets.m_Prev.enable) {

		XMVECTOR det;
		// 前フレームのビュー行列
		XMMATRIX invCamera = XMMatrixInverse(&det, rtTargets.m_Prev.mtxCamera);

		XMFLOAT3X4	m34;

		XMStoreFloat3x4(&m34, (invCamera));

		rayGenCB.mtxCurToPrevView = m34;

	} 
	
	{
		rtTargets.m_Prev.enable = true;
		rtTargets.m_Prev.mtxCamera = sceneInfo.mtxCamera;
	}



	Texture2D* const texSky = sceneInfo.texSky;
	Texture2D* const texSkyRem = sceneInfo.texSkyRem;
	Texture2D* const texSkyIem = sceneInfo.texSkyIem;

	RtModel* const rtModel = sceneInfo.rtModel;

	//  CommandList上に、シェーダテーブルを構築する

	// for Model
	struct RootArguments {
		//RayGenConstantBuffer cb;
		D3D12_GPU_VIRTUAL_ADDRESS	cb;			//	RayGenConstantBuffer
		D3D12_GPU_DESCRIPTOR_HANDLE	indexSRV;	//	IndexSRV	
		D3D12_GPU_DESCRIPTOR_HANDLE	vtxSRV;		//	VtxSRV
		D3D12_GPU_DESCRIPTOR_HANDLE	texSRV;		//	Textures
	} rootArguments;

	// for RayGen
	struct RootArguments_RayGen {
		//RayGenConstantBuffer cb;
		D3D12_GPU_VIRTUAL_ADDRESS	cb;			//	RayGenConstantBuffer
		D3D12_GPU_DESCRIPTOR_HANDLE	uav;		//	UAV
		D3D12_GPU_DESCRIPTOR_HANDLE	texSRV;		//	Textures
	} rootArguments_RayGen;

	// for Miss
	struct RootArguments_Miss {
		//RayGenConstantBuffer cb;
		D3D12_GPU_DESCRIPTOR_HANDLE	srv;	//	IndexSRV	
	} rootArguments_Miss;


	//rootArguments.cb = m_rayGenCB;


	// LocalRootArguments for RayGen
	{
		GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(rayGenCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
		memcpy(cbBuffer.GetCpuAddr(), &rayGenCB, sizeof(rayGenCB));
		rootArguments_RayGen.cb = cbBuffer.GetGpuAddr();

		GfxLib::DescriptorBuffer	uavDesc = cmdList.AllocateDescriptorBuffer(1);
		uavDesc.CopyHandle(0, rtTargets.GetWriteSceneHDR().GetUavDescHandle());

		GfxLib::DescriptorBuffer	srvDesc = cmdList.AllocateDescriptorBuffer(1);
		srvDesc.CopyHandle(0, rtTargets.GetReadSceneHDR().GetSrvDescHandle());

		rootArguments_RayGen.uav = uavDesc.GetGPUDescriptorHandle();
		rootArguments_RayGen.texSRV = srvDesc.GetGPUDescriptorHandle();


	}

	// LocalRootArguments for miss
	{

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1);
		vtxSrvDesc.CopyHandle(0, texSky->GetSrvDescHandle());
		rootArguments_Miss.srv = vtxSrvDesc.GetGPUDescriptorHandle();

	}

	GfxLib::ShaderTable  rayGenShaderTable(cmdList, 1, shaderIdentifierSize + sizeof(rootArguments_RayGen));
	rayGenShaderTable.AddRecord(rayGenShaderIdentifier, &rootArguments_RayGen, sizeof(rootArguments_RayGen));

	GfxLib::ShaderTable  missShaderTable(cmdList, 2, shaderIdentifierSize + sizeof(RootArguments_Miss));
	missShaderTable.AddRecord(missShaderIdentifier0, &rootArguments_Miss, sizeof(rootArguments_Miss));
	missShaderTable.AddRecord(missShaderIdentifier1, &rootArguments_Miss, sizeof(rootArguments_Miss));


	auto& subGroupVec = sceneInfo.rtModel->GetSubGroups();
	GfxLib::ShaderTable  hitGroupShaderTable(cmdList, TRACE_TYPE_NUM * (uint32_t)subGroupVec.size(), shaderIdentifierSize + sizeof(RootArguments));

	{

		// LocalRootSignatureだから配列じゃなくてよいか？

		// モデル全体のパラメータ


#if 0
		GfxLib::DescriptorBuffer	indexSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		indexSrvDesc.CopyHandle(0, m_rtGeometry.GetIndexBufferSRV());
		rootArguments.indexSRV = indexSrvDesc.GetGPUDescriptorHandle();

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		vtxSrvDesc.CopyHandle(0, m_rtGeomAttrib.GetSrvDescHandle());
		rootArguments.vtxSRV = vtxSrvDesc.GetGPUDescriptorHandle();

#endif

		GfxLib::DescriptorBuffer	indexSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		indexSrvDesc.CopyHandle(0, rtModel->GetIndexBufferSRV());
		rootArguments.indexSRV = indexSrvDesc.GetGPUDescriptorHandle();

		GfxLib::DescriptorBuffer	vtxSrvDesc = cmdList.AllocateDescriptorBuffer(1/*IndexBufferCount*/);
		vtxSrvDesc.CopyHandle(0, rtModel->GetGeomAttributeSRV());
		rootArguments.vtxSRV = vtxSrvDesc.GetGPUDescriptorHandle();



		for (auto subgroup : subGroupVec) {
			//RootArguments rootArgMaterial = rootArguments;

			// マテリアル毎のパラメータ
			auto* material = rtModel->GetMaterials()[subgroup->GetMaterialId()];

			RayTracing::ModelConstantBuffer  modelCB = {};

			modelCB.isIndex16bit = rtModel->GetIndexFormat() == GfxLib::Format::R16_UINT;
			modelCB.primitiveOffset = subgroup->GetStartIndex() / 3;

			modelCB.roughness = material->GetRoughness();
			modelCB.diffuseAlbedo = material->GetDiffuseColor();
			modelCB.specularAlbedo = material->GetSpecularColor();
			modelCB.emissive = material->GetEmissiveColor();


			GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(modelCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			memcpy(cbBuffer.GetCpuAddr(), &modelCB, sizeof(modelCB));
			rootArguments.cb = cbBuffer.GetGpuAddr();


			// Texture
			GfxLib::DescriptorBuffer	texSrvDesc = cmdList.AllocateDescriptorBuffer(1);

			const TextureBase* pTex = material->GetDiffuseTex();
			if (!pTex) pTex = &m_texDummy[DUMMY_TEX_WHITE];
			texSrvDesc.CopyHandle(0, pTex->GetSrvDescHandle());
			rootArguments.texSRV = texSrvDesc.GetGPUDescriptorHandle();

			for (uint32_t i = 0; i < TRACE_TYPE_NUM; ++i) {

				// TraceType毎にLocalRootSignatureは同じにしている（別でもよい）

				const void* hitGroupShaderIdentifier = m_rtStateObject.GetShaderIdentifier(c_hitGroupName[i]);
				hitGroupShaderTable.AddRecord(hitGroupShaderIdentifier, &rootArguments, sizeof(RootArguments));
			}
		}
	}


	auto DispatchRays = [&](auto* commandList, auto* stateObject, D3D12_DISPATCH_RAYS_DESC* dispatchDesc)
	{
		// Since each shader table has only one shader record, the stride is same as the size.

		dispatchDesc->HitGroupTable = hitGroupShaderTable.GetGpuVirtualAddressRangeAndStride();
		dispatchDesc->MissShaderTable = missShaderTable.GetGpuVirtualAddressRangeAndStride();
		dispatchDesc->RayGenerationShaderRecord = rayGenShaderTable.GetGpuVirtualAddressRange();

		dispatchDesc->Width = (UINT)sceneInfo.Width;
		dispatchDesc->Height = (UINT)sceneInfo.Height;
		dispatchDesc->Depth = 1;
		commandList->SetPipelineState1(stateObject);
		commandList->DispatchRays(dispatchDesc);
	};


	auto* commandList = cmdList.GetD3DCommandList4();
	commandList->SetComputeRootSignature(m_globalRootSig.GetD3DRootSignature());

	cmdList.ReserveDescriptorBuffers(2, 0);


	/*
	GfxLib::DescriptorBuffer  descBuff = cmdList.AllocateDescriptorBuffer(1);
	commandList->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, m_raytracingOutputResourceUAVGpuDescriptor);
	commandList->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, m_topLevelAccelerationStructure->GetGPUVirtualAddress());

	descBuff.CopyHandle(0, m_rtOutput.GetUavDescHandle());
	*/

	D3D12_DISPATCH_RAYS_DESC dispatchDesc = {};

	GfxLib::DescriptorBuffer db1 = cmdList.AllocateDescriptorBuffer(1);
	db1.CopyHandle(0, outputUAV);

	GfxLib::DescriptorBuffer db2 = cmdList.AllocateDescriptorBuffer_Sampler(1);
	db2.CopyHandle(0, m_sampsLinear.GetDescHandle());


	GfxLib::DescriptorBuffer db3 = cmdList.AllocateDescriptorBuffer(3);
	db3.CopyHandle(0, texSky->GetSrvDescHandle());
	db3.CopyHandle(1, texSkyRem->GetSrvDescHandle());
	db3.CopyHandle(2, texSkyIem->GetSrvDescHandle());


	GfxLib::GpuBufferRange cbBuffer = cmdList.AllocateGpuBuffer(sizeof(rayGenCB), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
	memcpy(cbBuffer.GetCpuAddr(), &rayGenCB, sizeof(rayGenCB));

	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::OutputViewSlot, db1.GetGPUDescriptorHandle());
	cmdList.GetD3DCommandList()->SetComputeRootShaderResourceView(GlobalRootSignatureParams::AccelerationStructureSlot, sceneInfo.TLAS->GetD3DResource()->GetGPUVirtualAddress());
	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::SamplerStateSlot, db2.GetGPUDescriptorHandle());
	cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::SkyTextures, db3.GetGPUDescriptorHandle());
	//cmdList.GetD3DCommandList()->SetComputeRootDescriptorTable(GlobalRootSignatureParams::SkyTextures, db3.GetGPUDescriptorHandle());
	cmdList.GetD3DCommandList()->SetComputeRootConstantBufferView(GlobalRootSignatureParams::SceneCb, cbBuffer.GetGpuAddr());


	DispatchRays(cmdList.GetD3DCommandList4(), m_rtStateObject.GetD3DStateObject(), &dispatchDesc);


	cmdList.ResourceTransitionBarrier(&rtTargets.GetOutputTexture(), GfxLib::ResourceStates::UnorderedAccess, GfxLib::ResourceStates::ShaderResource);
	cmdList.ResourceTransitionBarrier(&rtTargets.GetWriteSceneHDR(), GfxLib::ResourceStates::UnorderedAccess, GfxLib::ResourceStates::ShaderResource);
}




void	RayTracingRenderer::_CreateRayTracingRootSignature()
{


::RootSignatureDesc	rootSigDesc;


	rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Uav,1,0 }, 1);
	rootSigDesc.AddParam_Srv(0);
	rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Sampler,1,0 }, 1);
	rootSigDesc.AddParam_DescriptorTable(&GfxLib::DESCRIPTOR_RANGE{ GfxLib::DescriptorRangeType::Srv,3,1 }, 1);
	rootSigDesc.AddParam_Cbv(0);

	m_globalRootSig.Initialize(rootSigDesc);

	rootSigDesc.Clear();


	//	Model ClosestHit Local Signature

	//rootSigDesc.AddParam_32BitConstants(sizeof(m_rayGenCB) / sizeof(uint32_t), 0);
	rootSigDesc.AddParam_Cbv(16);
	GfxLib::DESCRIPTOR_RANGE	ranges = { GfxLib::DescriptorRangeType::Srv , 1,16,1 };
	rootSigDesc.AddParam_DescriptorTable(&ranges, 1);

	GfxLib::DESCRIPTOR_RANGE	ranges2 = { GfxLib::DescriptorRangeType::Srv , 1,16,2 };
	rootSigDesc.AddParam_DescriptorTable(&ranges2, 1);


	GfxLib::DESCRIPTOR_RANGE	ranges3 = { GfxLib::DescriptorRangeType::Srv , 1,17,0 };
	rootSigDesc.AddParam_DescriptorTable(&ranges3, 1);


	rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

	m_localRootSig.Initialize(rootSigDesc);

	rootSigDesc.Clear();

	//	Miss Shader

	{
		GfxLib::DESCRIPTOR_RANGE	ranges = { GfxLib::DescriptorRangeType::Srv , 1,16,0 };
		rootSigDesc.AddParam_DescriptorTable(&ranges, 1);
	}
	rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

	m_localRootSigMiss.Initialize(rootSigDesc);


	// RayGen
	{
		rootSigDesc.Clear();

		rootSigDesc.AddParam_Cbv(16);


		GfxLib::DESCRIPTOR_RANGE	ranges = { GfxLib::DescriptorRangeType::Uav , 1,1,0 };
		rootSigDesc.AddParam_DescriptorTable(&ranges, 1);

		GfxLib::DESCRIPTOR_RANGE	ranges2 = { GfxLib::DescriptorRangeType::Srv , 1,16,0 };
		rootSigDesc.AddParam_DescriptorTable(&ranges2, 1);

		rootSigDesc.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE);

		m_localRootSigRayGen.Initialize(rootSigDesc);
	}
}


void	RayTracingRenderer::_CreateRayTracingPipelineStateObject()
{

	GfxLib::RtPipelineStateDesc	stateDesc;


	{
		auto* subobj = stateDesc.CreateSubObject<GfxLib::PipelineState_DxilLibrary>();

		D3D12_SHADER_BYTECODE dxillib = m_rtShaderLib.GetD3D12ShaderBytecode();
		subobj->SetDXILLibrary(dxillib);
		subobj->AddExport(c_raygenShaderName);
		//for (auto s : c_missShaderName)		subobj->AddExport(s);
	}

	{
		auto* subobj = stateDesc.CreateSubObject<GfxLib::PipelineState_DxilLibrary>();

		D3D12_SHADER_BYTECODE dxillib = m_rtShaderLibMiss.GetD3D12ShaderBytecode();
		subobj->SetDXILLibrary(dxillib);
		for (auto s : c_missShaderName)		subobj->AddExport(s);

	}

	{
		auto* subobj = stateDesc.CreateSubObject<GfxLib::PipelineState_DxilLibrary>();

		D3D12_SHADER_BYTECODE dxillib = m_rtShaderLibModel.GetD3D12ShaderBytecode();
		subobj->SetDXILLibrary(dxillib);
		for (auto s : c_closestHitShaderName)	subobj->AddExport(s);
	}

	for (uint32_t i = 0; i < TRACE_TYPE_NUM; ++i)
	{
		/*
			HitGroupはAnyHit、ClosestHit、ClosestHitで同じグループに入る
		*/

		auto* hitGroup = stateDesc.CreateSubObject<GfxLib::PipelineState_HitGroup>();

		hitGroup->SetClosestHitShaderImport(c_closestHitShaderName[i]);
		hitGroup->SetHitGroupExport(c_hitGroupName[i]);
		hitGroup->SetHitGroupType(D3D12_HIT_GROUP_TYPE_TRIANGLES);

	}


	{
		auto* shaderConfig = stateDesc.CreateSubObject<GfxLib::PipelineState_RaytracingShaderConfig>();

		UINT payloadSize = 4 * sizeof(float);   // float4 color
		UINT attributeSize = 2 * sizeof(float); // float2 barycentrics
		shaderConfig->Config(payloadSize, attributeSize);
	}

	{
		//	LocalRootSignature Associate

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(m_localRootSig.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		for (auto s : c_closestHitShaderName) rootSignatureAssociation->AddExport(s);
	}

	{
		//	LocalRootSignature Associate [raygen]

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(m_localRootSigRayGen.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		rootSignatureAssociation->AddExport(c_raygenShaderName);
	}


	{
		//	Local Root Signature for [miss]

		auto localRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_LocalRootSignature>();
		localRootSignature->SetRootSignature(m_localRootSigMiss.GetD3DRootSignature());
		// Shader association
		auto rootSignatureAssociation = stateDesc.CreateSubObject<GfxLib::PipelineState_SubobjectToExportsAssociation>();
		rootSignatureAssociation->SetRootSignature(localRootSignature);
		for (auto s : c_missShaderName) rootSignatureAssociation->AddExport(s);
	}

	{
		//	Global Root Signature

		auto globalRootSignature = stateDesc.CreateSubObject<GfxLib::PipelineState_GlobalRootSignature>();
		globalRootSignature->SetRootSignature(m_globalRootSig.GetD3DRootSignature());
	}

	{
		// Pipeline Config
		auto pipelineConfig = stateDesc.CreateSubObject<GfxLib::PipelineState_PipelineConfig>();
		// PERFOMANCE TIP: Set max recursion depth as low as needed 
		// as drivers may apply optimization strategies for low recursion depths. 
		UINT maxRecursionDepth = Config::MaxRecursionDepth; // ~ primary rays only. 
		pipelineConfig->Config(maxRecursionDepth);

	}

	{
		//	


		stateDesc.Resolve();


		D3D12_STATE_OBJECT_DESC desc = {};

		desc.Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE;
		desc.pSubobjects = stateDesc.GetStateSubObjects();
		desc.NumSubobjects = stateDesc.GetNumStateSubObjects();


		m_rtStateObject.Initialize(desc);


	}

}