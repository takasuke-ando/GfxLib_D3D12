#ifndef __INCLUDE_GFXBLENDSTATE_H__
#define __INCLUDE_GFXBLENDSTATE_H__

/***************************************************************
	@file	GfxBlendState
	@par	[説明]

		デプスステンシルステート
		D3D11風に、デプスステンシルステートをカプセル化する

***************************************************************/





namespace GfxLib
{

	class BlendState
	{

	public:
		BlendState();
		~BlendState();


		bool	Initialize( const D3D12_BLEND_DESC &desc );
		void	Finalize();


		const D3D12_BLEND_DESC&		GetDesc() const { return m_Desc; }

	private:
		D3D12_BLEND_DESC	m_Desc;

	};




}



#endif // !__INCLUDE_GFXBLENDSTATE_H__
