#include "YRAggressiveStance.h"

#include <Drawing.h>

#include <Utilities/Debug.h>
#include <Utilities/Patch.h>
#include <Utilities/Macro.h>

HANDLE YRAggressiveStance::hInstance = 0;

char YRAggressiveStance::readBuffer[YRAggressiveStance::readLength];
wchar_t YRAggressiveStance::wideBuffer[YRAggressiveStance::readLength];

void YRAggressiveStance::ExeRun()
{
	Patch::ApplyStatic();
}

// =============================
// hooks

bool __stdcall DllMain(HANDLE hInstance, DWORD dwReason, LPVOID v)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		YRAggressiveStance::hInstance = hInstance;
	}
	return true;
}

DEFINE_HOOK(0x7CD810, ExeRun, 0x9)
{
	YRAggressiveStance::ExeRun();
	return 0;
}

DEFINE_HOOK(0x52F639, _YR_CmdLineParse, 0x5)
{
	GET(char**, ppArgs, ESI);
	GET(int, nNumArgs, EDI);

	Debug::LogDeferredFinalize();
	return 0;
}
