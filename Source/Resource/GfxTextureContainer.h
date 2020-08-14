#ifndef		__GFXTEXTURECONTAINER_H__
#define		__GFXTEXTURECONTAINER_H__



#include <map>
#include <string>

namespace GfxLib
{

	class TextureBase;

	class TextureContainer
	{
	public:
		TextureContainer();
		~TextureContainer();


		bool	Initialize();
		void	Finalize(bool delayed=GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
			@brief
			@par	[説明]
			@param
		*/
		const TextureBase*	LoadTextureFromFile(const wchar_t* filePath);

		struct TEX_ENTRY
		{
			TextureBase* pTex;
		};


		void	SetBasePath(const wchar_t* basePath) {
			m_strBasePath = basePath;
		}


	protected:
	private:
		std::map<std::wstring, TEX_ENTRY>	m_mapTex;
		std::wstring m_strBasePath;

	};



}








#endif		//__GFXTEXTURECONTAINER_H__
