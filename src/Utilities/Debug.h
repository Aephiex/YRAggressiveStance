#pragma once

#include <Windows.h>

class Debug
{
public:
	enum class ExitCode : int
	{
		Undefined = -1,
		SLFail = 114514,
		BadINIUsage = 1919810,
	};

	static char StringBuffer[0x1000];
	static char FinalStringBuffer[0x1000];
	static char DeferredStringBuffer[0x1000];
	static int CurrentBufferSize;

	static void Log(const char* pFormat, ...);
	static void LogGame(const char* pFormat, ...);
	static void LogDeferred(const char* pFormat, ...);
	static void LogDeferredFinalize();
	static void LogAndMessage(const char* pFormat, ...);
	static void LogWithVArgs(const char* pFormat, va_list args);
	static void INIParseFailed(const char* section, const char* flag, const char* value, const char* Message = nullptr);
	static void FatalErrorAndExit(const char* pFormat, ...);
	static void FatalErrorAndExit(ExitCode nExitCode, const char* pFormat, ...);
};
