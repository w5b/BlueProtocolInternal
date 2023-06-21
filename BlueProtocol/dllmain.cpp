#include "pch.h"
#include "hooks/hooks.h"

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);

	if (ulReasonForCall != DLL_PROCESS_ATTACH)
		return TRUE;

	CloseHandle(CreateThread(0, 0, LPTHREAD_START_ROUTINE(hooks::StartHooks), hModule, 0, 0));

	return TRUE;
}
