



#include "stdafx.h"
#include "GfxDepthStencilState.h"



using namespace GfxLib;



DepthStencilState::DepthStencilState()
{


}



DepthStencilState::~DepthStencilState()
{



}



bool	DepthStencilState::Initialize(const D3D12_DEPTH_STENCIL_DESC &desc )
{

	m_Desc = desc;

	return true;

}



void	DepthStencilState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}