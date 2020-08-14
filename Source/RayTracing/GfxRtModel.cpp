/***************************************************************
	@file	GfxRtModel
	@par	[説明]

		レイトレーシングモデル

		RtModelInstanceに接続して使う


***************************************************************/


#include "stdafx.h"

#include "GfxRtModel.h"

#include "Model/GfxInterModelData.h"
#include "Resource/GfxTextureContainer.h"
#include "Resource/GfxTextureBase.h"

#if 1



using namespace GfxLib;
using namespace DirectX;



RtModel::RtModel()
{


}


RtModel::~RtModel()
{
	Finalize();
}


bool	RtModel::Initialize(const InterModelData& data, TextureContainer& texContainer)
{

	//	マテリアル初期化


	for (auto &it : data.GetMaterials()) {

		Material* mat = new Material;

		if (it->m_DiffuseMap != L"") {

			const TextureBase* pTex = texContainer.LoadTextureFromFile(it->m_DiffuseMap.c_str());

			mat->m_pDiffuseTex = pTex;


		}


		mat->m_DiffuseColor		= it->m_DiffuseColor;
		mat->m_SpecularColor	= it->m_SpecularColor;
		mat->m_Roughness		= it->m_Roughness;


		m_vecMaterial.push_back(mat);

	}




	//	ジオメトリを作成

	float scale = 1.f;

	auto& subMeshes = data.GetSubMeshes();
	auto& vertices = data.GetVertex();
	const uint32_t vertices_size = (uint32_t)vertices.size();
	const uint32_t totalTriangles = (uint32_t)data.GetTotalTriangleCount();


	uint32_t indices_size = (uint32_t)totalTriangles * 3;

	uint32_t* indices32= nullptr;
	uint16_t* indices16= nullptr;

	uint32_t indices_idx = 0;
	
	const bool isIndex16 = vertices_size <= 0xffff;
	if (isIndex16) {
		indices16 = new uint16_t[indices_size];
	} else {
		indices32 = new uint32_t[indices_size];
	}



	// インデックスバッファの作成
	for (auto subMesh : subMeshes) {


		auto& tris = subMesh->GetTriangle();

		uint32_t startIndex = indices_idx;

		for (auto& tri : tris) {

			if (isIndex16) {

				indices16[indices_idx++] = (uint16_t)tri.v0;
				indices16[indices_idx++] = (uint16_t)tri.v1;
				indices16[indices_idx++] = (uint16_t)tri.v2;

			} else {

				indices32[indices_idx++] = tri.v0;
				indices32[indices_idx++] = tri.v1;
				indices32[indices_idx++] = tri.v2;

			}

		}

		// サブメッシュごとに割り当てられた範囲を記録
		SubGroup *subg = new SubGroup(startIndex, (uint32_t)tris.size()*3);


		
		subg->m_materialId = subMesh->GetMaterial();

		m_vecSubGroup.push_back(subg);

	}


	GFX_ASSERT(indices_idx == indices_size, L"Index Count Unmatch");

	RtVertex* positions = new RtVertex[vertices_size];
	RtAttrib* attribs = new RtAttrib[vertices_size];

	// 頂点情報の作成
	uint32_t i = 0;
	for (auto& vert : vertices) {

		RtVertex& vtx = positions[i];
		RtAttrib& attr = attribs[i];

		vtx.v1 = vert.pos.x * scale;
		vtx.v2 = vert.pos.y * scale;
		vtx.v3 = vert.pos.z * scale;


		attr.BaseColor = Float3{ 1,1,1 };
		attr.Normal = vert.norm;
		attr.Uv = Float2{ vert.uv.x,vert.uv.y };


		++i;

	}


	m_rtGeomAttrib.InitializeImmutable(attribs, sizeof(RtAttrib), vertices_size, GfxLib::ResourceStates::ShaderResource);

	if (isIndex16) {

		m_rtGeometry.Initialize(positions, sizeof(RtVertex), vertices_size, indices16, GfxLib::Format::R16_UINT, indices_size);

	} else {

		m_rtGeometry.Initialize(positions, sizeof(RtVertex), vertices_size, indices32, GfxLib::Format::R32_UINT, indices_size);

	}


	delete[] positions;
	delete[] attribs;
	delete[] indices32;
	delete[] indices16;

	// サブグループに割り当てられるGEOMETRY_DESCを抽出

	for (auto subg : m_vecSubGroup) {


		D3D12_RAYTRACING_GEOMETRY_DESC geomDesc;
		m_rtGeometry.GetSubsetDesc( geomDesc , subg->GetStartIndex(), subg->GetIndexCount() );


		m_vecGeomDesc.push_back(geomDesc);

	}


	return true;
}



void	RtModel::Finalize(bool delayed)
{


	for (auto it : m_vecSubGroup) {
		delete it;
	}
	m_vecSubGroup.clear();
	for (auto it : m_vecMaterial) {
		delete it;
	}
	m_vecMaterial.clear();
	m_vecGeomDesc.clear();

	m_rtGeomAttrib.Finalize(delayed);
	m_rtGeometry.Finalize(delayed);


}



#endif