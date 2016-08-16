






#include "stdafx.h"

#include "Resource/GfxTexture2D.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;






Texture2D::Texture2D()
{

	m_SrvHandle.ptr = 0;


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


	SuperClass::Finalize();
}




/***************************************************************
@brief	テクスチャへの書き込み
@par	[説明]
Initialize後に呼び出し、初期化を行う
現状はD3D12_HEAP_TYPE_UPLOADに対して
あまり効率は良くないはず

UPLOADからDEFAULTへのコピーを行う

Initialize関数に初期化構造体を渡すようにするかも


typedef struct D3D11_SUBRESOURCE_DATA {
const void *pSysMem;
UINT       SysMemPitch;
UINT       SysMemSlicePitch;
} D3D11_SUBRESOURCE_DATA;

的な

しかしその場合呼び出し側でミップレベル数を知っておく必要がある

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
