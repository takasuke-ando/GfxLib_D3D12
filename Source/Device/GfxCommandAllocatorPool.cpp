

/***************************************************************
@file	GfxCommandAllocatorPool.cpp
@par	[説明]

	コマンドアロケータを再利用するためのプール

	アロケータがGPU側で利用完了するのを認識するため
	フェンスとのペアで登録する

	取得時は、GPU利用が完了していたら、そのアロケータを返す
	終わっていなければ新しく生成する



	DirectX-Graphics-Samples\MiniEngine\Core\CommandAllocatorPool.cpp
	を参考

***************************************************************/



#include "stdafx.h"


#include "GFXCommandAllocatorPool.h"




using namespace GfxLib;



CommandAllocatorPool::CommandAllocatorPool(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type )
	:m_pd3dDev( device )
	,m_ListType( type )
{
}

CommandAllocatorPool::~CommandAllocatorPool()
{

	// クリアする
	// この呼び出し前に、すべてのコマンドはフラッシュ済みであることを保証する事

	while( !m_ReadyAllocators.empty() ) {

		auto allocatorPair = m_ReadyAllocators.front();
		allocatorPair.second->Release();
		m_ReadyAllocators.pop();
	}

}



/***************************************************************
	@brief	アロケータを要求する
	@par	[説明]
		
	@para[in]	CompletedFence:	GPU側で利用完了が保証されているフェンスの値

*/
ID3D12CommandAllocator*	CommandAllocatorPool::Require(uint64_t CompletedFence)
{

	ID3D12CommandAllocator* allocator = nullptr;


	if (m_ReadyAllocators.size() > 0) {


		std::pair<uint64_t, ID3D12CommandAllocator*>& AllocatorPair = m_ReadyAllocators.front();

		if (AllocatorPair.first <= CompletedFence)
		{
			allocator = AllocatorPair.second;
			HRESULT hr = allocator->Reset();
			m_ReadyAllocators.pop();

			if (FAILED(hr)) {
				GFX_ERROR(L"CommandAllocator::Reset Failed error=%08x", hr);
				allocator = nullptr;	//	返すわけには行かない…
			}
		}
	}

	if (allocator) {
		return allocator;
	}


	HRESULT hr;
	hr = m_pd3dDev->CreateCommandAllocator(m_ListType, IID_PPV_ARGS(&allocator));
	if (FAILED(hr)) {
		GFX_ERROR(L"CreateCommandAllocator Failed error=%08x", hr);
		return nullptr;
	}


	return allocator;

}



void		CommandAllocatorPool::Release(uint64_t FenceValue,ID3D12CommandAllocator* allocator )
{


	m_ReadyAllocators.push(std::make_pair(FenceValue, allocator));


}




