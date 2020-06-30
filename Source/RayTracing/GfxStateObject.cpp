




#include "stdafx.h"

#include "RayTracing/GfxStateObject.h"
#include "System/GfxCoreSystem.h"




using namespace GfxLib;




StateObject::StateObject()
{
}

StateObject::~StateObject()
{
	Finalize();
}




bool	StateObject::Initialize(const D3D12_STATE_OBJECT_DESC& desc)
{

	HRESULT hr = CoreSystem::GetInstance()->GetD3DDevice5()->CreateStateObject(&desc, IID_PPV_ARGS(m_d3dStateObject.InitialAccept()));


	if (FAILED(hr)) {
		
		GFX_ERROR( L"Create State Object Failed !   error=0x%08x" , hr );

		return false;

	}

	return true;
}

void	StateObject::Finalize(bool delayed )
{

	if (delayed) {

		// 遅延開放の場合
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dStateObject);

	}

	m_d3dStateObject.Release();
}

