#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料

#include<windows.h>
#include<string>
#include<algorithm>
#include<wincodec.h>
#include<assert.h>
#include"Timer.h"

#ifndef HINST_THISCOMPONENT 
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONET ((HINSTANCE)&__ImageBase) 
#endif

#define WM_SYSMESSAGE (WM_APP+1)
#define WM_USERMESSAGE (WM_APP+2)

#ifdef _DEBUG
#define FDASSERT(f) assert(f);
#else 
#define FDASSERT(f) f
#endif

template <typename InterfaceType>
inline void SafeRelease(InterfaceType** currentObject)
{
	if (*currentObject != nullptr)
	{
		(*currentObject)->Release();
		*currentObject = nullptr;
	}
}

extern void UTF8ToUnicode(const char *cUTF8, std::wstring& dest);
extern bool NextString(std::string&a, std::string&b, int& start);

