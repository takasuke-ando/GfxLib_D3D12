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
		@param
	*/
	template< DescriptorHeapType heapType >
	class AutoDescriptorHandle :
		AutoDescriptorHandleBase
	{
	public:
		AutoDescriptorHandle();
		~AutoDescriptorHandle();


		operator D3D12_CPU_DESCRIPTOR_HANDLE()	{return m_Handle; }


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





}


#endif // !__INCLUDE_GFXAUTODESCRIPTORHANDLE_H__
