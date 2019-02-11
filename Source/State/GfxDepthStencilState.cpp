﻿



#include "stdafx.h"
#include "GfxDepthStencilState.h"

#include "Util/GfxCrc32.h"


using namespace GfxLib;



DepthStencilState::DepthStencilState()
	:m_HashValue(0)
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


	return true;

}



void	DepthStencilState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}