#ifndef __INCLUDE_GFXDEPTHSTENCILSTATE_H__
#define __INCLUDE_GFXDEPTHSTENCILSTATE_H__

/***************************************************************
	@file	GfxDepthStencilState
	@par	[説明]

		デプスステンシルステート
		D3D11風に、デプスステンシルステートをカプセル化する

***************************************************************/





namespace GfxLib
{

	class DepthStencilState
	{

	public:
		DepthStencilState();
		~DepthStencilState();


		bool	Initialize( const D3D12_DEPTH_STENCIL_DESC &desc );
		void	Finalize();
		
		
		const D3D12_DEPTH_STENCIL_DESC&		GetDesc() const { return m_Desc; }


	private:
		D3D12_DEPTH_STENCIL_DESC	m_Desc;

	};




}



#endif // !__INCLUDE_GFXDEPTHSTENCILSTATE_H__
