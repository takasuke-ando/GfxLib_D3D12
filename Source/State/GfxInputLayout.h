#ifndef __INCLUDE_GFXINPUTLAYOUT_H__
#define __INCLUDE_GFXINPUTLAYOUT_H__

/***************************************************************
	@file	GfxInputLayout
	@par	[説明]

		インプットレイアウト
		D3D11風に、インプットレイアウトをカプセル化する

***************************************************************/





namespace GfxLib
{

	class InputLayout
	{

	public:
		InputLayout();
		~InputLayout();


		/***************************************************************
		@brief	インプットレイアウトの作成
		@par	[説明]
			インプットレイアウトの作成
			D3D12_INPUT_ELEMENT_DESC::SemanticName は内部にコピーされるので
			リテラル文字列でなくても構わない
		@param[in]	count:		文字列数
		@param[in]	element:	入力エレメント
		*/

		bool	Initialize( uint32_t count , const D3D12_INPUT_ELEMENT_DESC *element );
		void	Finalize();


		D3D12_INPUT_LAYOUT_DESC		GetInputLayoutDesc() const {
			return D3D12_INPUT_LAYOUT_DESC{ m_Element , m_Count };
		}

	private:

		uint32_t m_Count;
		D3D12_INPUT_ELEMENT_DESC	*m_Element;

		LPSTR				m_SemanticNameBuff;

		uint32_t			m_HashValue;		//!<	ハッシュ値

	};




}



#endif // !__INCLUDE_GFXINPUTLAYOUT_H__
