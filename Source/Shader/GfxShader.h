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

		//!	有効か？。読み込みが完了していたら、true
		bool	IsValid() const { return m_pShaderByteCode != nullptr; }
		

		/***************************************************************
		@brief	D3D12_SHADER_BYTECODE取得
		@par	[説明]
			ユーティリティ関数
			D3D12_SHADER_BYTECODEを作成する
		@param	なし
		*/
		D3D12_SHADER_BYTECODE	GetD3D12ShaderBytecode() const {
			return D3D12_SHADER_BYTECODE{ m_pShaderByteCode,m_BytecodeLength };
		}


	private:
		const void* m_pShaderByteCode;
		size_t m_BytecodeLength;


	};


	typedef Shader		PixelShader;
	typedef Shader		VertexShader;


}

#endif // !__INCLUDE_GFXSHADER_H__
