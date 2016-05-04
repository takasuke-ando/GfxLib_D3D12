
/***************************************************************
	@brief	GfxPipelineState.cpp
		パイプラインステート
	@par	[説明]
		
		グラフィクスパイプラインステートをカプセル化する

		ひとまずシンプルなカプセル化を行っているが
		そのままだと使いにくいのはわかりきっているので
		オンザフライでキャッシュしつつ生成するよてい


/****************************************************************/







#include "stdafx.h"


#include "State/GfxPipelineState.h"
#include "System/GfxCoreSystem.h"


using namespace GfxLib;





PipelineState::PipelineState()
{
}



PipelineState::~PipelineState()
{
	Finalize();
}




/***************************************************************
@brief	開放
@par	[説明]
@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
GPUアクセス中のリソースに対して書き込みを行うことがない
*/
void	PipelineState::Finalize( bool delayed )
{
	if (delayed) {

		// 遅延開放
		CoreSystem::GetInstance()->GetDelayDelete().Regist(m_d3dPSO);

	}

	m_d3dPSO = nullptr;


}





bool	PipelineState::Initialize(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc)
{
	Finalize();

	ID3D12Device* dev = CoreSystem::GetInstance()->GetD3DDevice();


	HRESULT hr = dev->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(m_d3dPSO.InitialAccept()));


	if (FAILED(hr)) {
		GFX_ERROR_LOG("CreatePipeline State failed %08x",hr );

		return false;
	}

	


	return true;
}









