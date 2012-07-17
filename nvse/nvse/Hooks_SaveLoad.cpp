#include "SafeWrite.h"
#include "Hooks_SaveLoad.h"
#include "PluginAPI.h"
#include "PluginManager.h"
//#include "Serialization.h"

#if RUNTIME

bool g_gameLoaded = false;
static const char* LoadGameMessage = "---Finished loading game: %s";
static const char* SaveGameMessage = "---Finished saving game: %s";

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	static const UInt32 kLoadGamePatchAddr =	0x00848C3C;
	static const UInt32 kLoadGameRetnAddr =		0x00848C41;

	static const UInt32 kSaveGamePatchAddr =	0x00847C45;		// push SaveGameMessage
	static const UInt32 kSaveGameRetnAddr =		0x00847C4A;

//	static const UInt32 kNewGamePatchAddr =		0x006816D7;		// overwrite nullsub(void) call
//	static const UInt32 kDeleteGamePatchAddr =	0x006D54FC;		// DeleteFile() call	
//	static const UInt32 kRenameGamePatchAddr =	0x006DF821;		// call to rename()
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	static const UInt32 kLoadGamePatchAddr =	0x0084883C;
	static const UInt32 kLoadGameRetnAddr =		0x00848841;

	static const UInt32 kSaveGamePatchAddr =	0x00847845;		// push SaveGameMessage
	static const UInt32 kSaveGameRetnAddr =		0x0084784A;
#else
#error
#endif

static void __stdcall DoLoadGameHook(const char* saveFilePath)
{
	g_gameLoaded = true;

	_MESSAGE("DoLoadGameHook: %s", saveFilePath);
	//no serialization support, just events, call the plugin mgr directly
	//to dispatch the notification
    PluginManager::Dispatch_Message(0, NVSEMessagingInterface::kMessage_LoadGame, (void*)saveFilePath, strlen(saveFilePath), NULL);
//	Serialization::HandleLoadGame(saveFilePath);
}

static __declspec(naked) void LoadGameHook(void)
{
	__asm
	{
		pushad
		push		ecx				// filepath for savegame
		call		DoLoadGameHook
		popad

		// overwritten code
		push		offset	LoadGameMessage
		jmp			[kLoadGameRetnAddr]
	}
}

static void __stdcall DoSaveGameHook(const char* saveFilePath)
{
	_MESSAGE("DoSaveGameHook: %s", saveFilePath);
//	Serialization::HandleSaveGame(saveFilePath);
}

static __declspec(naked) void SaveGameHook(void)
{
	__asm
	{
		pushad
		push		edx				// filepath for savegame
		call		DoSaveGameHook
		popad

		// overwritten code
		push		offset	SaveGameMessage
		jmp			[kSaveGameRetnAddr]
	}
}

#if 0
static void NewGameHook(void)
{
	_MESSAGE("NewGameHook");

	Serialization::HandleNewGame();
}

static void __stdcall DeleteGameHook(const char * path)
{
	_MESSAGE("DeleteGameHook: %s", path);

	Serialization::HandleDeleteGame(path);

	DeleteFile(path);
}

static void RenameGameHook(const char * oldPath, const char * newPath)
{
	_MESSAGE("RenameGameHook: %s -> %s", oldPath, newPath);

	Serialization::HandleRenameGame(oldPath, newPath);

	rename(oldPath, newPath);
}
#endif

void Hook_SaveLoad_Init(void)
{
	WriteRelJump(kLoadGamePatchAddr, (UInt32)&LoadGameHook);

#if _DEBUG
	WriteRelJump(kSaveGamePatchAddr, (UInt32)&SaveGameHook);
//	WriteRelCall(kNewGamePatchAddr, (UInt32)&NewGameHook);
//	WriteRelCall(kDeleteGamePatchAddr, (UInt32)&DeleteGameHook);
//	SafeWrite8(kDeleteGamePatchAddr + 5, 0x90);		// nop out leftover byte from original instruction
//	WriteRelCall(kRenameGamePatchAddr, (UInt32)&RenameGameHook);
#endif

}

#endif
