#pragma once
#include <Windows.h>

#include <string>

class YRAggressiveStance
{
public:
	static void ExeRun();

	//variables
	static HANDLE hInstance;

	static const size_t readLength = 2048;
	static char readBuffer[readLength];
	static wchar_t wideBuffer[readLength];
};
