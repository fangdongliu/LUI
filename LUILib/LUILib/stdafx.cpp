// stdafx.cpp : 只包括标准包含文件的源文件
// LUILib.pch 将作为预编译标头
// stdafx.obj 将包含预编译类型信息

#include "stdafx.h"

// TODO: 在 STDAFX.H 中引用任何所需的附加头文件，
//而不是在此文件中引用

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")



void UTF8ToUnicode(const char *cUTF8, std::wstring& dest)
{
	int nLen = MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, NULL, 0);
	wchar_t* s = new wchar_t[nLen + 1];
	MultiByteToWideChar(CP_UTF8, 0, cUTF8, -1, s, nLen);
	dest = s;
	delete[]s;
}

bool NextString(std::string&a, std::string&b, int& start) {

	int end;
	if ((start = a.find_first_not_of(' ', start)) != a.npos) {
		if ((end = a.find_first_of(' ', start)) != a.npos) {
			b = a.substr(start, end - start);
		}
		else {
			b = a.substr(start);
		}
		start = end;
		return true;
	}
	return false;
}