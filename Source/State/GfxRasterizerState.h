#ifndef __INCLUDE_GFXRASTERIZERSTATE_H__
#define __INCLUDE_GFXRASTERIZERSTATE_H__

/***************************************************************
	@file	GfxRasterizerState
	@par	[説明]

		ラスタライザステート
		D3D11風に、ラスタライザステートをカプセル化する

***************************************************************/





namespace GfxLib
{

	class RasterizerState
	{

	public:
		RasterizerState();
		~RasterizerState();


		bool	Initialize( const D3D12_RASTERIZER_DESC &desc );
		void	Finalize();


		const D3D12_RASTERIZER_DESC&		GetDesc() const { return m_Desc; }

	private:
		D3D12_RASTERIZER_DESC	m_Desc;
		uint32_t			m_HashValue;		//!<	ハッシュ値

	};




}



#endif // !__INCLUDE_GFXBLENDSTATE_H__
