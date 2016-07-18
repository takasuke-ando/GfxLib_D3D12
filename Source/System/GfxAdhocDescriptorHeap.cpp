/***************************************************************
	@file	GfxAdhocDescriptorHeap
	@par	[説明]

		フレーム中に一時的な領域として確保するデスクリプタヒープを管理する

		AdhocDescriptorHeapクラスは確保されたDescriptorHeapを管理し
		使いまわしなどを行う

		AdhocDescriptorHeapClientクラスはコマンドリスト毎スレッドローカルに作成されて
		同期オブジェクトの操作などなしに、リニアにDescriptorHeapの領域を確保していく
		領域が不足したら、AdhocDescriptorHeapクラス側に新しいヒープを要求する




*/




#include "stdafx.h"

#include "System/GfxAdhocDescriptorHeap.h"

#include "Resource/GfxDescriptorHeap.h"


using namespace GfxLib;


namespace {


	const uint32_t MaxBufferSize = 1024;	//	一つのDescriptorHeapに格納可能なDescriptor数
	

}







/***************************************************************
	@brief	コンストラクタ
	@par	[説明]
	@param
*/
AdhocDescriptorHeap::AdhocDescriptorHeap(DescriptorHeapType heapType )
	: m_heapType(heapType)
	, m_nCurrentIndex(0)
	//, m_pCurrentHeap(nullptr)
	//, m_nCurrentHeapUsedSize(0)
	, m_allocatedCount(0)
{

}


/***************************************************************
	@brief	デストラクタ
	@par	[説明]
	@param
*/
AdhocDescriptorHeap::~AdhocDescriptorHeap()
{


	for ( auto &vec : m_aUsingDescHeap ) {

		for (auto &desc : vec) {
			delete desc;
		}

		vec.clear();

	}


	for (auto &desc : m_FreeDescHeap ) {
		delete desc;
	}
	m_FreeDescHeap.clear();

	//delete m_pCurrentHeap;
	//m_pCurrentHeap = nullptr;
	
}


/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める

*/
#if 0
DescriptorHeap*	AdhocDescriptorHeap::Require(uint32_t requestSize, uint32_t &startIndex)
{

	if (m_pCurrentHeap) {

		// 現在のヒープをそのまま使える
		const uint32_t capacity = m_pCurrentHeap->GetBufferCount();

		if (requestSize + m_nCurrentHeapUsedSize  <= capacity ) {
			
			startIndex = m_nCurrentHeapUsedSize;
			m_nCurrentHeapUsedSize += requestSize;

			return m_pCurrentHeap;
		}
		else {

			// 利用中リストに入れる
			m_aUsingDescHeap[m_nCurrentIndex].push_back(m_pCurrentHeap);

			m_nCurrentHeapUsedSize = 0;
			m_pCurrentHeap = nullptr;

		}

	}

	
	if (requestSize > MaxBufferSize) {
		GFX_ERROR(L"AdhocDescriptorHeap::Require size over (%d)", requestSize);
		return nullptr;
	}

	if (m_FreeDescHeap.size() == 0) {
		// 新たにDescHeapの作成
		m_pCurrentHeap = new DescriptorHeap;

		// とりあえずCBV/SRV/UAV用として
		bool b = FALSE;
		if (m_heapType == DescriptorHeapType::CBV_SRV_UAV) {
			b = m_pCurrentHeap->InitializeCBV_SRV_UAV(MaxBufferSize); // サイズ適当
		}
		else if (m_heapType == DescriptorHeapType::SAMPLER) {
			b = m_pCurrentHeap->InitializeSampler(MaxBufferSize);
		}

		if (!b) {
			delete m_pCurrentHeap;
			m_pCurrentHeap = nullptr;

			return nullptr;
		}
		++m_allocatedCount;
	}
	else {

		// 未使用リストから取り出す
		m_pCurrentHeap = m_FreeDescHeap.back();
		m_FreeDescHeap.pop_back();

	}

	startIndex = 0;
	m_nCurrentHeapUsedSize = requestSize;


	return m_pCurrentHeap;
}
#endif


/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
AdhocDescriptorHeapClientから呼び出される

