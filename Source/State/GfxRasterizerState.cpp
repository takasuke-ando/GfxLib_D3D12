



#include "stdafx.h"
#include "GfxRasterizerState.h"



using namespace GfxLib;



RasterizerState::RasterizerState()
{


}



RasterizerState::~RasterizerState()
{



}



bool	RasterizerState::Initialize(const D3D12_RASTERIZER_DESC &desc )
{

	m_Desc = desc;

	return true;

}



void	RasterizerState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}