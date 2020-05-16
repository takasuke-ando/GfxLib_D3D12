



#include "stdafx.h"
#include "GfxRasterizerState.h"

#include "Util/GfxCrc32.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;



RasterizerState::RasterizerState()
	:m_HashValue(0)
	, m_UniqId(-1)
{


}



RasterizerState::~RasterizerState()
{
	Finalize();
}



bool	RasterizerState::Initialize(const D3D12_RASTERIZER_DESC &desc )
{

	m_Desc = desc;

	//	ハッシュ計算
	Crc32 crc;
	crc.Update(&desc, sizeof(desc));
	m_HashValue = crc.GetValue();

	m_UniqId = CoreSystem::GetInstance()->MakeUniqId(desc);

	return true;

}



void	RasterizerState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));
	m_UniqId = -1;

}



D3D12_RASTERIZER_DESC	RasterizerState::GetDefaultDesc()
{


	//	ラスタライザステートの設定
	D3D12_RASTERIZER_DESC descRS = {};

	descRS.FillMode = D3D12_FILL_MODE_SOLID;
	descRS.CullMode = D3D12_CULL_MODE_NONE;
	descRS.FrontCounterClockwise = FALSE;
	descRS.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	descRS.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	descRS.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	descRS.DepthClipEnable = TRUE;
	descRS.MultisampleEnable = FALSE;
	descRS.AntialiasedLineEnable = FALSE;
	descRS.ForcedSampleCount = 0;
	descRS.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;


	return descRS;

}