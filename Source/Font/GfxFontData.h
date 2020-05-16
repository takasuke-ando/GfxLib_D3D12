#pragma		once
#include <Util\GfxD3DPtr.h>
#include <Resource\GfxTexture2D.h>


/*

	シンプルなビットマップフォント描画ユーティリティ

*/



namespace GfxLib
{

	class	FontData
	{
	public:
		FontData();
		~FontData();

		BOOL	Initialize(  const wchar_t * szTextureFilePath );
		void	Finalize();

		Texture2D&		GetTexture()	{	return	m_FontTexture;	}

	private:

		D3DPtr< ID3D12Device >				m_pD3DDevice;
		
		Texture2D					m_FontTexture;


	};

}



