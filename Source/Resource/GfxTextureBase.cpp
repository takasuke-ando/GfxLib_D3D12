




#include "stdafx.h"

#include "Resource/GfxTextureBase.h"


#include "System/GfxCoreSystem.h"


#include "External/DirectX-Graphics-Samples/d3dx12.h"
#include "External/DirectX-Graphics-Samples/DDSTextureLoader.h"



using namespace GfxLib;





TextureBase::TextureBase()
{

	m_SrvHandle.ptr = 0;

}


TextureBase::~TextureBase()
{

	Finalize();

}



/***************************************************************
	@brief	ファイルから初期化
	@par	[説明]
		ファイル(dds)からテクスチャを読み込み初期化する
		2D/Cube/3Dなどの種類は読み込むまでわからない
	@param
*/
bool	TextureBase::InitializeFromFile(const wchar_t* filePath)
{

	Finalize();


	//ID3D12Resource *resource = nullptr;

	ID3D12Device* d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
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


	return true;

}




void	TextureBase::Finalize(bool delayed)
{

	if (m_SrvHandle.ptr != 0) {

		GfxLib::FreeDescriptorHandle(DescriptorHeapType::CBV_SRV_UAV, m_SrvHandle);
		m_SrvHandle.ptr = 0;

	}



	SuperClass::Finalize(delayed);

}