*/
DescriptorHeap*	AdhocDescriptorHeap::Require()
{

	DescriptorHeap *descHeap = nullptr;

	if (m_FreeDescHeap.size() == 0) {
		// 新たにDescHeapの作成
		descHeap = new DescriptorHeap;

		// とりあえずCBV/SRV/UAV用として
		bool b = FALSE;
		if (m_heapType == DescriptorHeapType::CBV_SRV_UAV) {
			b = descHeap->InitializeCBV_SRV_UAV(MaxBufferSize); // サイズ適当
		}
		else if (m_heapType == DescriptorHeapType::SAMPLER) {
			b = descHeap->InitializeSampler(MaxBufferSize);
		}

		if (!b) {
			delete descHeap;

			return nullptr;
		}
		++m_allocatedCount;
	}
	else {

		// 未使用リストから取り出す
		descHeap = m_FreeDescHeap.back();
		m_FreeDescHeap.pop_back();

	}


	m_aUsingDescHeap[m_nCurrentIndex].push_back(descHeap);


	return descHeap;
}




/***************************************************************
@brief	次のフレーム
@par	[説明]
	毎フレーム呼び出す
	MAX_FRAME_QUEUEフレーム前の描画が完全に完了していることが保証されていないといけない
@param
*/
void AdhocDescriptorHeap::NextFrame()
{
	//if (m_pCurrentHeap) {
	//	m_aUsingDescHeap[m_nCurrentIndex].push_back(m_pCurrentHeap);
	//	m_pCurrentHeap = nullptr;
	//}

	// Rotate Index
	m_nCurrentIndex = (m_nCurrentIndex+1) % _countof(m_aUsingDescHeap);

	// 再利用可能になったヒープを回収する
	for (auto &desc : m_aUsingDescHeap[m_nCurrentIndex]) {

		m_FreeDescHeap.push_back(desc);

	}
	m_aUsingDescHeap[m_nCurrentIndex].clear();

}




AdhocDescriptorHeapClient::AdhocDescriptorHeapClient()
	:m_pHost(nullptr)
	,m_pCurrentHeap(nullptr)
	,m_nCurrentHeapUsedSize(0)
{

}



AdhocDescriptorHeapClient::~AdhocDescriptorHeapClient()
{


}



/***************************************************************
@brief	初期化
@par	[説明]
@param
*/
void	AdhocDescriptorHeapClient::Initialize(AdhocDescriptorHeap* host)
{

	m_pHost = host;

}

/***************************************************************
@brief	リセット
@par	[説明]
再利用する場合、フレームの最初に呼び出す
@param
*/
void AdhocDescriptorHeapClient::Reset()
{

	m_pCurrentHeap = nullptr;
	m_nCurrentHeapUsedSize = 0;

}



/***************************************************************
@brief	利用可能なデスクリプタヒープを取得する
@par	[説明]
このフレームの間だけ、利用可能なデスクリプタヒープを取得する
@param[in]	size:		要求サイズ
@param[out]	startIndex:	ヒープのこのインデックスから書き込める

*/
DescriptorHeap*	AdhocDescriptorHeapClient::Require(uint32_t requestSize, uint32_t &startIndex)
{

	if (m_pCurrentHeap) {

		// 現在のヒープをそのまま使える
		const uint32_t capacity = m_pCurrentHeap->GetBufferCount();

		if (requestSize + m_nCurrentHeapUsedSize <= capacity) {

			startIndex = m_nCurrentHeapUsedSize;
			m_nCurrentHeapUsedSize += requestSize;

			return m_pCurrentHeap;
		}
		else {

			// 利用中リストに入れる
			m_nCurrentHeapUsedSize = 0;
			m_pCurrentHeap = nullptr;

		}

	}


	if (requestSize > MaxBufferSize) {
		GFX_ERROR(L"AdhocDescriptorHeap::Require size over (%d)", requestSize);
		return nullptr;
	}

#if 0
	if (m_FreeDescHeap.size() == 0) {
		// 新たにDescHeapの作成
		m_pCurrentHeap = new DescriptorHeap;

		// とりあえずCBV/SRV/UAV用として
		bool b = FALSE;
		if (m_heapType == DescriptorHeapType::CBV_SRV_UAV) {
			b = m_pCurrentHeap->InitializeCBV_SRV_UAV(MaxBufferSize); // サイズ適当
		}
		else if (m_heapType == DescriptorHeapType::SAMPLER) {
			b = m_pCurrentHeap->InitializeSampler(MaxBufferSize);
		}

		if (!b) {
			delete m_pCurrentHeap;
			m_pCurrentHeap = nullptr;

			return nullptr;
		}
		++m_allocatedCount;
	}
	else {

		// 未使用リストから取り出す
		m_pCurrentHeap = m_FreeDescHeap.back();
		m_FreeDescHeap.pop_back();

	}
#endif

	m_pCurrentHeap = m_pHost->Require();


	startIndex = 0;
	m_nCurrentHeapUsedSize = requestSize;


	return m_pCurrentHeap;
}

