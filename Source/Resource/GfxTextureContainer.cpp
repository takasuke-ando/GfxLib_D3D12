


#include "stdafx.h"



#include "Resource/GfxTextureContainer.h"
#include "GfxTextureBase.h"


using namespace GfxLib;



TextureContainer::TextureContainer()
{



}



TextureContainer::~TextureContainer()
{
	Finalize();
}



bool	TextureContainer::Initialize()
{

	m_strBasePath = L"";

	return true;
}


void	TextureContainer::Finalize(bool delayed)
{

	for (auto &it : m_mapTex)
	{

		TextureBase* tex = (it).second.pTex;

		delete tex;

	}

	m_mapTex.clear();

	m_strBasePath = L"";

}


/***************************************************************
	@brief
	@par	[説明]
	@param
*/
const TextureBase*	TextureContainer::LoadTextureFromFile(const wchar_t* filePath)
{

	std::wstring totalPath = m_strBasePath + filePath;


	auto it = m_mapTex.find(totalPath);


	if (it != m_mapTex.end()) {

		return (*it).second.pTex;

	}



	TextureBase* pTex = new TextureBase;

	bool bOk = pTex->InitializeFromFile(totalPath.c_str());

	if (bOk) {


		GFX_INFO(L"[TextureContainer]LoadTexture Success: <%s>\n", totalPath.c_str());

		TEX_ENTRY	Ent = {};
		Ent.pTex = pTex;

		m_mapTex.insert(std::pair < std::wstring, TEX_ENTRY >(totalPath, Ent));

		return pTex;

	} else {

		delete pTex;
		GFX_WARN(L"[TextureContainer]LoadTexture Failed: <%s>\n", totalPath.c_str());

		TEX_ENTRY	Ent = {};
		m_mapTex.insert(std::pair < std::wstring,TEX_ENTRY >(totalPath,Ent) );


		return nullptr;
	}





}






