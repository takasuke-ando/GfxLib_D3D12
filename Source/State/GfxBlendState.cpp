



#include "stdafx.h"
#include "GfxBlendState.h"

#include "Util/GfxCrc32.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;



BlendState::BlendState()
	:m_HashValue( 0 )
	, m_UniqId(0xffffffff)
{


}



BlendState::~BlendState()
{



}



bool	BlendState::Initialize(const D3D12_BLEND_DESC &desc )
{

	m_Desc = desc;

	//	ハッシュ計算
	Crc32 crc;
	crc.Update(&desc, sizeof(desc));
	m_HashValue = crc.GetValue();
	m_UniqId = CoreSystem::GetInstance()->MakeUniqId(desc);

	return true;

}



void	BlendState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));
	m_UniqId = 0xffffffff;

}



D3D12_BLEND_DESC	BlendState::GetDefaultDesc()
{
	// レンダーターゲットのブレンド設定
	D3D12_RENDER_TARGET_BLEND_DESC descRTBS = {
		FALSE,	FALSE,
		D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE,	D3D12_BLEND_ZERO,	D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	// ブレンドステートの設定
	D3D12_BLEND_DESC descBS = {};
	descBS.AlphaToCoverageEnable = FALSE;
	descBS.IndependentBlendEnable = FALSE;
	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i) {
		descBS.RenderTarget[i] = descRTBS;
	};

	return descBS;

}
