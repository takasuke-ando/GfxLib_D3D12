/***************************************************************
	@file	32bit CRC計算を行うクラス
	@par	[説明]
		32bit CRC計算を行うクラス
		実装は
		https://ja.wikipedia.org/wiki/%E5%B7%A1%E5%9B%9E%E5%86%97%E9%95%B7%E6%A4%9C%E6%9F%BB
		を参考にした
		（ライセンス明記が無いため、気になる場合は置き換えを検討）

***************************************************************/



#include "stdafx.h"

#include "Util/GfxCrc32.h"


using namespace GfxLib;


uint32_t	Crc32::m_crcTable[256];


void	Crc32::StaticInitialize()
{

	for (uint32_t i = 0; i < 256; i++) {
		uint32_t c = i;
		for (int j = 0; j < 8; j++) {
			c = (c & 1) ? (0xEDB88320 ^ (c >> 1)) : (c >> 1);
		}
		m_crcTable[i] = c;
	}

}




Crc32::Crc32()
{
	m_Value = 0xffffffff;

}



Crc32::~Crc32()
{


}



void	Crc32::Update(const void * _buf, size_t len)
{
	const uint8_t *buf = (const uint8_t*)_buf;

	for (size_t i = 0; i < len; i++) {
		m_Value = m_crcTable[(m_Value ^ buf[i]) & 0xFF] ^ (m_Value >> 8);
	}

}
