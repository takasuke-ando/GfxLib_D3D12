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

		/***************************************************************
			@brief	ブレンドステートを作成する
			@par	[説明]
				ブレンドステートを作成する
				パイプラインステートキャッシュ生成を補助するため
				内部で多少の計算が行われる
			@param
		*/
		bool	Initialize( const D3D12_BLEND_DESC &desc );
		void	Finalize();


		const D3D12_BLEND_DESC&		GetDesc() const { return m_Desc; }

	private:
		D3D12_BLEND_DESC	m_Desc;
		uint32_t			m_HashValue;		//!<	ハッシュ値

	};




}



#endif // !__INCLUDE_GFXBLENDSTATE_H__
