#include "Hooks_Editor.h"
#include "SafeWrite.h"

#if EDITOR

static char s_InfixToPostfixBuf[0x800];
static const UInt32 kInfixToPostfixBufLen = sizeof(s_InfixToPostfixBuf);

static const UInt32 kInfixToPostfixHookAddr_1 =	0x005B1AD9;
static const UInt32 kInfixToPostfixRetnAddr_1 =	0x005B1ADE;

static const UInt32 kInfixToPostfixHookAddr_2 =	0x005B1C6C;
static const UInt32 kInfixToPostfixRetnAddr_2 =	0x005B1C73;

static __declspec(naked) void InfixToPostfixHook_1(void)
{
	__asm {
		lea	eax, s_InfixToPostfixBuf
		push eax
		jmp	[kInfixToPostfixRetnAddr_1]
	}
}

static __declspec(naked) void InfixToPostfixHook_2(void)
{
	__asm {
		lea	edx, s_InfixToPostfixBuf
		mov	byte ptr [edi], 0x20
		jmp	[kInfixToPostfixRetnAddr_2]
	}
}

static void FixInfixToPostfixOverflow(void)
{
	// this function uses a statically allocated 64 byte work buffer that overflows really quickly
	// hook replaces the buffer, safe to use a static buffer because it isn't re-entrant and can't be called from multiple threads
	WriteRelJump(kInfixToPostfixHookAddr_1, (UInt32)&InfixToPostfixHook_1);
	WriteRelJump(kInfixToPostfixHookAddr_2, (UInt32)&InfixToPostfixHook_2);
}

void Hook_Editor_Init(void)
{
	FixInfixToPostfixOverflow();
}

#endif
