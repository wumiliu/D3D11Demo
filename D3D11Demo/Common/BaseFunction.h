#pragma once
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <string>
class BaseFunction
{
public:
	static wchar_t* gAnsiToUnicode(const char* szStr);
	static std::string UnicodeToANSI(const wchar_t* str);
};

