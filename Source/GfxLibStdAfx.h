#ifndef __INCLUDE_GFXLIB_STDAFX_H__
#define	__INCLUDE_GFXLIB_STDAFX_H__



/*

	内部用インクルード

*/





#include <assert.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <stdint.h>



void	GfxLibErrorLog(const char *str,...);
void	GfxLibErrorLog( const wchar_t *str,... );

#define		GFX_RELEASE( ptr )		if ( ptr )	{	(ptr)->Release(); (ptr)=NULL;	}

#define		GFX_ASSERT( exp , ... )		{	assert(exp);	}

#define		GFX_ERROR_LOG( ... )		{GfxLibErrorLog(__VA_ARGS__);}



#include "Util/GfxD3DPtr.h"



#endif // !__INCLUDE_GFXLIB_STDAFX_H__
