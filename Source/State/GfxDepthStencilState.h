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

		uint32_t	GetHashValue() const { return 	m_HashValue; }	//!<	ハッシュ値

	private:
		D3D12_DEPTH_STENCIL_DESC	m_Desc;
		uint32_t			m_HashValue;		//!<	ハッシュ値
		uint32_t			m_UniqId;

	};




}



#endif // !__INCLUDE_GFXDEPTHSTENCILSTATE_H__
