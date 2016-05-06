#ifndef __INCLUDE_GFXLIB_STDAFX_H__
#define	__INCLUDE_GFXLIB_STDAFX_H__



/*

	内部用インクルード

*/





#include <assert.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdint.h>
#include <vector>
#include <directxmath.h>



#define		GFX_RELEASE( ptr )		if ( ptr )	{	(ptr)->Release(); (ptr)=NULL;	}

#define		GFX_ASSERT( exp , ... )		{	assert(exp);	}


namespace GfxLib {

	enum class DebugLevel : uint32_t {
		None = 0,
		Info,
		Warn,
		Error,
	};

}

void	GfxLibDebugEvent(GfxLib::DebugLevel ,  const char *str, ...);
void	GfxLibDebugEvent(GfxLib::DebugLevel ,  const wchar_t *str, ...);



#define		GFX_ERROR_LOG( ... )		{GfxLibDebugEvent(GfxLib::DebugLevel::Error,__VA_ARGS__);}



#include "Util/GfxD3DPtr.h"


#define		GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE		(false)



#endif // !__INCLUDE_GFXLIB_STDAFX_H__
