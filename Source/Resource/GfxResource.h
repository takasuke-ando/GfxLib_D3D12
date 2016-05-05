#ifndef __INCLUDE_GFXRESOURCE_H__
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
		~Resource();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


	protected:

		bool		_Initialize_Buffer( size_t sizeInBytes );

		bool		_Initialize_RenderTarget(Format format, uint32_t width, uint32_t height, uint32_t mipLevls, bool EnableShaderResource);

		/*
			DepthStencilとして初期化
			初期ステートはD3D12_RESOURCE_STATE_DEPTH_WRITE
		*/
		bool		_Initialize_DepthStencil( Format format , uint32_t width ,uint32_t height , uint32_t mipLevls, bool EnableShaderResource);


		D3DPtr<ID3D12Resource>	m_d3dRes;
		
	private:


	};





}













#endif // !__INCLUDE_GFXRESOURCE_H__
