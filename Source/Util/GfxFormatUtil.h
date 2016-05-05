#ifndef __INCLUDE_GFXFORMATUITL_H__
#define	__INCLUDE_GFXFORMATUITL_H__



#include "System/GfxDefines.h"


namespace GfxLib
{


	namespace FormatUtil
	{


		bool IsDepthStencil(Format format)
		{
			return format == Format::D32_FLOAT ||
				format == Format::D32_FLOAT_S8X24_UINT ||
				format == Format::D24_UNORM_S8_UINT;

		}

		






	}


}


#endif // !__INCLUDE_GFXFORMATUITL_H__
