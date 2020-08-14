#ifndef __INCLUDE_GFXTEXTUREBASE_H__
#define __INCLUDE_GFXTEXTUREBASE_H__




#include "GfxResource.h"
#include "Util/GfxAutoDescriptorHandle.h"



namespace GfxLib
{



	/***************************************************************
		@brief	テクスチャ基底クラス
		@par	[説明]
			特にタイプ（2D/3D/Cube等）を固定しない、テクスチャ基底クス
		@param
	*/
	class TextureBase:
		public Resource
	{

		typedef Resource  SuperClass;


	public:

		TextureBase();
		virtual ~TextureBase();


		/***************************************************************
			@brief	ファイルから初期化
			@par	[説明]
				ファイル(dds)からテクスチャを読み込み初期化する
				2D/Cube/3Dなどの種類は読み込むまでわからない
			@param
		*/
		bool	InitializeFromFile(const wchar_t* filePath);


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
			GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetSrvDescHandle() const { return m_SrvHandle; }	//!<	SRVのCPUハンドルを取得


	protected:

		D3D12_CPU_DESCRIPTOR_HANDLE	m_SrvHandle;	//!<	CPUハンドル


	private:
	};








}





#endif // !__INCLUDE_GFXTEXTUREBASE_H__
