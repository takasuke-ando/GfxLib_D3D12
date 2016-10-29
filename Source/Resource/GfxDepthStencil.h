#ifndef __INCLUDE_GFXDEPTHSTENCIL_H__
#define __INCLUDE_GFXDEPTHSTENCIL_H__



#include "GfxResource.h"
#include "GfxDescriptorHeap.h"


namespace GfxLib
{


	/***************************************************************
		@brief	デプスステンシル
		@par	[説明]
			深度ステンシルをカプセル化
			GfxRenderTargetとほぼ同じ
		@param
	*/
	class DepthStencil : public Resource
	{

		typedef Resource SuperClass;

	public:
		DepthStencil();
		~DepthStencil();


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
		GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize(bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE);


		/***************************************************************
		@brief	初期化
		@par	[説明]
			デプスステンシルを初期化する
		@param[in]	format:	フォーマット
		@param[in]	width:	幅
		@param[in]	height:	高さ
		@param[in]	mipLevels : ミップレベル。0: 完全ミップ
		@param[in]	rtvEnable:	ShaderResource有効
		*/
		bool	Initialize(Format format, uint32_t width, uint32_t height, uint32_t mipLevels = 1, bool EnableShaderResource = true);


		/***************************************************************
		@brief	Descriptorのコピーを行う
		@par	[説明]
		@param
		*/
		void	CopySRVDescriptor(D3D12_CPU_DESCRIPTOR_HANDLE dstHandle);


		/***************************************************************
		@brief	フォーマットの取得
		@par	[説明]
			Resourceクラスから派生
			リソースのフォーマットではなく、深度ステンシルのフォーマット(DXGI_D***)を返す
		*/
		virtual DXGI_FORMAT		GetFormat() const	override { return m_DSVFormat; }


		D3D12_CPU_DESCRIPTOR_HANDLE	GetDSVDescriptorHandle() const { return m_DSVDescHeap.GetCPUDescriptorHandleByIndex(0); }

	private:


		DescriptorHeap	m_DSVDescHeap;
		DescriptorHeap  m_SRVDescHeap;

		DXGI_FORMAT		m_DSVFormat;

	};



}



#endif // !__INCLUDE_GFXDEPTHSTENCIL_H__
