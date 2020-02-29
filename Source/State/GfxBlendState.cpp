



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