#ifndef __INCLUDE_GFXRENDERTARGET_H__
#define __INCLUDE_GFXRENDERTARGET_H__




#include "GfxResource.h"
#include "GfxDescriptorHeap.h"
#include "System/GfxDefines.h"

namespace GfxLib {

	template< DescriptorHeapType heapType >
	class AutoDescriptorHandle;

	/***************************************************************
		@brief	レンダーターゲット
		@par	[説明]
			レンダーターゲットクラス
			テクスチャとしてもアクセスは可能として初期化されるが
			EnableShaderResource=false とするとSRV不可になる
	*/
	class RenderTarget : 
		public Resource
	{

		typedef Resource SuperClass;

	public:
		RenderTarget();
		~RenderTarget();


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
			レンダーターゲットを初期化する
		@param[in]	format:	フォーマット
		@param[in]	width:	幅
		@param[in]	height:	高さ
		@param[in]	mipLevels : ミップレベル。0: 完全ミップ
		@param[in]	rtvEnable:	ShaderResource有効
		*/
		bool	Initialize( Format format, uint32_t width ,uint32_t height , uint32_t mipLevels = 1, bool EnableShaderResource = true );


		/***************************************************************
		@brief	初期化
		@par	[説明]
			すでに作成済みのリソースを使って、初期化を行う
		@param	rtResource	:	レンダーターゲットリソース
		*/
		bool	Initialize(ID3D12Resource *rtResource);


		/***************************************************************
		@brief	Descriptorのコピーを行う
		@par	[説明]
		@param
		*/
		void	CopySRVDescriptor( D3D12_CPU_DESCRIPTOR_HANDLE dstHandle );


		D3D12_CPU_DESCRIPTOR_HANDLE	GetRTVDescriptorHandle() const;

	private:

		bool		_CreateRTV( DXGI_FORMAT format , uint32_t mipLevels );


		//DescriptorHeap	m_RTVDescHeap;
		//D3D12_CPU_DESCRIPTOR_HANDLE *m_paRTVDescHandle;
		AutoDescriptorHandle< DescriptorHeapType::RTV >	*m_paRTVDescHandle;
		DescriptorHeap  m_SRVDescHeap;

	};


	
}




#endif // !__INCLUDE_GFXRENDERTARGET_H__
