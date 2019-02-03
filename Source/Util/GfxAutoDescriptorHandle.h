#ifndef __INCLUDE_GFXAUTODESCRIPTORHANDLE_H__
#define __INCLUDE_GFXAUTODESCRIPTORHANDLE_H__


#include "System/GfxDefines.h"

namespace GfxLib
{



	class AutoDescriptorHandleBase
	{
	protected:

		// 依存関係を減らすためだけの基底クラス
		D3D12_CPU_DESCRIPTOR_HANDLE		Allocate(DescriptorHeapType type);
		void							Free(DescriptorHeapType type,D3D12_CPU_DESCRIPTOR_HANDLE handle);

	};



	/***************************************************************
		@brief	デスクリプタハンドル自動確保クラス
		@par	[説明]
			デスクリプタハンドルを自動的に確保、開放する
			AllocateDescriptorHandle , FeeDescriptorHandle が使用される
			CPU用として確保される(D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLEはつかない)
		@param
	*/
	template< DescriptorHeapType heapType >
	class AutoDescriptorHandle :
		AutoDescriptorHandleBase
	{
	public:
		AutoDescriptorHandle();
		~AutoDescriptorHandle();


		operator D3D12_CPU_DESCRIPTOR_HANDLE() const	{return m_Handle; }

		// GPU HANDLEは付きません

	private:

		D3D12_CPU_DESCRIPTOR_HANDLE	m_Handle;
	};




	template<DescriptorHeapType heapType>
	AutoDescriptorHandle<heapType>::AutoDescriptorHandle()
	{


		m_Handle = Allocate(heapType);


	}

	template<DescriptorHeapType heapType>
	AutoDescriptorHandle<heapType>::~AutoDescriptorHandle()
	{

		Free(heapType, m_Handle);
	}



	/*
	
	
	*/
	typedef AutoDescriptorHandle<DescriptorHeapType::CBV_SRV_UAV>	AutoCBV_SRV_UAVHandle;
	typedef AutoDescriptorHandle<DescriptorHeapType::SAMPLER>		AutoSamplerHandle;
	typedef AutoDescriptorHandle<DescriptorHeapType::RTV>			AutoRTVHandle;
	typedef AutoDescriptorHandle<DescriptorHeapType::DSV>			AutoDSVHandle;




}


#endif // !__INCLUDE_GFXAUTODESCRIPTORHANDLE_H__
