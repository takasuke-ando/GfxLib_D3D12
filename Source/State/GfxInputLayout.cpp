



#include "stdafx.h"
#include "GfxInputLayout.h"

#include "Util/GfxCrc32.h"


using namespace GfxLib;



InputLayout::InputLayout()
	:m_Count(0)
	, m_Element(nullptr)
	,m_HashValue(0)
{


}



InputLayout::~InputLayout()
{

	Finalize();

}


/***************************************************************
	@brief	インプットレイアウトの作成
	@par	[説明]
		インプットレイアウトの作成
		D3D12_INPUT_ELEMENT_DESC::SemanticName は内部にコピーされるので
		リテラル文字列でなくても構わない
	@param
*/
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



	//	最初になめて、文字列バッファに必要なサイズを計算
	size_t stringbuffsize = 0;
	for (uint32_t i = 0; i < count; ++i) {

		size_t len = strlen(element[i].SemanticName);
		stringbuffsize += (len + 1);
	}


	if (stringbuffsize > 0) {
		m_SemanticNameBuff = new CHAR[stringbuffsize];
	}

	LPSTR nameCopyBuff = m_SemanticNameBuff;

	//	ハッシュ計算とコピー
	Crc32 crc;

	for (uint32_t i = 0; i < count; ++i) {

		m_Element[i] = element[i];

		size_t namelen = strlen(element[i].SemanticName);

		//	文字列コピーしてそのアドレスを設定
		strcpy_s(nameCopyBuff, m_SemanticNameBuff + stringbuffsize - nameCopyBuff , element[i].SemanticName);
		m_Element[i].SemanticName = nameCopyBuff;
		nameCopyBuff += (namelen+1);

		//	SemanticNameはポインタなので
		//	一旦NULLに変えて計算し、文字列は別途CRC計算機に投入する
		D3D12_INPUT_ELEMENT_DESC	tmpElem = element[i];

		tmpElem.SemanticName = nullptr;
		crc.Update(&tmpElem, sizeof(tmpElem));

		crc.Update(element[i].SemanticName, sizeof(*element[i].SemanticName) * namelen);

	}

	GFX_ASSERT(nameCopyBuff == m_SemanticNameBuff + stringbuffsize, "Algorithm error");


	m_Count = count;	
	m_HashValue = crc.GetValue();


	return true;

}



void	InputLayout::Finalize()
{

	if (m_Element) {
		delete[] m_Element;
	}

	if (m_SemanticNameBuff) {
		delete[] m_SemanticNameBuff;
		m_SemanticNameBuff = nullptr;
	}


	m_Count = 0;
	m_Element = nullptr;
	m_HashValue = 0;
}