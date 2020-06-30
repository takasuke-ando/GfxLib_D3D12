/***************************************************************
	@brief	RootSignature作成情報
	@par	[説明]
	@param
***************************************************************/





#include "stdafx.h"
#include "GfxRootSignatureDesc.h"

#include "System/GfxDefines.h"



using namespace GfxLib;


RootSignatureDesc::RootSignatureDesc()
{
	m_Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
}


RootSignatureDesc::~RootSignatureDesc()
{

	Clear();

}


void RootSignatureDesc::Clear()
{

	for (auto &param : m_vecRootParameter)
	{
		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE) {
			// new したRange情報を削除
			delete[] param.DescriptorTable.pDescriptorRanges;
		}

	}
	m_vecRootParameter.clear();
	m_Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
}


void		RootSignatureDesc::AddParam_DescriptorTable(const DESCRIPTOR_RANGE *ranges, uint32_t numRanges, ShaderVisibility visb)
{

	if (numRanges <= 0) {
		return;
	}

	const bool bIsSampler = ranges->RangeType == DescriptorRangeType::Sampler;


	// D3D構造体に変換する
	// このアドレスはデストラクタまで保持する
	D3D12_DESCRIPTOR_RANGE	*d3dranges = new D3D12_DESCRIPTOR_RANGE[numRanges];

	// 一つ一つ変換する
	for (uint32_t i = 0; i < numRanges; ++i) {

		bool _bIsSampler = ranges[i].RangeType == DescriptorRangeType::Sampler;
		if (bIsSampler != _bIsSampler) {
			// Sampler と、CBV,SRV,UAVのいずれかを混ぜることはできません
			GFX_ERROR( L"Range type sampler can not used with CBV,SRV,UAV  in one Descriptor Table" );
			return;
		}

		D3D12_DESCRIPTOR_RANGE range = {};

		range.RangeType = (D3D12_DESCRIPTOR_RANGE_TYPE)ranges[i].RangeType;
		range.BaseShaderRegister = ranges[i].BaseShaderRegister;
		range.NumDescriptors = ranges[i].NumDescriptors;
		range.RegisterSpace = 0;
		range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

		d3dranges[i] = range;
	}

	D3D12_ROOT_PARAMETER	rootParameter = {};

	// RootParamterを保存
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameter.DescriptorTable.pDescriptorRanges = d3dranges;
	rootParameter.DescriptorTable.NumDescriptorRanges = numRanges;

	rootParameter.ShaderVisibility = (D3D12_SHADER_VISIBILITY)visb;	

	
	m_vecRootParameter.push_back(rootParameter);

}



void		RootSignatureDesc::AddParam_32BitConstants(uint32_t num32bitValues, uint32_t shaderRegister, ShaderVisibility visb)
{

	D3D12_ROOT_PARAMETER	rootParameter = {};

	
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;


	rootParameter.Constants.Num32BitValues = num32bitValues;
	rootParameter.Constants.ShaderRegister = shaderRegister;
	rootParameter.Constants.RegisterSpace = 0;

	rootParameter.ShaderVisibility = (D3D12_SHADER_VISIBILITY)visb;


	m_vecRootParameter.push_back(rootParameter);


}



void		RootSignatureDesc::AddParam_Cbv(uint32_t shaderRegister, ShaderVisibility visb)
{
	_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE_CBV, shaderRegister, visb);
}


void		RootSignatureDesc::AddParam_Srv(uint32_t shaderRegister, ShaderVisibility visb)
{
	_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE_SRV, shaderRegister, visb);
}


void		RootSignatureDesc::AddParam_Uav(uint32_t shaderRegister, ShaderVisibility visb)
{
	_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE_UAV, shaderRegister, visb);
}



void		RootSignatureDesc::_AddParam_CbvSrvUav(D3D12_ROOT_PARAMETER_TYPE type, uint32_t shaderRegister, ShaderVisibility visb)
{

	D3D12_ROOT_PARAMETER	rootParameter = {};

	rootParameter.ParameterType = type;

	rootParameter.Descriptor.ShaderRegister = shaderRegister;
	rootParameter.Descriptor.RegisterSpace = 0;
	rootParameter.ShaderVisibility = (D3D12_SHADER_VISIBILITY)visb;

	m_vecRootParameter.push_back(rootParameter);


}
