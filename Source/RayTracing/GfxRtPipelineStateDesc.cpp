




#include "stdafx.h"

#include "GfxRtPipelineStateDesc.h"
#include "GfxPipelineStateSubobject.h"



using namespace GfxLib;



RtPipelineStateDesc::RtPipelineStateDesc()
{
	m_numStateSubObjects = 0;
	m_pStateSubObject = nullptr;

}



RtPipelineStateDesc::~RtPipelineStateDesc()
{

	for (auto& it : m_vecSubobject) {

		delete it;

	}


	delete[] m_pStateSubObject;
	m_pStateSubObject = nullptr;
	m_numStateSubObjects = 0;
}


void	RtPipelineStateDesc::Resolve()
{

	uint32_t numSubobjects = 0;
	for (auto& it : m_vecSubobject) {

		numSubobjects +=it->PreResolve();

	}

	if (numSubobjects == 0)	return;


	m_pStateSubObject = new D3D12_STATE_SUBOBJECT[numSubobjects];

	D3D12_STATE_SUBOBJECT* pNow = m_pStateSubObject;
	
	for (auto& it : m_vecSubobject) {

		uint32_t written = it->Resolve(pNow);


		pNow += written;

	}

	m_numStateSubObjects = numSubobjects;


	GFX_ASSERT(pNow == m_pStateSubObject + numSubobjects, L"PreResolve and Resolve  mismatch!!");


}