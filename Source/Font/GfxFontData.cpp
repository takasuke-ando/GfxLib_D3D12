

#include "stdafx.h"

#include "GfxFontData.h"






namespace GfxLib
{

	
FontData::FontData()
{

}


FontData::~FontData()
{

}



/*

	フォントデータの読み込み
	実際はテクスチャの読み込み

*/
BOOL	FontData::Initialize( const wchar_t * szTextureFilePath )
{
	Finalize();

	BOOL bOk = m_FontTexture.InitializeFromFile(szTextureFilePath );

	if ( !bOk )
	{
		Finalize();
	}

	return bOk;
}


void	FontData::Finalize()
{
	m_pD3DDevice = NULL;
	m_FontTexture.Finalize();

}





}

