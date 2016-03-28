





#include "stdafx.h"

#include "GfxFence.h"
#include "System/GfxCoreSystem.h"



using namespace GfxLib;




Fence::Fence()
	:m_bWaiting(false)
	,m_event(NULL)
	,m_waitingFenceValue(0)
{
}




Fence::~Fence()
{
	Finalize();
}



bool Fence::Initialize(CoreSystem *coreSystem, bool pollingMode)
{
	Finalize();

	
	HRESULT hr = coreSystem->GetD3DDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.InitialAccept()));

	if (FAILED(hr)) {

		GFX_ERROR_LOG( L"CreateFence failed %08x",hr);

		return false;
	}

	if ( !pollingMode ){
		m_event = ::CreateEventEx(nullptr, FALSE, FALSE, EVENT_ALL_ACCESS);

		GFX_ASSERT(m_event!=NULL,L"Create Event Failed!!")
	}


	return true;
}



void Fence::Finalize()
{
	GFX_ASSERT(m_bWaiting == false);
	m_fence.Release();
	if (m_event != NULL) {
		::CloseHandle(m_event);
		m_event = NULL;
	}
}




bool	Fence::Sync()
{

	if (IsPollingMode()) {


		return m_fence->GetCompletedValue() >= m_waitingFenceValue;


	}else {

		if (m_bWaiting) {

			::WaitForSingleObject(m_event, INFINITE);

			m_bWaiting = false;

		}


		return true;
	}



}




void	Fence::_Insert(CoreSystem * coreSystem )
{
	GFX_ASSERT(m_fence!=nullptr , L"GfxLib::Fence Is not initialized!");

	if (!IsPollingMode()) {

		GFX_ASSERT(m_bWaiting == false, L"GfxLib::Fence Is not initialized!");

		uint64_t fenceValue = coreSystem->GetNextFenceValue();

		// Signal‚æ‚è‘O‚É“o˜^
		m_fence->SetEventOnCompletion(fenceValue, m_event);

	}


	m_waitingFenceValue = coreSystem->Signal(m_fence);
	

	if (!IsPollingMode()) {
		m_bWaiting = true;
	}

}

