#include "CommandTable.h"
#include "Hooks_DirectInput8Create.h"
#include "Hooks_SaveLoad.h"
#include "Hooks_Editor.h"
#include "Hooks_Logging.h"
#include "SafeWrite.h"
#include "Utilities.h"
#include "Commands_Input.h"

#if RUNTIME
IDebugLog	gLog("nvse.log");

STATIC_ASSERT(
	(RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525) ||
	(RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng));
#else
IDebugLog	gLog("nvse_editor.log");

STATIC_ASSERT(EDITOR_VERSION == CS_VERSION_1_4_0_518);
#endif

#if RUNTIME

// fix dinput code so it doesn't acquire the keyboard/mouse in exclusive mode
// bBackground Mouse works on startup, but when gaining focus that setting is ignored
// there's probably a better way to fix the bug but this is good enough
void PatchCoopLevel(void)
{
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	SafeWrite8(0x00A22371 + 1, 0x16);
	SafeWrite8(0x00A2259D + 1, 0x06);
	SafeWrite8(0x00A2387F + 1, 0x06);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	SafeWrite8(0x00A227A1 + 1, 0x16);
	SafeWrite8(0x00A229CB + 1, 0x06);
	SafeWrite8(0x00A23CAD + 1, 0x06);
#else
#error
#endif
}

// fix render path selection
void PatchRenderPath(void)
{
	//	0	none
	//	1	1x
	//	2	2
	//	3	2a (96)
	//	4	2b (96)
	//	5	2a
	//	6	2b
	//	7	3

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	for(UInt32 i = 0; i < 6; i++)
		SafeWrite8(0x00B4F94D + i, 0x90);	// nop

	SafeWrite32(0x00B4F953 + 6, 7);	// render path goes here
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	for(UInt32 i = 0; i < 6; i++)
		SafeWrite8(0x00B4FCBD + i, 0x90);	// nop

	SafeWrite32(0x00B4FCC3 + 6, 7);	// render path goes here
#else
#error
#endif
}

#endif

void WaitForDebugger(void)
{
	while(!IsDebuggerPresent())
	{
		Sleep(10);
	}

	Sleep(1000 * 2);
}

void NVSE_Initialize(void)
{
#ifndef _DEBUG
	__try {
#endif

		FILETIME	now;
		GetSystemTimeAsFileTime(&now);

#if RUNTIME
		_MESSAGE("NVSE runtime: initialize (version = %d.%d.%d %08X %08X%08X)",
			NVSE_VERSION_INTEGER, NVSE_VERSION_INTEGER_MINOR, NVSE_VERSION_INTEGER_BETA, RUNTIME_VERSION,
			now.dwHighDateTime, now.dwLowDateTime);
#else
		_MESSAGE("NVSE editor: initialize (version = %d.%d.%d %08X %08X%08X)",
			NVSE_VERSION_INTEGER, NVSE_VERSION_INTEGER_MINOR, NVSE_VERSION_INTEGER_BETA, EDITOR_VERSION,
			now.dwHighDateTime, now.dwLowDateTime);
#endif
		_MESSAGE("imagebase = %08X", GetModuleHandle(NULL));

#ifdef _DEBUG
		SetPriorityClass(GetCurrentProcess(), IDLE_PRIORITY_CLASS);

#if RUNTIME
		PatchCoopLevel();
#endif

//		WaitForDebugger();
#endif

//		PatchRenderPath();

		MersenneTwister::init_genrand(GetTickCount());
		CommandTable::Init();

#if RUNTIME
		Commands_Input_Init();
		Hook_DirectInput8Create_Init();
		Hook_SaveLoad_Init();
		Hook_Logging_Init();
#endif

#if EDITOR
		Hook_Editor_Init();
#endif

		FlushInstructionCache(GetCurrentProcess(), NULL, 0);

#ifndef _DEBUG
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		_ERROR("exception");
	}
#endif

	_MESSAGE("init complete");
}

void NVSE_DeInitialize(void)
{
	//
}

extern "C" {

void StartNVSE(void)
{
	NVSE_Initialize();
}

BOOL WINAPI DllMain(HANDLE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	switch(dwReason)
	{
		case DLL_PROCESS_ATTACH:
			NVSE_Initialize();
			break;

		case DLL_PROCESS_DETACH:
			NVSE_DeInitialize();
			break;
	};

	return TRUE;
}

};
