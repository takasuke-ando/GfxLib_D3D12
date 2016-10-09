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


		bool	Initialize( uint32_t count , const D3D12_INPUT_ELEMENT_DESC *element );
		void	Finalize();


	private:

		uint32_t m_Count;
		D3D12_INPUT_ELEMENT_DESC	*m_Element;

	};




}



#endif // !__INCLUDE_GFXINPUTLAYOUT_H__
