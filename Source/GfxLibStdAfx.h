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


	/*
		デバッグレベル
	*/
	enum class DebugLevel : uint32_t {
		None = 0,	//	なし
		Info,		//	詳細な情報。普段は、出していると邪魔になる。
		Warn,		//	警告。通常起こり得るが、別のエラーを引き起こす可能性があるので、注意しておいたほうが良い。
		Error,		//	エラー。起きてはいけない。

		Num,
	};

	/*
		各種エラーが起きた時の動作を設定する
	*/
	enum class DebugAction : uint32_t {

		Ignore = 0,	//	無視
		Log,		//	ログを出す
		Assert,		//	ログをだし、アサートする
		
	};

	extern DebugAction	g_DebugLevelAction[];

}

void	GfxLibDebugEvent(GfxLib::DebugLevel ,  const wchar_t *file , uint32_t line , const char *str, ...);
void	GfxLibDebugEvent(GfxLib::DebugLevel ,  const wchar_t *file, uint32_t line, const wchar_t *str, ...);



#define		GFX_ERROR( ... )		{if (g_DebugLevelAction[(uint32_t)GfxLib::DebugLevel::Error] >= DebugAction::Log)GfxLibDebugEvent(GfxLib::DebugLevel::Error,_CRT_WIDE(__FILE__), (unsigned)(__LINE__),__VA_ARGS__);}
#define		GFX_WARN( ... )			{if (g_DebugLevelAction[(uint32_t)GfxLib::DebugLevel::Warn] >= DebugAction::Log)GfxLibDebugEvent(GfxLib::DebugLevel::Warn,_CRT_WIDE(__FILE__), (unsigned)(__LINE__),__VA_ARGS__);}
#define		GFX_INFO( ... )			{if (g_DebugLevelAction[(uint32_t)GfxLib::DebugLevel::Info] >= DebugAction::Log)GfxLibDebugEvent(GfxLib::DebugLevel::Info,_CRT_WIDE(__FILE__), (unsigned)(__LINE__),__VA_ARGS__);}



#include "Util/GfxD3DPtr.h"


#define		GFX_DEFAULT_DELAY_DELETE_FLAG_ON_FINALIZE		(false)

#define		GFX_DEFAULT_DEPTH_CLEAR_VALUE					(1.f)




#endif // !__INCLUDE_GFXLIB_STDAFX_H__
