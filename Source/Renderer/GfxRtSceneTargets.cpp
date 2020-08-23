



#include "stdafx.h"

#include "GfxRtSceneTargets.h"



using namespace GfxLib;


RtSceneTargets::RtSceneTargets()
{


}



RtSceneTargets::~RtSceneTargets()
{

	Finalize();

}



bool	RtSceneTargets::Initialize(const INITIALIZE_PARAM& InitParam )
{

	m_rtOutput.InitializeUAV(GfxLib::Format::R8G8B8A8_UNORM, InitParam.Width, InitParam.Height, 1, GfxLib::ResourceStates::ShaderResource);


	return true;
}



void	RtSceneTargets::Finalize(bool bDelayed )
{

	m_rtOutput.Finalize(bDelayed);

}




