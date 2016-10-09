



#include "stdafx.h"
#include "GfxInputLayout.h"



using namespace GfxLib;



InputLayout::InputLayout()
	:m_Count(0)
	, m_Element(nullptr)
{


}



InputLayout::~InputLayout()
{

	Finalize();

}



bool	InputLayout::Initialize(uint32_t count, const D3D12_INPUT_ELEMENT_DESC *element)
{
	Finalize();

	if (count == 0) {

		//	エレメントカウントなし
		// よく考えたらこれは許容されるのではないだろうか？

		GFX_ERROR(L"[InputLayout] Element Count is zero");

		return false;
	}

	m_Element = new D3D12_INPUT_ELEMENT_DESC[count];

	for (uint32_t i = 0; i < count; ++i) {

		m_Element[i] = element[i];

	}

	m_Count = count;


	return true;

}



void	InputLayout::Finalize()
{

	if (m_Element) {
		delete[] m_Element;
	}


	m_Count = 0;
	m_Element = nullptr;

}