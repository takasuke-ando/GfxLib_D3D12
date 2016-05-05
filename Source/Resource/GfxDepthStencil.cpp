




#include "stdafx.h"

#include "Resource/GfxDepthStencil.h"
#include "System/GfxCoreSystem.h"




using namespace GfxLib;






DepthStencil::DepthStencil()
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
bool	DepthStencil::Initialize(Format format, uint32_t width, uint32_t height, uint32_t mipLevels /*= 1*/, bool EnableShaderResource /*= true */ )
{


	Finalize();
#if 0
	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();

	// レンダーターゲットとして初期化
	// 初期ステートはDEPTH_WRITE
	bool b = _Initialize_DepthStencil(format, width, height, mipLevels, EnableShaderResource);

	if (!b) {

		return false;
	}


	D3D12_RESOURCE_DESC resDesc = m_d3dRes->GetDesc();

	uint32_t mipLevels = resDesc.MipLevels;

	GFX_ASSERT(mipLevels > 0);


	m_DSVDescHeap.InitializeDSV(mipLevls);


	for (uint32_t lv = 0; lv < mipLevels; ++lv) {

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

		rtvDesc.Format = (DXGI_FORMAT)format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = lv;
		rtvDesc.Texture2D.PlaneSlice = 0;

		d3dDev->CreateRenderTargetView(m_d3dRes, &rtvDesc, m_RTVDescHeap.GetCPUDescriptorHandleByIndex(lv));

	
	}



	if (EnableShaderResource) {


		m_SRVDescHeap.InitializeCBV_SRV_UAV(1);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};


		srvDesc.Format = (DXGI_FORMAT)format;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = mipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.PlaneSlice = 0;
		srvDesc.Texture2D.ResourceMinLODClamp = 0;	//	?
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;


		d3dDev->CreateShaderResourceView(m_d3dRes, &srvDesc, m_SRVDescHeap.GetCPUDescriptorHandleByIndex(0));


		
	}


	return true;
#endif


	return false;

}


