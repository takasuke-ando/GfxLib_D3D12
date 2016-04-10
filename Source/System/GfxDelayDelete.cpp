/***************************************************************
@brief	遅延開放
@par	[説明]
	遅延開放
	D3Dオブジェクトなど、GPUから参照されている可能性がある
	メモリ上のオブジェクトを、安全なフレーム数遅延させた上で
	開放する
***************************************************************/





#include "stdafx.h"
#include "GfxDelayDelete.h"



using namespace GfxLib;


DelayDelete::DelayDelete()
	:m_nCurrentIndex(0)
{
}

DelayDelete::~DelayDelete()
{
}


void	DelayDelete::Update()
{
	if (++m_nCurrentIndex >= MAX_FRAME_COUNT) {
		m_nCurrentIndex = 0;
	}


	//	このフレーム情報を再利用可能にする
	FRAME_INFO &frameInfo = m_aFrameInfo[m_nCurrentIndex];


	for (auto &it : frameInfo.vecIUnknown) {
		it->Release();
	}


	//	swap テクニックでメモリ解放したほうが良い？


}



void	DelayDelete::DeleteAll()
{
	for (auto & frameInfo : m_aFrameInfo) {
		for (auto &it : frameInfo.vecIUnknown) {
			it->Release();
		}
	}

}




void	DelayDelete::Regist(IUnknown* object)
{
	if (!object)return;

	object->AddRef();
	m_aFrameInfo[m_nCurrentIndex].vecIUnknown.push_back(object);


}

