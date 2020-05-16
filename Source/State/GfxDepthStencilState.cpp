



#include "stdafx.h"
#include "GfxDepthStencilState.h"

#include "Util/GfxCrc32.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;



DepthStencilState::DepthStencilState()
	:m_HashValue(0)
	, m_UniqId(-1)
{


}



DepthStencilState::~DepthStencilState()
{



}



bool	DepthStencilState::Initialize(const D3D12_DEPTH_STENCIL_DESC &desc )
{

	m_Desc = desc;

	//	ハッシュ計算
	Crc32 crc;
	crc.Update(&desc, sizeof(desc));
	m_HashValue = crc.GetValue();
	m_UniqId = CoreSystem::GetInstance()->MakeUniqId(desc);

	return true;

}



void	DepthStencilState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));
	m_UniqId = -1;

}


D3D12_DEPTH_STENCIL_DESC  DepthStencilState::GetDefaultDesc()
{


	D3D12_DEPTH_STENCIL_DESC	DepthStencilState = {};

	DepthStencilState.DepthEnable = TRUE;
	DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	DepthStencilState.StencilEnable = FALSE;

	return DepthStencilState;
}