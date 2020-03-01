

#include "stdafx.h"


#include "GfxSampler.h"

#include "System/GfxCoreSystem.h"




namespace GfxLib
{



	Sampler::Sampler( )
	{

		m_SampHandle = nullptr;

	}
	Sampler::~Sampler()
	{
		Finalize();
	}


	BOOL	Sampler::Initialize( const D3D12_SAMPLER_DESC &desc )
	{
		/*
		GFX_RELEASE( m_pSamplerState );
		HRESULT hr = pdev->CreateSamplerState( &bd , m_pSamplerState.InitialAccept() );

		if ( FAILED( hr ) ) 
			return FALSE;

		m_pd3dDev = pdev;
		*/

		m_SampHandle = new AutoDescriptorHandle< DescriptorHeapType::SAMPLER >();
		GfxLib::CoreSystem::GetInstance()->GetD3DDevice()->CreateSampler(&desc, *m_SampHandle);

		return TRUE;

	}

	void	Sampler::Finalize()
	{
		delete m_SampHandle;
		m_SampHandle = nullptr;
	}



	/*

		デフォルトの設定を返す
		SetSamplerState( NULL )を行ったときと同じ情報を返すようにしている

	*/
	D3D12_SAMPLER_DESC	Sampler::GetDefaultDesc()
	{

		/*
		D3D11_SAMPLER_DESC desc;
		ZeroMemory( &desc , sizeof( desc ) );


		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR    ;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP    ;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MinLOD   = (-FLT_MAX) ;
		desc.MaxLOD = (FLT_MAX) ;
		desc.MipLODBias = 0.f;
		desc.MaxAnisotropy = 16;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		//desc.BorderColor = float4(0.0f,0.0f,0.0f,0.0f) 
		*/


		D3D12_SAMPLER_DESC	sampsDesc = {
			D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.f,
			16,
			D3D12_COMPARISON_FUNC_ALWAYS,
			{ 0.f,0.f,0.f,0.f },
			-FLT_MAX,
			FLT_MAX,
		};

		return sampsDesc;


	}

}
