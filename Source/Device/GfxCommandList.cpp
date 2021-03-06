﻿



#include "stdafx.h"


#include "GfxCommandList.h"
#include "System/GfxCoreSystem.h"
#include "Resource/GfxDescriptorHeap.h"
#include "Device/GfxCommandQueue.h"

#include "External/DirectX-Graphics-Samples/d3dx12.h"



namespace {


	//	このサイズ以下で当てれば、転送にAdhocバッファを使用する
	const uint32_t	USE_ADHOC_BUFFER_SIZE_MAX = 1024 * 1024;


}



using namespace GfxLib;


CommandList::CommandList()
	:m_pd3dDev(nullptr)
	,m_pCmdQueue(nullptr)
	,m_pLastCbvSrvUavHeap(nullptr)
	,m_pLastSamplerHeap(nullptr)
	,m_pCurCmdAllocator(nullptr)
{
}

CommandList::~CommandList()
{
	Finalize();
}


bool	CommandList::Initialize( CommandQueue* cmdQueue, ID3D12GraphicsCommandList *cmdList)
{

	CoreSystem *coreSystem = CoreSystem::GetInstance();


	/*
	HRESULT hr = coreSystem->GetD3DDevice()->CreateCommandList(
		0,									//	Node Mask  マルチGPU識別用
		cmdListType,
		//coreSystem->GetCurrentCommandAllocator(),
		m_pCurCmdAllocator = cmdQueue->RequireCommandAllocator(),
		nullptr,
		IID_PPV_ARGS(m_pCmdList.InitialAccept())
	);


	if ( FAILED(hr)) {
		
		GFX_ERROR(L"CreateCommandList error %08x",hr);
		
		return false;

	}
	*/

	m_pCmdList = cmdList;


	m_pd3dDev = coreSystem->GetD3DDevice();
	HRESULT hr = m_pCmdList.QueryInterface((m_pCmdList4.InitialAccept()));
	m_pCmdQueue = cmdQueue;

	m_GpuBufferAllocator.Initialize(cmdQueue->GetAdhocGpuBufferHost());
	m_DescHeapAllocator.Initialize(cmdQueue->GetAdhocDescriptorHeapHost(DescriptorHeapType::CBV_SRV_UAV));
	m_DescHeapAllocator_Sampler.Initialize(cmdQueue->GetAdhocDescriptorHeapHost(DescriptorHeapType::SAMPLER));

	// おまじない
	//m_pCmdList->Close();


	return true;
}



