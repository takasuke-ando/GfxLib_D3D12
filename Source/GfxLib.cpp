/*******************************************************************************

	ライブラリ全体で使用されるマクロや関数の定義など



*/






#include "stdafx.h"

#include <stdio.h>
#include <stdarg.h>



namespace GfxLib {

	DebugAction		g_DebugLevelAction[(uint32_t)DebugLevel::Num] =
	{
		DebugAction::Ignore,	//	None
		DebugAction::Log,		//	Info
		DebugAction::Log,		//	Warn
		DebugAction::Asset,		//	Error
	};

}

using namespace GfxLib;




void GfxLibDebugEvent(GfxLib::DebugLevel level, const wchar_t *file, uint32_t line, const char *str, ...)
{
	if (g_DebugLevelAction[(uint32_t)level] < DebugAction::Log)
	{
		return;
	}


	char buff[256];

	va_list list;
	va_start(list, str);

	vsprintf_s(buff, str, list);

	va_end(list);

	OutputDebugStringA(buff);

	if (g_DebugLevelAction[(uint32_t)level] < DebugAction::Asset)
	{
		return;
	}

	assert(0);

}

void GfxLibDebugEvent(GfxLib::DebugLevel level, const wchar_t *file, uint32_t line, const wchar_t *str, ...)
{
	if (g_DebugLevelAction[(uint32_t)level] < DebugAction::Log)
	{
		return;
	}

	wchar_t buff[256];

	va_list list;
	va_start(list, str);

	wvsprintf(buff, str, list );

	va_end(list);

	OutputDebugStringW(buff);

	if (g_DebugLevelAction[(uint32_t)level] < DebugAction::Asset)
	{
		return;
	}

	_wassert(buff, file, line);

}

