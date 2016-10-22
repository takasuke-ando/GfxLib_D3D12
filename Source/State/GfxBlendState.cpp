



#include "stdafx.h"
#include "GfxBlendState.h"

#include "Util/GfxCrc32.h"


using namespace GfxLib;



BlendState::BlendState()
	:m_HashValue( 0 )
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
	

	return true;

}



void	BlendState::Finalize()
{

	memset(&m_Desc, 0, sizeof(m_Desc));


}