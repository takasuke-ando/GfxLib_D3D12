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
			case	Format::D16_UNORM:				return Format::R16_UNORM;
			default:
				break;
			}

			return format;
		}


		/*

			DSV format => Buffer , SRV , stencilSRV formatへの変換を行う
			
		*/
		inline	void	GetBufferAndSrvFormatFromDepth(Format depthFormat, Format& buffFormat , Format& srvFormat , Format& stencilSrvFormat)
		{


			buffFormat = Format::UNKNOWN;
			srvFormat = Format::UNKNOWN;
			stencilSrvFormat = Format::UNKNOWN;

			switch (depthFormat)
			{
			case Format::D24_UNORM_S8_UINT:
				buffFormat = Format::R24G8_TYPELESS;
				srvFormat = Format::R24_UNORM_X8_TYPELESS;
				stencilSrvFormat = Format::X24_TYPELESS_G8_UINT;
				break;
			case Format::D32_FLOAT:
				buffFormat = Format::R32_TYPELESS;
				srvFormat = Format::R32_FLOAT;
				break;
			case Format::D32_FLOAT_S8X24_UINT:
				buffFormat = Format::R32G8X24_TYPELESS;
				srvFormat = Format::R32_FLOAT_X8X24_TYPELESS;
				stencilSrvFormat = Format::X32_TYPELESS_G8X24_UINT;
				break;
			case Format::D16_UNORM:
				buffFormat = Format::R16_TYPELESS;
				srvFormat = Format::R16_UNORM;
				break;
			default:
				break;
			};

			
		}



	}


}


#endif // !__INCLUDE_GFXFORMATUITL_H__
