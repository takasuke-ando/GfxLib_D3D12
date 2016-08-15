#ifndef			__INCLUDE_GFXCOMMANDALLOCATORPOOL_H__
#define			__INCLUDE_GFXCOMMANDALLOCATORPOOL_H__

/***************************************************************
	@file	GfxCommandAllocatorPool.h
	@par	[説明]
		
		コマンドアロケータを再利用するためのプール

		アロケータがGPU側で利用完了するのを認識するため
		フェンスとのペアで登録する

		取得時は、GPU利用が完了していたら、そのアロケータを返す
		終わっていなければ新しく生成する



		DirectX-Graphics-Samples\MiniEngine\Core\CommandAllocatorPool.cpp
		を参考

***************************************************************/


#include <queue>


namespace GfxLib
{

	class CommandAllocatorPool
	{
	public:
		CommandAllocatorPool(ID3D12Device* device , D3D12_COMMAND_LIST_TYPE  );
		~CommandAllocatorPool();



		ID3D12CommandAllocator*	Require(uint64_t CompletedFence);
		void					Release(uint64_t FenceValue,ID3D12CommandAllocator*);


	private:

		ID3D12Device* m_pd3dDev;
		D3D12_COMMAND_LIST_TYPE	m_ListType;
		std::queue<std::pair<uint64_t, ID3D12CommandAllocator*> > m_ReadyAllocators;

	};




}


#endif		//__INCLUDE_GFXCOMMANDALLOCATORPOOL_H__
