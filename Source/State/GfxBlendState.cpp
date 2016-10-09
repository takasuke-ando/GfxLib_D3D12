



#include "stdafx.h"
#include "GfxBlendState.h"



using namespace GfxLib;



BlendState::BlendState()
{


}



BlendState::~BlendState()
{



}



bool	BlendState::Initialize(const D3D12_BLEND_DESC &desc )
{

	m_Desc = desc;

	return true;

}



void	BlendState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}