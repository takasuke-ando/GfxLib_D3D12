



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