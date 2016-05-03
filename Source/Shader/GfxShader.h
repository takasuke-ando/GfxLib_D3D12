#ifndef __INCLUDE_GFXSHADER_H__
#define	__INCLUDE_GFXSHADER_H__


namespace GfxLib
{


	/***************************************************************
		@brief	シェーダクラス
		@par	[説明]
			シェーダリソースをカプセル化
			ファイルから読み込んだりする
			ランタイムコンパイル機能はない

			D3D12の仕様上インターフェースはなく、単にバイナリイメージを保持しているだけ
			
		@param
	*/
	class  Shader
	{
	public:
		Shader();
		~Shader();

		void	Finalize(bool delayDelete = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);

		bool	CreateFromFile(const wchar_t * path );



	private:
		const void* m_pShaderByteCode;
		size_t m_BytecodeLength;


	};


}

#endif // !__INCLUDE_GFXSHADER_H__
