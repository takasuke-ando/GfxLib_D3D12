






#include "stdafx.h"

#include "Resource/GfxTexture2D.h"
#include "System/GfxCoreSystem.h"
#include "Device/GfxCommandList.h"
#include "Device/GfxGraphicsCommandList.h"

#include "External/DirectX-Graphics-Samples/d3dx12.h"
#include "External/DirectX-Graphics-Samples/DDSTextureLoader.h"

using namespace GfxLib;






Texture2D::Texture2D()
{

	m_SrvHandle.ptr = 0;
	m_UavHandle.ptr = 0;

}



Texture2D::~Texture2D()
{

	Finalize();

}



/***************************************************************
@brief	初期化
@par	[説明]
2Dテクスチャとして初期化する
@param[in]	format	:	バッファフォーマット
@param[in]	width	:	幅
@param[in]	height	:	高さ
@param[in]	mipLevels:	ミップレベル
*/
bool	Texture2D::Initialize(Format format, uint32_t width, uint32_t height, uint32_t mipLevels)
{
	Finalize();


	if (!SuperClass::_Initialize_Texture2D_Test(format, width, height, mipLevels)) {

		GFX_ERROR(L"Texture2D Creation Error");

		return false;
	}


	// 基本デフォルトで作っておく
	// Unknownフォーマットなら無理...

	{
		ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();


		const D3D12_RESOURCE_DESC resDesc = GetD3DResource()->GetDesc();

		m_SrvHandle = GfxLib::AllocateDescriptorHandle(GfxLib::DescriptorHeapType::CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
			(DXGI_FORMAT)format,
			D3D12_SRV_DIMENSION_TEXTURE2D,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		};

		auto &tex2D = srvDesc.Texture2D;
		tex2D.MipLevels = resDesc.MipLevels;
		tex2D.MostDetailedMip = resDesc.MipLevels - 1;
		tex2D.PlaneSlice = 0;
		tex2D.ResourceMinLODClamp = 0;


		d3dDev->CreateShaderResourceView(GetD3DResource(), &srvDesc, m_SrvHandle);


	}



	return true;
}



/***************************************************************
@brief	初期化
@par	[説明]
2Dテクスチャとして初期化する
@param[in]	format	:	バッファフォーマット
@param[in]	width	:	幅
@param[in]	height	:	高さ
@param[in]	mipLevels:	ミップレベル
@param[in]  subData:	サブリソースデータ
*/
bool	Texture2D::Initialize(
	Format format, 
	uint32_t width, uint32_t height, 
	uint32_t mipLevels,
	uint32_t subDataNum,
	const D3D12_SUBRESOURCE_DATA subData[])
{


	Finalize();




	if (!SuperClass::_Initialize_Texture2D(format, width, height, mipLevels, D3D12_RESOURCE_STATE_COPY_DEST,D3D12_RESOURCE_FLAG_NONE)) {

		GFX_ERROR(L"Texture2D Creation Error");

		return false;
	}


	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();


	//	一時的なリソースを作成し、コピーする
	{
		// @TODO	マルチスレッド処理の場合はどうするのか


		auto *initCmdList = CoreSystem::GetInstance()->GetResourceInitCommandList();

		initCmdList->InitializeResource(GetD3DResource(), subDataNum, subData);

#if 0
		UINT64 uploadBufferSize = d3dx12::GetRequiredIntermediateSize(GetD3DResource(), 0, subDataNum);


		D3DPtr<ID3D12Resource>	d3dResForUpload;

		D3D12_HEAP_PROPERTIES heapProp = {};


		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;


		D3D12_RESOURCE_DESC resDesc = {};

		resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//	TextureへのUpload中間バッファは、Bufferとして作る必要がある
		resDesc.Alignment = 0;
		resDesc.Width = uploadBufferSize;
		resDesc.Height = 1;
		resDesc.DepthOrArraySize = 1;
		resDesc.MipLevels = 1;
		resDesc.Format = DXGI_FORMAT_UNKNOWN;
		resDesc.SampleDesc.Count = 1;
		resDesc.SampleDesc.Quality = 0;
		resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//	RowMajor→UNKNOWN へのコピー可能？
		resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



HRESULT hr = d3dDev->CreateCommittedResource(
	&heapProp,
	D3D12_HEAP_FLAG_NONE,
	&resDesc,
	D3D12_RESOURCE_STATE_GENERIC_READ,		//	変更不可
	nullptr,
	IID_PPV_ARGS(d3dResForUpload.InitialAccept())
	);


if (FAILED(hr)) {
	GFX_ERROR(L"Create Committed Resource Failed (%08x)", hr);
	return false;
}



d3dx12::UpdateSubresources(initCmdList->GetD3DCommandList(), GetD3DResource(), d3dResForUpload, 0, 0, subDataNum, subData);
initCmdList->ResourceTransitionBarrier(GetD3DResource(), ResourceStates::CopyDest, ResourceStates::GenericRead);


// 遅延開放に登録
CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);

#endif



	}


	//	Handle取得
	{


		const D3D12_RESOURCE_DESC resDesc = GetD3DResource()->GetDesc();

		m_SrvHandle = GfxLib::AllocateDescriptorHandle(GfxLib::DescriptorHeapType::CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
			(DXGI_FORMAT)format,
			D3D12_SRV_DIMENSION_TEXTURE2D,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		};

		auto &tex2D = srvDesc.Texture2D;
		tex2D.MipLevels = resDesc.MipLevels;
		tex2D.MostDetailedMip = resDesc.MipLevels - 1;
		tex2D.PlaneSlice = 0;
		tex2D.ResourceMinLODClamp = 0;


		d3dDev->CreateShaderResourceView(GetD3DResource(), &srvDesc, m_SrvHandle);


	}



	return true;
}


