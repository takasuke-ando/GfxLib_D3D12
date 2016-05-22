/***************************************************************
	@brief	GfxAutoDescriptorHandle
	@par	[説明]
		自動的にハンドルのストレージ確保、開放を行うユーティリティクラス
		CoreSystemに依存するため、初期化前に構築が行われてはいけない
	@param
/****************************************************************/






#include "stdafx.h"


#include "Util/GfxAutoDescriptorHandle.h"

#include "System/GfxCoreSystem.h"



using namespace GfxLib;







D3D12_CPU_DESCRIPTOR_HANDLE		AutoDescriptorHandleBase::Allocate(DescriptorHeapType type)
{

	return AllocateDescriptorHandle(type);

}


void							AutoDescriptorHandleBase::Free(DescriptorHeapType type, D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	FreeDescriptorHandle(type, handle);

}









