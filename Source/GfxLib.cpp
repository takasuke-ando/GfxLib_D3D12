/*******************************************************************************

	ライブラリ全体で使用されるマクロや関数の定義など



*/






#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>

void GfxLibDebugEvent(GfxLib::DebugLevel, const char *str, ...)
{

	char buff[256];

	va_list list;
	va_start(list, str);

	vsprintf_s(buff, str, list);

	va_end(list);

	OutputDebugStringA(buff);

}

void GfxLibDebugEvent(GfxLib::DebugLevel, const wchar_t *str, ...)
{
	wchar_t buff[256];

	va_list list;
	va_start(list, str);

	wvsprintf(buff, str, list );

	va_end(list);

	OutputDebugStringW(buff);
}

