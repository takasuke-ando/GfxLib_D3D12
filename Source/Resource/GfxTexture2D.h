#ifndef __INCLUDE_GFXTEXTURE2D_H__
#define	__INCLUDE_GFXTEXTURE2D_H__



#include "GfxResource.h"
#include "Util/GfxAutoDescriptorHandle.h"


namespace GfxLib
{


	/***************************************************************
		@brief	2Dテクスチャクラス
		@par	[説明]
			2Dテクスチャクラス
	*/
	class Texture2D
		: public Resource	//	TextureBase的なのを作るかも
	{
		typedef Resource	SuperClass;
	public:
		Texture2D();
		virtual ~Texture2D();


		/***************************************************************
		@brief	初期化
		@par	[説明]
			Immutableな2Dテクスチャとして初期化する
		@param[in]	format	:	バッファフォーマット
		@param[in]	width	:	幅
		@param[in]	height	:	高さ
		@param[in]	mipLevels:	ミップレベル(0にすると、自動的に完全なミップを作成する)
		@param[in]  subData:	初期化に使用するサブリソースデータ
		*/
		bool	Initialize(Format format, uint32_t width, uint32_t height, uint32_t mipLevels);
		bool	Initialize( Format format, uint32_t width, uint32_t height, uint32_t mipLevels , uint32_t subDataNum, const D3D12_SUBRESOURCE_DATA subData [] );

		bool	InitializeFromFile(const wchar_t *filePath);


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
			GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
		@brief	テクスチャへの書き込み
		@par	[説明]
			Initialize後に呼び出し、初期化を行う
			現状はD3D12_HEAP_TYPE_UPLOADに対して
			あまり効率は良くないはず

			UPLOADからDEFAULTへのコピーを行う

			Initialize関数に初期化構造体を渡すようにするかも
		@param
		*/
		bool	WriteToSubresource(uint32_t DstSubResource, const D3D12_BOX *pDstBox, const void* pSrcData, uint32_t SrcRowPitch, uint32_t SrcDepthPitch);


		D3D12_CPU_DESCRIPTOR_HANDLE	GetSrvDescHandle() const { return m_SrvHandle; }	//!<	CPUハンドル

	private:


		//AutoCBV_SRV_UAVHandle	m_SrvHandle;	//!<	SRVハンドル

		D3D12_CPU_DESCRIPTOR_HANDLE	m_SrvHandle;	//!<	CPUハンドル

	};








}



#endif // !__INCLUDE_GFXTEXTURE2D_H__
