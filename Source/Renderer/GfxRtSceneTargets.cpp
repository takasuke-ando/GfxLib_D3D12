



#include "stdafx.h"

#include "GfxRtSceneTargets.h"



using namespace GfxLib;


RtSceneTargets::RtSceneTargets()
{
	m_nIdx = 0;
	m_bFirstDraw = true;
}



RtSceneTargets::~RtSceneTargets()
{

	Finalize();

}



bool	RtSceneTargets::Initialize(const INITIALIZE_PARAM& InitParam )
{

	m_rtOutput.InitializeUAV(GfxLib::Format::R8G8B8A8_UNORM, InitParam.Width, InitParam.Height, 1, GfxLib::ResourceStates::ShaderResource);

	for (auto& tex : m_SceneHDR) {

		tex.InitializeUAV(GfxLib::Format::R16G16B16A16_FLOAT, InitParam.Width, InitParam.Height, 1, GfxLib::ResourceStates::ShaderResource);

	}


	m_nIdx = 0;
	m_bFirstDraw = true;

	return true;
}



void	RtSceneTargets::Finalize(bool bDelayed )
{

	m_rtOutput.Finalize(bDelayed);

	for (auto& tex : m_SceneHDR) {

		tex.Finalize(bDelayed);

	}

}




