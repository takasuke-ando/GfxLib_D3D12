﻿




#include "stdafx.h"

#include "Resource/GfxDepthStencil.h"
#include "System/GfxCoreSystem.h"
#include "Util/GfxFormatUtil.h"



using namespace GfxLib;






DepthStencil::DepthStencil()
	:m_DSVFormat( DXGI_FORMAT_UNKNOWN )
{
}

DepthStencil::~DepthStencil()
{
	Finalize();
}


/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void	DepthStencil::Finalize(bool delayed /*= GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE */ )
{

	SuperClass::Finalize(delayed);

	m_DSVDescHeap.Finalize(delayed);
	m_SRVDescHeap.Finalize(delayed);

	m_DSVFormat = DXGI_FORMAT_UNKNOWN;

}



/***************************************************************
@brief	初期化
@par	[説明]
デプスステンシルを初期化する
@param[in]	format:	フォーマット
@param[in]	width:	幅
@param[in]	height:	高さ
@param[in]	mipLevels : ミップレベル。0: 完全ミップ
@param[in]	rtvEnable:	ShaderResource有効
*/
bool	DepthStencil::Initialize(Format format, 
	uint32_t width, uint32_t height, uint32_t _mipLevels /*= 1*/, 
	bool EnableShaderResource /*= true */ )
{


	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();



	if (!FormatUtil::IsDepthStencil(format)) {

		GFX_ERROR(L"This format is not DepthStencil ! (%d)", format);

		return false;
	}

	Format buffFormat, srvFormat, stencilSrvFormat;
	FormatUtil::GetBufferAndSrvFormatFromDepth(format, buffFormat, srvFormat, stencilSrvFormat);



	D3D12_CLEAR_VALUE depthClear = {};
	depthClear.Format = (DXGI_FORMAT)format;
	depthClear.DepthStencil.Depth = GFX_DEFAULT_DEPTH_CLEAR_VALUE;

	// レンダーターゲットとして初期化
	// 初期ステートはDEPTH_WRITE
	bool b = _Initialize_DepthStencil(buffFormat, width, height, _mipLevels, EnableShaderResource , &depthClear);

	if (!b) {

		return false;
	}


	D3D12_RESOURCE_DESC resDesc = m_d3dRes->GetDesc();

	uint32_t mipLevels = resDesc.MipLevels;

	GFX_ASSERT(mipLevels > 0);


	m_DSVDescHeap.InitializeDSV(mipLevels);


	for (uint32_t lv = 0; lv < mipLevels; ++lv) {

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.Format = (DXGI_FORMAT)format;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = lv;


		d3dDev->CreateDepthStencilView(m_d3dRes, &dsvDesc, m_DSVDescHeap.GetCPUDescriptorHandleByIndex(lv));

	
	}



	if (EnableShaderResource) {


		m_SRVDescHeap.InitializeCBV_SRV_UAV(1,false);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};


		srvDesc.Format = (DXGI_FORMAT)srvFormat;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = mipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0;	//	?
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		d3dDev->CreateShaderResourceView(m_d3dRes, &srvDesc, m_SRVDescHeap.GetCPUDescriptorHandleByIndex(0));
		
		
	}


	m_DSVFormat = (DXGI_FORMAT)format;


	return true;



}



/***************************************************************
@brief	Descriptorのコピーを行う
@par	[説明]
@param
*/
void	DepthStencil::CopySRVDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE dstHandle)
{

	if (!m_SRVDescHeap.IsValid()) {
		GFX_ASSERT(0);
		return;
	}


	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();


	d3dDev->CopyDescriptorsSimple(1, dstHandle, m_SRVDescHeap.GetCPUDescriptorHandleByIndex(0), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
}



