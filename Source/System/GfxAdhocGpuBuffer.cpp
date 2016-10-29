/***************************************************************
	@file	GfxAdhocGpuBuffer
	@par	[説明]
		
		フレーム中に一時的に確保される定数・頂点・インデックスバッファを作成する
		
		アルゴリズムは2パターン考えられる

		１．大きなバッファリソースを作成し、そこからオフセットで確保していく
		　　この場合CreateConstantBufferView呼び出し回数が多くなる

		２．サイズごとに定数バッファをあらかじめ作成しておき、使いまわす
		  　メモリ効率は良くないが、CreateConstantBufferViewの呼び出し回数は最小限
		    親リソースから部分的に確保すればよいので無駄にCBVが作られることに目をつぶれば悪くないか？


		結局…
		GPU_DESCRIPTOR_HANDLEにCBVを直接書き込むようにする

		DescriptorHeap自体がAdhocであれば
		CBVの使いまわし、寿命を考える必要がなくなる

		さらに汎用的に考えて
		AdhocConstantBuffer => AdhocGpuBuffer に変更


		マルチスレッド処理を考慮すると、細かい単位で同期オブジェクトで制御しないといけないので
		スレッドローカルなオブジェクト(AdhocGpuBufferClient)を作成し、それ経由でバッファの要求を行う



/***************************************************************/





#include "stdafx.h"



#include "System/GfxAdhocGpuBuffer.h"
#include "Device/GfxCommandQueue.h"
#include "Util/GfxMath.h"



using namespace GfxLib;



namespace {


	const uint32_t MaxBufferSize = 4 * 1024 * 1024;
	const uint32_t MaxBufferAlignemnt = 64 * 1024;


}


AdhocGpuBuffer::AdhocGpuBuffer()
	: m_allocatedCount(0)

{
}

AdhocGpuBuffer::~AdhocGpuBuffer()
{



	while (!m_FreeBuffer.empty()) {
		auto &it = m_FreeBuffer.front();
		delete it.second;
		m_FreeBuffer.pop_front();
	}


}






/***************************************************************
@brief	AdhocGpuBufferより呼び出される。Bufferオブジェクトの確保
@par	[説明]
@param
*/
Buffer*	AdhocGpuBuffer::RequireBuffer(uint64_t finished_fence)
{
	std::lock_guard<std::mutex> LockGuard(m_Mutex);

	Buffer *buffer = nullptr;



	if (m_FreeBuffer.size() > 0){

		// 未使用リストから取り出す
		auto &fence_and_buffer = m_FreeBuffer.front();

		if (finished_fence >= fence_and_buffer.first) {
			buffer = fence_and_buffer.second;
			m_FreeBuffer.pop_front();
		}

	}
	if (!buffer) {
		// 新たにDescHeapの作成
		buffer = new Buffer;


		bool b = buffer->Initialize(MaxBufferSize);

		if (!b) {
			delete buffer;
			return nullptr;
		}
		++m_allocatedCount;
	}



	return buffer;

}


/***************************************************************
@brief	バッファの回収を行う
@par	[説明]
FenceValueを0にすると、使わなかったバッファということで待機なしでの回収になる
@param
*/
void	AdhocGpuBuffer::ReleaseBuffer(uint64_t FenceValue, Buffer* buffer)
{

	if (FenceValue) {
		m_FreeBuffer.push_back(std::make_pair(FenceValue, buffer));
	}
	else {
		//	待機せずに使いまわしたい
		m_FreeBuffer.push_front(std::make_pair(FenceValue, buffer));
	}


}






AdhocGpuBufferClient::AdhocGpuBufferClient()
	:m_pHost(nullptr)
	,m_pCurrentBuffer(nullptr)
	,m_nCurrentBufferUsedSize(0)
{

}



AdhocGpuBufferClient::~AdhocGpuBufferClient()
{

	for (auto buffer : m_vecUsingBuffer) {
		m_pHost->ReleaseBuffer(0, buffer);
	}
	m_vecUsingBuffer.clear();

}



void	AdhocGpuBufferClient::Initialize(AdhocGpuBuffer *host)
{

	m_pHost = host;
}



