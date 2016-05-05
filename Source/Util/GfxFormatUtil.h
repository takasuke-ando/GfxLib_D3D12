#ifndef __INCLUDE_GFXFORMATUITL_H__
#define	__INCLUDE_GFXFORMATUITL_H__



#include "System/GfxDefines.h"


namespace GfxLib
{


	namespace FormatUtil
	{


		inline	bool IsDepthStencil(Format format)
		{
			return format == Format::D32_FLOAT ||
				format == Format::D32_FLOAT_S8X24_UINT ||
				format == Format::D24_UNORM_S8_UINT;

		}

		

		/*
			DSV format => SRV formatへの変換可能
			DSV formatでない場合、そのまま値を返す
		*/
		inline	Format GetSRVFormat(Format format)
		{

			switch (format) {
			case	Format::D32_FLOAT:				return Format::R32_FLOAT;
			case	Format::D32_FLOAT_S8X24_UINT:	return Format::R32_FLOAT_X8X24_TYPELESS;
			case	Format::D24_UNORM_S8_UINT:		return Format::R24_UNORM_X8_TYPELESS;
			default:
				break;
			}

			return format;
		}





	}


}


#endif // !__INCLUDE_GFXFORMATUITL_H__