void	CommandList::Finalize()
{
	
	//	OnExecuteが呼ばれなかった場合、こちらで回収を行う
	m_GpuBufferAllocator.Reset(0);	
	m_DescHeapAllocator.Reset(0);
	m_DescHeapAllocator_Sampler.Reset(0);

	if (m_pCurCmdAllocator) {
		// CommandQueueに返却しないといけない
		m_pCmdQueue->ReleaseCommandAllocator( m_pCmdQueue->InsertFence(), m_pCurCmdAllocator );

		m_pCurCmdAllocator = nullptr;
	}

	m_pCmdList.Release();
	m_pCmdList4.Release();
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

	m_pLastCbvSrvUavHeap = nullptr;
	m_pLastSamplerHeap = nullptr;

	//m_pCmdList->Reset(coreSystem->GetCurrentCommandAllocator(), nullptr /*PSO*/);
	if (m_pCurCmdAllocator == nullptr) {

		//	このインスタンスが作られて、初回だけはこちらに来ない

		m_pCmdList->Reset(m_pCurCmdAllocator = m_pCmdQueue->RequireCommandAllocator(), nullptr /*PSO*/);

		OnReset();
	}

	// 本来フレームの最初だけ↓を呼べばいい
	// フレーム内での再利用を識別するため、フレームボーダーかどうかのフラグを持たせたほうが良さそう
	if (frameBorder) {
		m_GpuBufferAllocator.Reset(0);	//	OnExecuteが呼ばれなかった場合、こちらで回収を行う
		m_DescHeapAllocator.Reset(0);
		m_DescHeapAllocator_Sampler.Reset(0);
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


void	CommandList::ResourceTransitionBarrier(Resource* resource, ResourceStates stateBefore, ResourceStates stateAfter)
{
	
	ResourceTransitionBarrier(resource->GetD3DResource(), stateBefore, stateAfter);

}


void	CommandList::ResourceUavBarrier(ID3D12Resource* resource)
{
	D3D12_RESOURCE_BARRIER barrier = {};

	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
	barrier.UAV.pResource = resource;

	m_pCmdList->ResourceBarrier(1, &barrier);
}


void	CommandList::ResourceUavBarrier(Resource* resource)
{

	ResourceUavBarrier(resource->GetD3DResource());

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
@brief	デスクリプタバッファの予約
@par	[説明]
	CBV_SRV_UAV,  および、Sampler用デスクリプタの予約を行う
	0を指定する事も可能（その場合変更しない）
@param[in]	cbv_srv_uav_size
@param[in]	sampler_size
*/

bool	CommandList::ReserveDescriptorBuffers(uint32_t cbv_srv_uav_size, uint32_t sampler_size)
{

	DescriptorHeap *cbvSrvUavHeap = m_pLastCbvSrvUavHeap;
	DescriptorHeap *samplerHeap = m_pLastSamplerHeap;

	if (cbv_srv_uav_size>0) {

		uint32_t startIndex = 0;
		cbvSrvUavHeap = m_DescHeapAllocator.Require(startIndex, m_pCmdQueue, cbv_srv_uav_size, false);

	}

	if (sampler_size > 0) {
		uint32_t startIndex = 0;
		samplerHeap = m_DescHeapAllocator_Sampler.Require(startIndex, m_pCmdQueue, sampler_size, false);

	}

	if (cbvSrvUavHeap != m_pLastCbvSrvUavHeap ||
		samplerHeap != m_pLastSamplerHeap) {

		ID3D12DescriptorHeap *heap[2] = {};

		uint32_t heapNum = 0;

		if (cbvSrvUavHeap != nullptr) {
			heap[heapNum] = cbvSrvUavHeap->GetD3DDescriptorHeap();
			++heapNum;
		}
		if (samplerHeap != nullptr) {
			heap[heapNum] = samplerHeap->GetD3DDescriptorHeap();
			++heapNum;
		}

		// ヒープを登録
		m_pCmdList->SetDescriptorHeaps(heapNum, heap);

		m_pLastCbvSrvUavHeap = cbvSrvUavHeap;
		m_pLastSamplerHeap = samplerHeap;
	}


	return true;
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

	// 呼び出し忘れor予約値が不足
	GFX_ASSERT(m_pLastCbvSrvUavHeap == heap, L"ReserveDescriptorBuffers size over");


	return DescriptorBuffer(heap, startIndex, size,this );

}


DescriptorBuffer CommandList::AllocateDescriptorBuffer_Sampler(uint32_t size)
{

	uint32_t startIndex = 0;
	DescriptorHeap *heap = m_DescHeapAllocator_Sampler.Require(startIndex, m_pCmdQueue, size);
	if (!heap) {
		return DescriptorBuffer();
	}


	// 呼び出し忘れor予約値が不足
	GFX_ASSERT(m_pLastSamplerHeap == heap, L"ReserveDescriptorBuffers size over");

	return DescriptorBuffer(heap, startIndex, size, this);

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
	テクスチャで使用される
@param
*/
bool	CommandList::InitializeResource(
	ID3D12Resource* dstResource,
	uint32_t subDataNum,
	const D3D12_SUBRESOURCE_DATA subData[],
	ResourceStates states )
{

	UINT64 uploadBufferSize = d3dx12::GetRequiredIntermediateSize(dstResource, 0, subDataNum);
	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();
	

	//  小さい初期化の場合、アドホックなバッファを使うこともできるように

	if (uploadBufferSize <= USE_ADHOC_BUFFER_SIZE_MAX) {

		GpuBufferRange buffer = AllocateGpuBuffer((uint32_t)uploadBufferSize, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

		// CPU Copy
		d3dx12::UpdateSubresources(GetD3DCommandList(), dstResource, buffer.GetD3DResource(), buffer.GetStartOffset(),
			0, subDataNum, subData);

	}
	else {

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


		// 遅延開放に登録
		CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);
	}

	// バリヤを設定
	ResourceTransitionBarrier(dstResource, ResourceStates::CopyDest, states );


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
	const size_t byteSize,
	ResourceStates states)
{


	ID3D12Device *d3dDev = GfxLib::CoreSystem::GetInstance()->GetD3DDevice();

	

	if (byteSize <= USE_ADHOC_BUFFER_SIZE_MAX ) {

		GpuBufferRange buffer = AllocateGpuBuffer( (uint32_t)byteSize, 16);

		// CPU Copy
		memcpy(buffer.GetCpuAddr(), srcData, byteSize);
		GetD3DCommandList()->CopyBufferRegion(dstResource, 0, buffer.GetD3DResource(), buffer.GetStartOffset(), byteSize);
	}
	else {
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

		// 遅延開放に登録
		CoreSystem::GetInstance()->GetDelayDelete().Regist((ID3D12Resource*)d3dResForUpload);
	}
	ResourceTransitionBarrier(dstResource, ResourceStates::CopyDest, states);





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