/***************************************************************
@brief	フレームの最初に呼び出す
@par	[説明]
	再利用を可能にするため、
	所持していたバッファの返却などを行います
@param[in]	fence:	すべてのバッファが使用完了することが保証される、フェンス値
*/
void	AdhocGpuBufferClient::Reset(uint64_t fence )
{

	for (auto buffer : m_vecUsingBuffer) {
		m_pHost->ReleaseBuffer(fence, buffer);
	}
	m_vecUsingBuffer.clear();

	m_pCurrentBuffer = nullptr;
	m_nCurrentBufferUsedSize = 0;
}



/***************************************************************
@brief	バッファの確保を行います
@par	[説明]
このフレームの間だけ、利用可能なGPUアサインされたバッファを確保します
数フレーム後にはこの領域は再利用されるため、継続して保持することはできません
@param[out]  cpuAddress:	成功時に、CPUマップ済みアドレスが返される
@param[in]	size:		要求サイズ
@param[in]	alignment:	アライメント

*/
GpuBufferRange	AdhocGpuBufferClient::Require(CommandQueue *fenceOwnQueue, uint32_t requestSize, uint32_t alignment)
{

	//	最大アライメント制限をオーバーしている
	if (alignment > MaxBufferAlignemnt) {
		GFX_ERROR(L"AdhocGpuBufferClient::Require alignment over (%d)", alignment);
		return GpuBufferRange();
	}
	

	void *cpuAddress = nullptr;


	uint32_t srcDataSize = requestSize;
	requestSize = UpperBounds(requestSize, 16);

	if (m_pCurrentBuffer) {

		// 現在のヒープをそのまま使える
		const uint32_t capacity = m_pCurrentBuffer->GetBufferSize();

		D3D12_GPU_VIRTUAL_ADDRESS	nowTop = m_pCurrentBuffer->GetGpuVirtualAddress() + m_nCurrentBufferUsedSize;

		// アライメントオフセットを算出
		uint64_t addressOffset = UpperBounds(nowTop, (uint64_t)alignment) - nowTop;


		if (requestSize + m_nCurrentBufferUsedSize + addressOffset <= capacity) {

			D3D12_GPU_VIRTUAL_ADDRESS result = nowTop + addressOffset;

			//memcpy(((uint8_t*)m_pCurrentBuffer->GetCpuAddress()) + m_nCurrentBufferUsedSize + addressOffset, srcData, srcDataSize);
			uint64_t offset = m_nCurrentBufferUsedSize + addressOffset;
			cpuAddress = ((uint8_t*)m_pCurrentBuffer->GetCpuAddress()) + offset;

			m_nCurrentBufferUsedSize += uint32_t(requestSize + addressOffset);

			return GpuBufferRange(m_pCurrentBuffer->GetD3DResource(), offset, cpuAddress, result, requestSize );
		}
		else {


			//	今使っているバッファは使えなかった
			//  Hostに新しいバッファを要求する

			m_nCurrentBufferUsedSize = 0;
			m_pCurrentBuffer = nullptr;

		}

	}


	if (requestSize > MaxBufferSize) {
		GFX_ERROR(L"AdhocDescriptorHeap::Require size over (%d)", requestSize);
		return GpuBufferRange();
	}

	const uint64_t fence = fenceOwnQueue->GetCompletedFence();
	m_pCurrentBuffer = m_pHost->RequireBuffer(fence);

	m_vecUsingBuffer.push_back(m_pCurrentBuffer);

	// 新規バッファのアドレスを調整
	D3D12_GPU_VIRTUAL_ADDRESS result = m_pCurrentBuffer->GetGpuVirtualAddress();
	uint64_t addressOffset = UpperBounds(result, (uint64_t)alignment) - result;
	//	↑よほど大きな値じゃなければ0のはず
	//  64kBアライメント標準で行われるようなので、128kB以上の指定じゃなければ、値が入ることはない。そしてそういうアライメントが必要になることはないはず

	//memcpy( ((uint8_t*)m_pCurrentBuffer->GetCpuAddress()) + addressOffset, srcData, srcDataSize);
	cpuAddress = ((uint8_t*)m_pCurrentBuffer->GetCpuAddress()) + addressOffset;

	m_nCurrentBufferUsedSize = (uint32_t)(requestSize + addressOffset);


	return GpuBufferRange(m_pCurrentBuffer->GetD3DResource(), addressOffset, cpuAddress, result + addressOffset, requestSize);

}

