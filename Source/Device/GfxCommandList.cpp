



#include "stdafx.h"


#include "GfxCommandList.h"
#include "System/GfxCoreSystem.h"
#include "Resource/GfxDescriptorHeap.h"
#include "Device/GfxCommandQueue.h"

#include "External/DirectX-Graphics-Samples/d3dx12.h"

using namespace GfxLib;


CommandList::CommandList()
	:m_pd3dDev(nullptr)
	,m_pCmdQueue(nullptr)
	,m_pCurCmdAllocator(nullptr)
{
}

CommandList::~CommandList()
{
	Finalize();
}


bool	CommandList::Initialize( CommandQueue* cmdQueue )
{

	CoreSystem *coreSystem = CoreSystem::GetInstance();

	HRESULT hr = coreSystem->GetD3DDevice()->CreateCommandList(
		0,									//	Node Mask  マルチGPU識別用
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		//coreSystem->GetCurrentCommandAllocator(),
		m_pCurCmdAllocator = cmdQueue->RequireCommandAllocator(),
		nullptr,
		IID_PPV_ARGS(m_pCmdList.InitialAccept())
	);


	if ( FAILED(hr)) {
		
		GFX_ERROR(L"CreateCommandList error %08x",hr);
		
		return false;

	}


	m_pd3dDev = coreSystem->GetD3DDevice();
	m_pCmdQueue = cmdQueue;

	m_GpuBufferAllocator.Initialize(cmdQueue->GetAdhocGpuBufferHost());
	m_DescHeapAllocator.Initialize(cmdQueue->GetAdhocDescriptorHeapHost());

	// おまじない
	//m_pCmdList->Close();


	return true;
}



void	CommandList::Finalize()
{
	
	//	OnExecuteが呼ばれなかった場合、こちらで回収を行う
	m_GpuBufferAllocator.Reset(0);	
	m_DescHeapAllocator.Reset(0);

	if (m_pCurCmdAllocator) {
		// CommandQueueに返却しないといけない
		m_pCmdQueue->ReleaseCommandAllocator( m_pCmdQueue->InsertFence(), m_pCurCmdAllocator );

		m_pCurCmdAllocator = nullptr;
	}

	m_pCmdList.Release();
	m_pCmdQueue = nullptr;

}




/***************************************************************
	@brief	内部状態をリセットし、再利用可能にする
	@par	[説明]
	@param
*/
void	CommandList::Reset(bool frameBorder)
{

	CoreSystem * coreSystem = CoreSystem::GetInstance();

	//m_pCmdList->Reset(coreSystem->GetCurrentCommandAllocator(), nullptr /*PSO*/);
	if (m_pCurCmdAllocator == nullptr) {
		m_pCmdList->Reset(m_pCurCmdAllocator = m_pCmdQueue->RequireCommandAllocator(), nullptr /*PSO*/);
	}

	// 本来フレームの最初だけ↓を呼べばいい
	// フレーム内での再利用を識別するため、フレームボーダーかどうかのフラグを持たせたほうが良さそう
	if (frameBorder) {
		m_GpuBufferAllocator.Reset(0);	//	OnExecuteが呼ばれなかった場合、こちらで回収を行う
		m_DescHeapAllocator.Reset(0);
	}

}



//	リソースバリアを設定する
void	CommandList::ResourceTransitionBarrier(ID3D12Resource* res , ResourceStates stateBefore, ResourceStates stateAfter )
{

	D3D12_RESOURCE_BARRIER barrier = {};

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = res;
	barrier.Transition.StateBefore = (D3D12_RESOURCE_STATES)stateBefore;
	barrier.Transition.StateAfter  = (D3D12_RESOURCE_STATES)stateAfter;

	m_pCmdList->ResourceBarrier(1, &barrier);

}



/***************************************************************
@brief	バッファの確保を行います
@par	[説明]
このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
@param[in]	size:		要求サイズ
@param[in]	alignment:	アライメント

*/
GpuBufferRange	CommandList::AllocateGpuBuffer( uint32_t size, uint32_t alignment)
{

	
	return m_GpuBufferAllocator.Require(m_pCmdQueue, size, alignment);

}




/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める

*/
#if 0
DescriptorHeap*	CommandList::RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex)
{

	return m_DescHeapAllocator.Require(size, startIndex);

}
#endif


/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
同時に、ハンドルコピーも行う
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める
@param[in]	srcHandle:	コピー元となるハンドルの配列
*/
#if 0
DescriptorHeap*	CommandList::RequireAdhocDescriptorHeap(uint32_t size, uint32_t &startIndex, const D3D12_CPU_DESCRIPTOR_HANDLE *srcHandle)
{

	DescriptorHeap *heap = m_DescHeapAllocator.Require(size, startIndex);
	if (!heap) {
		return heap;
	}

	auto heapType = heap->GetHeapType();

	for (uint32_t i = 0; i < size; ++i) {

		m_pd3dDev->CopyDescriptorsSimple(1,
			heap->GetCPUDescriptorHandleByIndex(startIndex + i),
			srcHandle[i],
			heapType);
	}


	return heap;
}
#endif



