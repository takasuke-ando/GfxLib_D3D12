




#include "stdafx.h"

#include "GfxPipelineStateSubobject.h"



using namespace GfxLib;










PipelineState_DxilLibrary::PipelineState_DxilLibrary()
{

	memset(&m_desc, 0, sizeof(m_desc));


}



void PipelineState_DxilLibrary::AddExport(
	LPCWSTR Name,
	LPCWSTR ExportToRename /* = nullptr */,
	D3D12_EXPORT_FLAGS Flags /* = D3D12_EXPORT_FLAG_NONE*/)
{

	D3D12_EXPORT_DESC exportdesc = {};


	exportdesc.Name = m_stringContainer.Copy(Name);
	exportdesc.ExportToRename = m_stringContainer.Copy(ExportToRename);
	exportdesc.Flags = Flags;


	m_vecExportDesc.push_back(exportdesc);

}


uint32_t PipelineState_DxilLibrary::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{

	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY };

	subobj.pDesc = &m_desc;

	m_desc.NumExports = (uint32_t)m_vecExportDesc.size();
	if (m_desc.NumExports > 0) {
		m_desc.pExports = &m_vecExportDesc[0];
	} else {
		m_desc.pExports = nullptr;
	}

	*pDst = subobj;

	return 1;
}



PipelineState_HitGroup::PipelineState_HitGroup()
{
	memset(&m_desc, 0, sizeof(m_desc));
}


PipelineState_HitGroup::~PipelineState_HitGroup()
{

}


uint32_t PipelineState_HitGroup::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{

	m_desc.HitGroupExport = m_strHitGroupExport.empty() ? nullptr : m_strHitGroupExport.c_str();
	m_desc.AnyHitShaderImport = m_strAnyHitShaderExport.empty() ? nullptr : m_strAnyHitShaderExport.c_str();
	m_desc.ClosestHitShaderImport = m_strClosestHitShaderExport.empty() ? nullptr : m_strClosestHitShaderExport.c_str();
	m_desc.IntersectionShaderImport = m_strIntersectionShaderExport.empty() ? nullptr : m_strIntersectionShaderExport.c_str();


	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP };
	subobj.pDesc = &m_desc;

	*pDst = subobj;

	return 1;
}


PipelineState_RaytracingShaderConfig::PipelineState_RaytracingShaderConfig()
{

	memset(&m_desc, 0, sizeof(m_desc));

}


uint32_t PipelineState_RaytracingShaderConfig::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{



	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG };
	subobj.pDesc = &m_desc;


	*pDst = subobj;


	return 1;

}



PipelineState_LocalRootSignature::PipelineState_LocalRootSignature()
{

	memset(&m_desc, 0, sizeof(m_desc));


}


PipelineState_LocalRootSignature::~PipelineState_LocalRootSignature()
{




}



uint32_t PipelineState_LocalRootSignature::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{

	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_LOCAL_ROOT_SIGNATURE };


	subobj.pDesc = &m_desc;


	*pDst = subobj;

	m_pResolvedSubobject = pDst;

	return 1;
}




PipelineState_GlobalRootSignature::PipelineState_GlobalRootSignature()
{

	memset(&m_desc, 0, sizeof(m_desc));


}


PipelineState_GlobalRootSignature::~PipelineState_GlobalRootSignature()
{




}



uint32_t PipelineState_GlobalRootSignature::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{

	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE };


	subobj.pDesc = &m_desc;


	*pDst = subobj;
	m_pResolvedSubobject = pDst;

	return 1;
}


PipelineState_SubobjectToExportsAssociation::PipelineState_SubobjectToExportsAssociation()
	:m_pAssociateRootSig(nullptr)
{

	memset(&m_desc, 0, sizeof(m_desc));

}


PipelineState_SubobjectToExportsAssociation::~PipelineState_SubobjectToExportsAssociation()
{



}


uint32_t PipelineState_SubobjectToExportsAssociation::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{
	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_SUBOBJECT_TO_EXPORTS_ASSOCIATION };


	// 解決済みのアドレスを取得
	m_desc.pSubobjectToAssociate = m_pAssociateRootSig!=nullptr?m_pAssociateRootSig->GetResolvedSubobject():nullptr;

	m_desc.pExports = m_vecStr.size() > 0 ? &m_vecStr[0] : nullptr;
	m_desc.NumExports = (uint32_t)m_vecStr.size();


	subobj.pDesc = &m_desc;
	*pDst = subobj;


	return 1;
}





PipelineState_PipelineConfig::PipelineState_PipelineConfig()
{

	memset(&m_desc, 0, sizeof(m_desc));

}



uint32_t PipelineState_PipelineConfig::Resolve(D3D12_STATE_SUBOBJECT* pDst)
{

	D3D12_STATE_SUBOBJECT subobj = { D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG };



	subobj.pDesc = &m_desc;
	*pDst = subobj;

	return 1;
}


