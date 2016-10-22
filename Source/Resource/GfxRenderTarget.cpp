





#include "stdafx.h"

#include "GfxRenderTarget.h"

#include "System/GfxCoreSystem.h"

#include "Util/GfxAutoDescriptorHandle.h"

using namespace GfxLib;




RenderTarget::RenderTarget()
	:m_paRTVDescHandle( nullptr )
{
}



RenderTarget::~RenderTarget()
{
	Finalize();
}



/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void	RenderTarget::Finalize(bool delayed )
{

	if (m_paRTVDescHandle) {


		D3D12_RESOURCE_DESC resDesc = m_d3dRes->GetDesc();
		uint32_t mipLevels = resDesc.MipLevels;

		//for (uint32_t i = 0; i < mipLevels; ++i) {

		//	GfxLib::FreeDescriptorHandle( DescriptorHeapType::RTV , m_paRTVDescHandle[i] );

		//}

		delete[] m_paRTVDescHandle;
		m_paRTVDescHandle = nullptr;

	}


	SuperClass::Finalize(delayed);

	//m_RTVDescHeap.Finalize(delayed);
	m_SRVDescHeap.Finalize(delayed);


}




bool	RenderTarget::Initialize( Format format, uint32_t width, uint32_t height, uint32_t _mipLevls  , bool EnableShaderResource )
{

	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();

	// レンダーターゲットとして初期化
	// 初期ステートはRENDER_TARGET
	bool b = _Initialize_RenderTarget(format, width, height, _mipLevls,  EnableShaderResource);

	if (!b) {

		return false;
	}


	D3D12_RESOURCE_DESC resDesc = m_d3dRes->GetDesc();

	uint32_t mipLevels = resDesc.MipLevels;

	GFX_ASSERT(mipLevels > 0);


	//m_RTVDescHeap.InitializeRTV(mipLevels);

	b = _CreateRTV((DXGI_FORMAT)format, mipLevels);

	if (!b) {

		return false;
	}


	/*
	m_paRTVDescHandle = new AutoDescriptorHandle< DescriptorHeapType::RTV >[mipLevels];

	
	for (uint32_t lv = 0; lv < mipLevels; ++lv) {

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

		rtvDesc.Format = (DXGI_FORMAT)format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = lv;
		rtvDesc.Texture2D.PlaneSlice = 0;


		//m_paRTVDescHandle[lv] = AllocateDescriptorHandle(DescriptorHeapType::RTV);

		d3dDev->CreateRenderTargetView(m_d3dRes, &rtvDesc, m_paRTVDescHandle[lv]);

	}
	*/


	
	if (EnableShaderResource) {


		m_SRVDescHeap.InitializeCBV_SRV_UAV(1);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		//	@TODO	マルチサンプル対応
		
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
}



/***************************************************************
@brief	初期化
@par	[説明]
	すでに作成済みのリソースを使って、初期化を行う
@param	rtResource	:	レンダーターゲットリソース
*/
bool	RenderTarget::Initialize(ID3D12Resource *rtResource)
{


	Finalize();

	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();

	// レンダーターゲットとして初期化
	bool b = _Initialize(rtResource);

	if (!b) {

		return false;
	}


	D3D12_RESOURCE_DESC resDesc = rtResource->GetDesc();

	uint32_t mipLevels = resDesc.MipLevels;
	DXGI_FORMAT format = resDesc.Format;

	b = _CreateRTV(format, mipLevels);

	

	return b;
}



/***************************************************************
	@brief	RTVハンドルを作成する
	@par	[説明]
	@param
*/
bool	RenderTarget::_CreateRTV(DXGI_FORMAT format, uint32_t mipLevels)
{
	
	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();


	
	m_paRTVDescHandle = new AutoDescriptorHandle< DescriptorHeapType::RTV >[mipLevels];


	for (uint32_t lv = 0; lv < mipLevels; ++lv) {

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};

		//	@TODO	マルチサンプル対応したい

		rtvDesc.Format = (DXGI_FORMAT)format;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.MipSlice = lv;
		rtvDesc.Texture2D.PlaneSlice = 0;


		//m_paRTVDescHandle[lv] = AllocateDescriptorHandle(DescriptorHeapType::RTV);

		d3dDev->CreateRenderTargetView(m_d3dRes, &rtvDesc, m_paRTVDescHandle[lv]);

	}


	return true;

}



/***************************************************************
@brief	Descriptorのコピーを行う
@par	[説明]
@param	dstHandle
*/
void	RenderTarget::CopySRVDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE dstHandle)
{

	if (!m_SRVDescHeap.IsValid()) {
		GFX_ASSERT(0);
		return;
	}


	CoreSystem *coreSystem = CoreSystem::GetInstance();
	ID3D12Device* d3dDev = coreSystem->GetD3DDevice();


	d3dDev->CopyDescriptorsSimple(1, dstHandle, m_SRVDescHeap.GetCPUDescriptorHandleByIndex(0), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


}



D3D12_CPU_DESCRIPTOR_HANDLE	RenderTarget::GetRTVDescriptorHandle() const 
{ 
	return m_paRTVDescHandle[0]; 
}
