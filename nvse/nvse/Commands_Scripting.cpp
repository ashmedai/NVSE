#include "Commands_Scripting.h"
#include "GameForms.h"
#include "GameAPI.h"
#include <intrin.h>

static const UInt32 kMaxSavedIPs = 0x100;
static const UInt32	kMaxSavedIPStack = 20;	// twice the supposed limit

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
static const void * kOpHandlerRetnAddr = (void *)0x005E234B;
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
static const void * kOpHandlerRetnAddr = (void *)0x005E225B;
#else
#error
#endif

struct SavedIPInfo
{
	UInt32	ip;
	UInt32	stackDepth;
	UInt32	stack[kMaxSavedIPStack];
};

struct ScriptRunner
{
	UInt32	pad[0x20 >> 2];
	UInt32	stackDepth;
	UInt32	stack[1];
};

static SavedIPInfo s_savedIPTable[kMaxSavedIPs] = { { 0 } };

// ### stack abuse! get a pointer to the parent's stack frame
// ### valid when parent is called from kOpHandlerRetnAddr
static ScriptRunner * GetScriptRunner(UInt32 * opcodeOffsetPtr)
{
	UInt8			* scriptRunnerEBP = ((UInt8 *)opcodeOffsetPtr) + 0x10;
	ScriptRunner	** scriptRunnerPtr = (ScriptRunner **)(scriptRunnerEBP - 0xED0);	// this
	ScriptRunner	* scriptRunner = *scriptRunnerPtr;

	return scriptRunner;
}

static SInt32 * GetCalculatedOpLength(UInt32 * opcodeOffsetPtr)
{
	UInt8	* scriptRunnerEBP = ((UInt8 *)opcodeOffsetPtr) + 0x10;
	UInt8	* parentEBP = scriptRunnerEBP + 0x7B0;
	SInt32	* opLengthPtr = (SInt32 *)(parentEBP - 0x14);

	return opLengthPtr;
}

bool Cmd_Label_Execute(COMMAND_ARGS)
{
	// make sure this is only called from the main execution loop
	ASSERT_STR(scriptData == scriptObj->data, "Label may not be called inside a set or if statement");
	ASSERT(_ReturnAddress() == kOpHandlerRetnAddr);

	UInt32	idx = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &idx)) return true;

	// this must happen after extractargs updates opcodeOffsetPtr so it points to the next instruction
	if(idx < kMaxSavedIPs)
	{
		SavedIPInfo		* info = &s_savedIPTable[idx];
		ScriptRunner	* scriptRunner = GetScriptRunner(opcodeOffsetPtr);

		// save ip
		info->ip = *opcodeOffsetPtr;

		// save if/else/endif stack
		info->stackDepth = scriptRunner->stackDepth;
		ASSERT((info->stackDepth + 1) < kMaxSavedIPStack);

		memcpy(info->stack, scriptRunner->stack, (info->stackDepth + 1) * sizeof(UInt32));
	}

	return true;
}

bool Cmd_Goto_Execute(COMMAND_ARGS)
{
	// make sure this is only called from the main execution loop
	ASSERT_STR(scriptData == scriptObj->data, "Goto may not be called inside a set or if statement");
	ASSERT(_ReturnAddress() == kOpHandlerRetnAddr);

	UInt32	idx = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &idx)) return true;

	if(idx < kMaxSavedIPs)
	{
		SavedIPInfo		* info = &s_savedIPTable[idx];
		ScriptRunner	* scriptRunner = GetScriptRunner(opcodeOffsetPtr);
		SInt32			* calculatedOpLength = GetCalculatedOpLength(opcodeOffsetPtr);

		// restore ip
		*calculatedOpLength += info->ip - *opcodeOffsetPtr;

		// restore the if/else/endif stack
		scriptRunner->stackDepth = info->stackDepth;
		memcpy(scriptRunner->stack, info->stack, (info->stackDepth + 1) * sizeof(UInt32));
	}

	return true;
}