bool	Texture2D::InitializeUAV(
	Format format,
	uint32_t width,
	uint32_t height,
	uint32_t mipLevels, 
	ResourceStates states)
{

#if 1
	Finalize();




	if (!SuperClass::_Initialize_Texture2D(format, width, height, mipLevels, (D3D12_RESOURCE_STATES)states, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)) {

		GFX_ERROR(L"Texture2D Creation Error");

		return false;
	}


	ID3D12Device* d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();



	//	Handle取得
	{


		const D3D12_RESOURCE_DESC resDesc = GetD3DResource()->GetDesc();

		m_SrvHandle = GfxLib::AllocateDescriptorHandle(GfxLib::DescriptorHeapType::CBV_SRV_UAV);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {
			(DXGI_FORMAT)format,
			D3D12_SRV_DIMENSION_TEXTURE2D,
			D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		};

		auto& tex2D = srvDesc.Texture2D;
		tex2D.MipLevels = resDesc.MipLevels;
		tex2D.MostDetailedMip = resDesc.MipLevels - 1;
		tex2D.PlaneSlice = 0;
		tex2D.ResourceMinLODClamp = 0;


		d3dDev->CreateShaderResourceView(GetD3DResource(), &srvDesc, m_SrvHandle);


	}


	{


		const D3D12_RESOURCE_DESC resDesc = GetD3DResource()->GetDesc();

		m_UavHandle = GfxLib::AllocateDescriptorHandle(GfxLib::DescriptorHeapType::CBV_SRV_UAV);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {
			(DXGI_FORMAT)format,
			D3D12_UAV_DIMENSION_TEXTURE2D,
		};

		auto& tex2D = uavDesc.Texture2D;
		tex2D.MipSlice = 0;
		tex2D.PlaneSlice = 0;


		d3dDev->CreateUnorderedAccessView(GetD3DResource(),nullptr, &uavDesc, m_UavHandle);


	}

#endif

	return true;
}


/***************************************************************
	@brief	ファイルから初期化
	@par	[説明]
	@param
*/
bool	Texture2D::InitializeFromFile(const wchar_t *filePath)
{
	Finalize();


	//ID3D12Resource *resource = nullptr;

	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
	m_SrvHandle = GfxLib::AllocateDescriptorHandle(GfxLib::DescriptorHeapType::CBV_SRV_UAV);

	//	DDSファイルから、読み込む
	HRESULT hr = CreateDDSTextureFromFile(
		d3dDev,
		filePath,
		4096,
		false,
		m_d3dRes.InitialAccept(),
		m_SrvHandle
	);

	if (FAILED(hr)) {
		GFX_ERROR(L"InitializeFromFile (%08x)", hr);

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_SrvHandle);
		m_SrvHandle.ptr = 0;

		return false;
	}

	const D3D12_RESOURCE_DESC desc = GetD3DResource()->GetDesc();

	if ( desc.Dimension != D3D12_RESOURCE_DIMENSION_TEXTURE2D ){
		// テクスチャ2Dではなかった
		GFX_ERROR( L"Texture Dimension mismatch (%d)" , desc.Dimension );
		// コピー処理がキューイングされているため、遅延開放する必要がある
		Finalize(true);
		return false;
	}


	return true;

}




/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void	Texture2D::Finalize(bool delayed /* = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE*/ )
{

	if (m_SrvHandle.ptr != 0) {

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_SrvHandle);
		m_SrvHandle.ptr = 0;

	}

	if (m_UavHandle.ptr != 0) {

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_UavHandle);
		m_UavHandle.ptr = 0;

	}

	SuperClass::Finalize(delayed);
}




/***************************************************************
@brief	テクスチャへの書き込み
@par	[説明]
	Initialize後に呼び出し、初期化を行う
	現状はD3D12_HEAP_TYPE_UPLOADに対して
	あまり効率は良くない

@param
*/
bool	Texture2D::WriteToSubresource(uint32_t DstSubResource, const D3D12_BOX *pDstBox, const void* pSrcData, uint32_t SrcRowPitch, uint32_t SrcDepthPitch )
{

	if (!m_d3dRes) {
		GFX_ERROR(L"Texture2D::WriteToSubresource  invalid call");


		return true;
	}



	HRESULT hr = m_d3dRes->WriteToSubresource(DstSubResource, pDstBox, pSrcData, SrcRowPitch, SrcDepthPitch);


	if (FAILED(hr)) {
		GFX_ERROR(L"WriteToSubresource error  %08x",hr);

		return false;
	}





	return true;
}
