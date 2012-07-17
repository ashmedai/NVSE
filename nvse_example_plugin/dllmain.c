#include <Windows.h>

IDebugLog gLog("nvse_extender.log");
static void OnAttach(void);

BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
	    _MESSAGE("NVSEEX attached to process %d", dwReason);
		OnAttach();
	}
  return TRUE;
}

static void OnAttach(void)
{
	_MESSAGE("NVSEEX attached to process");
}