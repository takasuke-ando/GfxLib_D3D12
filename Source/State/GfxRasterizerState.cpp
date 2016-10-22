



#include "stdafx.h"
#include "GfxRasterizerState.h"

#include "Util/GfxCrc32.h"


using namespace GfxLib;



RasterizerState::RasterizerState()
	:m_HashValue(0)
{


}



RasterizerState::~RasterizerState()
{



}



bool	RasterizerState::Initialize(const D3D12_RASTERIZER_DESC &desc )
{

	m_Desc = desc;

	//	ハッシュ計算
	Crc32 crc;
	crc.Update(&desc, sizeof(desc));
	m_HashValue = crc.GetValue();


	return true;

}



void	RasterizerState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}