﻿/***************************************************************
	@brief
	@par	[説明]
	@param
***************************************************************/





#include "stdafx.h"

#include "GfxRootSignature.h"
#include "GfxRootSignatureDesc.h"
#include "System/GfxCoreSystem.h"
#include "Util/GfxCrc32.h"

using namespace GfxLib;


RootSignature::RootSignature()
	:m_HashValue( 0 )
{


}



RootSignature::~RootSignature()
{
	Finalize();
}



bool	RootSignature::Initialize(const RootSignatureDesc &desc)
{
	Finalize();




	const D3D12_ROOT_PARAMETER* const rootParamArray = desc.GetRootParameterArray();
	const uint32_t	rootParamCount = desc.GetRootParameterCount();

	
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};

	rootSigDesc.NumParameters = rootParamCount;
	rootSigDesc.pParameters = rootParamArray;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = desc.GetFlags();


	// BLOBにシリアライズ
	ID3DBlob *blob = nullptr;
	HRESULT hr;
	hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob,nullptr);

	if (FAILED(hr)) {

		GFX_ERROR("D3D12SerializeRootSignature error %08x" , hr);

		return false;
	}


	Crc32 crc;
	crc.Update(blob->GetBufferPointer(), blob->GetBufferSize());


	// D3D12 Root Signatureを作成
	ID3D12Device *pDevice = CoreSystem::GetInstance()->GetD3DDevice();

	hr = pDevice->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(m_d3dRootSignature.InitialAccept()));
	blob->Release(); 
	blob = nullptr;

	if (FAILED(hr)) {

		GFX_ERROR("CreateRootSignature error %08x", hr);
		return false;
	}

	m_HashValue = crc.GetValue();

	return true;
}



void	RootSignature::Finalize(bool delayed)
{

	if (delayed) {
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dRootSignature);
	}

	m_d3dRootSignature.Release();
	m_HashValue = 0;
}
