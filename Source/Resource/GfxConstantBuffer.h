﻿#ifndef __INCLUDE_GFXCONSTANTBUFFER_H__
#define	__INCLUDE_GFXCONSTANTBUFFER_H__


#include "GfxDescriptorHeap.h"


namespace GfxLib
{

	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		~ConstantBuffer();


		bool	Initialize(uint32_t byteSize);


		/***************************************************************
			@brief	既存のリソースのサブセットを指定して、初期化
			@par	[説明]
				既存のリソースを部分的にマッピングし、ConstantBufferとして利用可能にする
			@param
		*/
		bool	Initialize(ID3D12Resource* res, uint32_t buffWidth , D3D12_GPU_VIRTUAL_ADDRESS gpuAddr, void* cpuAddr);


		void	SetData(const void*pData, uint32_t byteSize);


		/***************************************************************
		@brief	開放
		@par	[説明]
		@param[in]	delayed:	遅延開放フラグ。数フレーム待ってから開放することで、
								GPUアクセス中のリソースに対して書き込みを行うことがない
		*/
		void	Finalize( bool delayed = GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE );

		//const DescriptorHeap&	GetDescriptorHeap() const { return m_descHeap; }		//	とりあえず作ったCBV用。ここで持ちたくはない…


		// CPU読み取り用のデスクリプタハンドル
		D3D12_CPU_DESCRIPTOR_HANDLE	GetCbvDescHandle() const {
			return m_CbvHandle;
		}


	private:

		D3DPtr<ID3D12Resource>		m_d3dRes;

		uint32_t					m_byteSize;
		void*						m_pMappedAddr;		//	GPU書き込み先アドレス
		D3D12_CPU_DESCRIPTOR_HANDLE	m_CbvHandle;

	};


}






#endif // !__INCLUDE_GFXCONSTANTBUFFER_H__