/***************************************************************
@brief	デスクリプタバッファの確保
@par	[説明]
デスクリプタバッファの確保
一時的なデスクリプタバッファの確保を行う
利用者側は、
DescriptorBufferクラスを経由して、デスクリプタの書き込み、コピーを行う
@param[in]	size
*/
DescriptorBuffer CommandList::AllocateDescriptorBuffer(uint32_t size)
{

	uint32_t startIndex = 0;
	DescriptorHeap *heap = m_DescHeapAllocator.Require(startIndex, m_pCmdQueue, size);
	if (!heap) {
		return DescriptorBuffer();
	}

	return DescriptorBuffer(heap, startIndex, size,this );

}


#if 0
/***************************************************************
@brief	デスクリプタバッファに定数を設定する
@par	[説明]
デスクリプタバッファに定数バッファを設定する
@param
*/
void	CommandList::SetConstantBuffer(DescriptorBuffer& descBuff, uint32_t index, const void* pData, uint32_t size)
{

	void* cpuAddr = nullptr;
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddr = AllocateGpuBuffer(cpuAddr,
		GfxLib::UpperBounds(size, 256), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	
	memcpy(cpuAddr, pData , size);

	descBuff.SetConstantBuffer( index , gpuAddr , size )

}
#endif

/***************************************************************
@brief		アロケータをデタッチする
*/
ID3D12CommandAllocator*	CommandList::DetachAllocator()
{
	ID3D12CommandAllocator *allocator = m_pCurCmdAllocator;
	m_pCurCmdAllocator = nullptr;

	return allocator;

}


/***************************************************************
@brief	リソースの初期化コピーを行う
@par	[説明]
@param
*/
bool	CommandList::InitializeResource(
	ID3D12Resource* dstResource,
	uint32_t subDataNum,
	const D3D12_SUBRESOURCE_DATA subData[])
{

	UINT64 uploadBufferSize = d3dx12::GetRequiredIntermediateSize(dstResource, 0, subDataNum);
	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
	

	// @TODO 小さい初期化の場合、アドホックなバッファを使うこともできるように
	D3DPtr<ID3D12Resource>	d3dResForUpload;
	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//	TextureへのUpload中間バッファは、Bufferとして作る必要がある
	resDesc.Alignment = 0;
	resDesc.Width = uploadBufferSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//	RowMajor→UNKNOWN へのコピー可能？
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



	HRESULT hr = d3dDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//	変更不可
		nullptr,
		IID_PPV_ARGS(d3dResForUpload.InitialAccept())
		);


	if (FAILED(hr)) {
		GFX_ERROR(L"Create Committed Resource Failed (%08x)", hr);
		return false;
	}



	d3dx12::UpdateSubresources(GetD3DCommandList(), dstResource, d3dResForUpload, 0, 0, subDataNum, subData);
	ResourceTransitionBarrier(dstResource, ResourceStates::CopyDest, ResourceStates::GenericRead);


	// 遅延開放に登録
	CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);

	return true;

}


/***************************************************************
@brief	リソースの初期化コピーを行う
@par	[説明]
バッファーリソースの初期化コピー
@param
*/
bool	CommandList::InitializeResource(
	ID3D12Resource* dstResource,
	const void *srcData,
	const size_t byteSize)
{


	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();

	
	// @TODO	マルチスレッド処理の場合はどうするのか


	D3DPtr<ID3D12Resource>	d3dResForUpload;

	D3D12_HEAP_PROPERTIES heapProp = {};


	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProp.CreationNodeMask = 0;
	heapProp.VisibleNodeMask = 0;


	D3D12_RESOURCE_DESC resDesc = {};

	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;	//	TextureへのUpload中間バッファは、Bufferとして作る必要がある
	resDesc.Alignment = 0;
	resDesc.Width = byteSize;
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.Format = DXGI_FORMAT_UNKNOWN;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;	//	RowMajor→UNKNOWN へのコピー可能？
	resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;



	HRESULT hr = d3dDev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&resDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,		//	変更不可
		nullptr,
		IID_PPV_ARGS(d3dResForUpload.InitialAccept())
		);


	if (FAILED(hr)) {
		GFX_ERROR(L"Create Committed Resource Failed (%08x)", hr);
		return false;
	}

	void *pInterBuff;
	hr = d3dResForUpload->Map(0, nullptr, &pInterBuff);
	if (FAILED(hr)) {
		GFX_ERROR(L"Intermediate Resouce Map Failed (%08x)", hr);
		return false;
	}

	// CPU Copy
	memcpy(pInterBuff, srcData, byteSize);

	d3dResForUpload->Unmap(0, nullptr);

	// Defaultバッファにコピーを行う
	GetD3DCommandList()->CopyBufferRegion(dstResource, 0, d3dResForUpload, 0, byteSize);
	ResourceTransitionBarrier(dstResource, ResourceStates::CopyDest, ResourceStates::GenericRead);


	// 遅延開放に登録
	CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);



	return true;

}



/***************************************************************
@brief	ExecuteCommandListが呼び出される
@par	[説明]
ExecuteCommandListが呼び出される直前に呼ばれる
@param
*/
void	CommandList::PreExecute()
{

	GetD3DCommandList()->Close();

}


/***************************************************************
@brief	ExecuteCommandListが呼び出された
@par	[説明]
ExecuteCommandListが呼び出された後に呼ばれる
@param[in]	fence:	コマンドがGPU側で完了したことを識別するフェンス
*/
void	CommandList::PostExecute(uint64_t fence)
{

	m_GpuBufferAllocator.Reset(fence);
	m_DescHeapAllocator.Reset(fence);

}
