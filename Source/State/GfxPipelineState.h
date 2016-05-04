#ifndef __INCLUDE_GFXPIPELINESTATE_H__
#define	__INCLUDE_GFXPIPELINESTATE_H__


namespace GfxLib
{

	class PipelineState
	{
	public:
		PipelineState();
		~PipelineState();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);



		bool	Initialize( const D3D12_GRAPHICS_PIPELINE_STATE_DESC &desc );

		ID3D12PipelineState*	GetD3DPipelineState() const { return m_d3dPSO; }

	private:

		D3DPtr< ID3D12PipelineState >	m_d3dPSO;

	};





}



#endif // !__INCLUDE_GFXPIPELINESTATE_H__
