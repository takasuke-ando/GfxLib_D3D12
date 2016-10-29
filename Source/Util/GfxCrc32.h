#ifndef __INCLUDE_GFXCRC32_H__
#define __INCLUDE_GFXCRC32_H__


/***************************************************************
	@brief	32bit CRC計算を行うクラス
	@par	[説明]
	@param
*/

namespace GfxLib
{

	class Crc32
	{
	public:
		Crc32();
		~Crc32();


		void	Update(const void * buf, size_t len);


		uint32_t	GetValue() const {
			return m_Value ^ 0xffffffff;
		}

		static void	StaticInitialize();

	private:

		static uint32_t m_crcTable[256];

		uint32_t	m_Value;

	};


}

#endif // !__INCLUDE_GFXCRC32_H__
