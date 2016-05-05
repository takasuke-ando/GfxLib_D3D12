
/***************************************************************
	@brief	GfxPipelineState.cpp
		パイプラインステート
	@par	[説明]
		
		グラフィクスパイプラインステートをカプセル化する

		ひとまずシンプルなカプセル化を行っているが
		そのままだと使いにくいのはわかりきっているので
		オンザフライでキャッシュしつつ生成するよてい


		シリアライズされたパイプラインステートは
		ID3D12PipelineState::GetCachedBlob() で取得できる
		これをD3D12_GRAPHICS_PIPELINE_STATE_DESC::CachedPSO フィールドに設定することで
		シリアライズから復元できる


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

	// test
	/*
	{
		ID3DBlob *blob = nullptr;
		m_d3dPSO->GetCachedBlob(&blob);


		D3D12_GRAPHICS_PIPELINE_STATE_DESC _desc = desc;


		_desc.CachedPSO.pCachedBlob = blob->GetBufferPointer();
		_desc.CachedPSO.CachedBlobSizeInBytes = blob->GetBufferSize();

		D3DPtr< ID3D12PipelineState >  pso;

		HRESULT hr = dev->CreateGraphicsPipelineState(&_desc, IID_PPV_ARGS(pso.InitialAccept()));

		blob->Release();
	}
	*/


	return true;
}









