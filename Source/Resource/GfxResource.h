﻿#ifndef __INCLUDE_GFXRESOURCE_H__
#define	__INCLUDE_GFXRESOURCE_H__

/***************************************************************
	@brief	リソース基底クラス
	@par	[説明]
		ID3D12Resourceをカプセル化
		テクスチャ、定数バッファ、レンダーターゲット
		頂点バッファなど
		すべてのリソースの基底となる
	
***************************************************************/


#include "System/GfxDefines.h"


namespace GfxLib
{

	class Resource
	{

	public:
		Resource();
		virtual ~Resource();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		// D3Dリソース取得
		ID3D12Resource*	GetD3DResource() const { return m_d3dRes; }

		/***************************************************************
			@brief	フォーマットの取得
			@par	[説明]
				D3DResourceから取得する
				エイリアスしている場合など、
				派生側でオーバーライドすることも可能
		*/
		virtual DXGI_FORMAT		GetFormat() const;


	protected:

		bool		_Initialize_Buffer( size_t sizeInBytes , D3D12_HEAP_TYPE heapType , D3D12_RESOURCE_STATES initState , D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);

		bool		_Initialize_RenderTarget(Format format, uint32_t width, uint32_t height, uint32_t mipLevls, bool EnableShaderResource);

		/*
			DepthStencilとして初期化
			初期ステートはD3D12_RESOURCE_STATE_DEPTH_WRITE
		*/
		bool		_Initialize_DepthStencil( Format format , uint32_t width ,uint32_t height , uint32_t mipLevls, bool EnableShaderResource , const D3D12_CLEAR_VALUE *clearValue );


		/*
			Texture2Dとして初期化
		*/
		bool		_Initialize_Texture2D_Test(Format format, uint32_t width, uint32_t height, uint32_t mipLevls);
		bool		_Initialize_Texture2D(Format format, uint32_t width, uint32_t height, uint32_t mipLevls , D3D12_RESOURCE_STATES initState, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE);



		/*
			すでに作成済みのリソースを使い、初期化
		*/
		bool			_Initialize(ID3D12Resource *rtResource);


		D3DPtr<ID3D12Resource>	m_d3dRes;
		
	private:


	};





}













#endif // !__INCLUDE_GFXRESOURCE_H__
