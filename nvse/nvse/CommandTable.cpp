#include "CommandTable.h"
#include "SafeWrite.h"
#include "GameAPI.h"
#include "GameData.h"
#include "GameObjects.h"
#include "GameEffects.h"
#include "GameExtraData.h"
#include "GameForms.h"
#include "GameProcess.h"
#include "GameRTTI.h"
#include "GameSettings.h"
#include "GameUI.h"
#include <string>
#include "Utilities.h"
#include "PluginManager.h"

#include "Commands_Console.h"
#include "Commands_Game.h"
#include "Commands_Input.h"
#include "Commands_Inventory.h"
#include "Commands_List.h"
#include "Commands_Math.h"
#include "Commands_MiscRef.h"
#include "Commands_Script.h"
#include "Commands_Scripting.h"
#include "Commands_UI.h"

CommandTable g_consoleCommands;
CommandTable g_scriptCommands;

#if RUNTIME

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525

// 1.4.0.525 runtime
UInt32 g_offsetConsoleCommandsStart = 0x0118E8E0;
UInt32 g_offsetConsoleCommandsLast = 0x011908C0;
UInt32 g_offsetScriptCommandsStart = 0x01190910;
UInt32 g_offsetScriptCommandsLast = 0x01196D10;
static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x005B1BA0;

#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng

// 1.4.0.525 nogore runtime
UInt32 g_offsetConsoleCommandsStart = 0x0118E8E0;
UInt32 g_offsetConsoleCommandsLast = 0x011908C0;
UInt32 g_offsetScriptCommandsStart = 0x01190910;
UInt32 g_offsetScriptCommandsLast = 0x01196D10;
static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x005B1C40;

#else
#error
#endif

#else

// 1.4.0.518 editor
UInt32 g_offsetConsoleCommandsStart = 0x00E9DB88;
UInt32 g_offsetConsoleCommandsLast = 0x00E9FB90;
UInt32 g_offsetScriptCommandsStart = 0x00E9FBB8;
UInt32 g_offsetScriptCommandsLast = 0x00EA5FB8;
static const Cmd_Parse g_defaultParseCommand = (Cmd_Parse)0x005C67E0;

#endif

struct PatchLocation
{
	UInt32	ptr;
	UInt32	offset;
	UInt32	type;
};

#if RUNTIME

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	{	0x005B1172, 0x00 },
	{	0x005B19B1, 0x00 },
	{	0x005B19CE, 0x04 },
	{	0x005B19F8, 0x08 },
	{	0x005BCC0A, 0x0C },
	{	0x005BCC2D, 0x00 },
	{	0x005BCC50, 0x04 },
	{	0x005BCC70, 0x0C },
	{	0x005BCC86, 0x0C },
	{	0x005BCCA6, 0x04 },
	{	0x005BCCB8, 0x04 },
	{	0x005BCCD4, 0x0C },
	{	0x005BCCE4, 0x04 },
	{	0x005BCCF4, 0x00 },
	{	0x005BCD13, 0x0C },
	{	0x005BCD23, 0x00 },
	{	0x005BCD42, 0x04 },
	{	0x005BCD54, 0x04 },
	{	0x005BCD70, 0x04 },
	{	0x005BCD80, 0x00 },
	{	0x005BCD9F, 0x00 },
	{	0x0068170B, 0x20 },
	{	0x00681722, 0x10 },
	{	0x00681752, 0x20 },
	{	0x00681AEB, 0x20 },
	{	0x00681CDE, 0x00 },
	{	0x006820FF, 0x14 },
	{	0x0068228D, 0x12 },
	{	0x006822FF, 0x14 },
	{	0x00682352, 0x14 },
	{	0x006823B2, 0x14 },
	{	0x0087A909, 0x12 },
	{	0x0087A948, 0x14 },
	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	{	0x005AEA59, 0x08 },
	{	0 },
};

// 127B / 027B
// 1280 / 0280
static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	{	0x00593909 + 1, 0 },
	{	0x005AEA57 + 6, 0 },
	{	0x005B115B + 3, 0 },
	{	0x005B19A0 + 3, (UInt32)(-0x1000) },
	{	0x005BCBDD + 6, (UInt32)(-0x1000) },

#if 0
	// ### investigate this function
	{	0x0067F0B8 + 3,	0 },
	{	0x0067F0D5 + 3, (UInt32)(-0x1000) },
	{	0x0067F0F4 + 3, (UInt32)(-0x1000) },
#endif

	{	0 },
};

#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	{	0x005B1212, 0x00 },
	{	0x005B1A51, 0x00 },
	{	0x005B1A6E, 0x04 },
	{	0x005B1A98, 0x08 },
	{	0x005BCC7A, 0x0C },
	{	0x005BCC9D, 0x00 },
	{	0x005BCCC0, 0x04 },
	{	0x005BCCE0, 0x0C },
	{	0x005BCCF6, 0x0C },
	{	0x005BCD16, 0x04 },
	{	0x005BCD28, 0x04 },
	{	0x005BCD44, 0x0C },
	{	0x005BCD54, 0x04 },
	{	0x005BCD64, 0x00 },
	{	0x005BCD83, 0x0C },
	{	0x005BCD93, 0x00 },
	{	0x005BCDB2, 0x04 },
	{	0x005BCDC4, 0x04 },
	{	0x005BCDE0, 0x04 },
	{	0x005BCDF0, 0x00 },
	{	0x005BCE0F, 0x00 },
	{	0x0068128B, 0x20 },
	{	0x006812A2, 0x10 },
	{	0x006812D2, 0x20 },
	{	0x0068166B, 0x20 },
	{	0x0068185E, 0x00 },
	{	0x00681C7F, 0x14 },
	{	0x00681E0D, 0x12 },
	{	0x00681E7F, 0x14 },
	{	0x00681ED2, 0x14 },
	{	0x00681F32, 0x14 },
	{	0x0087A469, 0x12 },
	{	0x0087A4A8, 0x14 },
	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	{	0x005AEAF9, 0x08 },
	{	0 },
};

// 127B / 027B
// 1280 / 0280
static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	{	0x00593AF9 + 1, 0 },
	{	0x005AEAF7 + 6, 0 },
	{	0x005B11FB + 3, 0 },
	{	0x005B1A40 + 3, (UInt32)(-0x1000) },
	{	0x005BCC4D + 6, (UInt32)(-0x1000) },

	{	0 },
};

#else
#error
#endif

#else

static const PatchLocation kPatch_ScriptCommands_Start[] =
{
	{	0x004072AF, 0x00 },
	{	0x004073FA, 0x00 },
	{	0x004A2374, 0x24 },
	{	0x004A23E8, 0x24 },
	{	0x004A2B9B, 0x00 },
	{	0x004A3CE2, 0x20 },
	{	0x004A3CF2, 0x10 },
	{	0x004A431A, 0x00 },
	{	0x004A474A, 0x20 },
	{	0x004A485F, 0x00 },
	{	0x004A4ED1, 0x00 },
	{	0x004A5134, 0x00 },
	{	0x004A58B4, 0x12 },
	{	0x004A58F5, 0x12 },
	{	0x004A5901, 0x14 },
	{	0x004A593E, 0x12 },
	{	0x004A5949, 0x14 },
	{	0x004A5A26, 0x12 },
	{	0x004A5A6D, 0x12 },
	{	0x004A5A79, 0x14 },
	{	0x004A5AD6, 0x12 },
	{	0x004A5B1D, 0x12 },
	{	0x004A5B29, 0x14 },
	{	0x004A5B7C, 0x12 },
	{	0x004A5BD9, 0x12 },
	{	0x004A5C28, 0x12 },
	{	0x004A5C34, 0x14 },
	{	0x004A600C, 0x14 },
	{	0x004A6704, 0x12 },
	{	0x004A6749, 0x12 },
	{	0x004A6755, 0x14 },
	{	0x004A684C, 0x12 },
	{	0x004A6A8F, 0x12 },
	{	0x004A6A9F, 0x14 },
	{	0x004A6BDF, 0x12 },
	{	0x004A6D30, 0x14 },
	{	0x004A6D74, 0x14 },
	{	0x004A703B, 0x12 },
	{	0x004A716D, 0x12 },
	{	0x004A71B5, 0x14 },
	{	0x004A7268, 0x14 },
	{	0x004A735A, 0x12 },
	{	0x004A7536, 0x14 },
	{	0x0059C532, 0x20 },
	{	0x0059C53B, 0x24 },
	{	0x0059C6BA, 0x24 },
	{	0x005C53F4, 0x04 },
	{	0x005C548D, 0x08 },
	{	0x005C6636, 0x00 },
	{	0x005C9499, 0x00 },
	{	0 },
};

static const PatchLocation kPatch_ScriptCommands_End[] =
{
	{	0x004A433B, 0x00 },
	{	0x0059C710, 0x24 },
	{	0x005C5372, 0x08 },
	{	0x005C541F, 0x04 },
	{	0 },
};

// 280 / 1280 / 27F
static const PatchLocation kPatch_ScriptCommands_MaxIdx[] =
{
	{	0x004A2B87 + 2,	(UInt32)(-0x1000) },
	{	0x0059C576 + 2,	(UInt32)(-0x1000) },
	{	0x005B1817 + 1,	0 },
	{	0x005C5370 + 6,	0 },

	{	0x004A439D + 2, (UInt32)(-0x1000) - 1 },
	{	0x004A43AD + 1, (UInt32)(-0x1000) - 1 },
	{	0x004A43B9 + 2, (UInt32)(-0x1000) - 1 },

	{	0x005C6625 + 1,	(UInt32)(-0x1000) - 1 },
	{	0x005C948B + 2,	(UInt32)(-0x1000) - 1 },

	{	0 },
};

#endif

static void ApplyPatch(const PatchLocation * patch, UInt32 newData)
{
	for(; patch->ptr; ++patch)
	{
		switch(patch->type)
		{
		case 0:
			SafeWrite32(patch->ptr, newData + patch->offset);
			break;

		case 1:
			SafeWrite16(patch->ptr, newData + patch->offset);
			break;
		}
	}
}

bool Cmd_Default_Execute(COMMAND_ARGS)
{
	return true;
}

bool Cmd_Default_Eval(COMMAND_ARGS_EVAL)
{
	return true;
}

bool Cmd_Default_Parse(UInt32 numParams, ParamInfo * paramInfo, ScriptLineBuffer * lineBuf, ScriptBuffer * scriptBuf)
{
	return g_defaultParseCommand(numParams, paramInfo, lineBuf, scriptBuf);
}

#if RUNTIME

bool Cmd_GetNVSEVersion_Eval(COMMAND_ARGS_EVAL)
{
	*result = NVSE_VERSION_INTEGER;
	if (IsConsoleMode()) {
		Console_Print("NVSE version: %d", NVSE_VERSION_INTEGER);
	}
	return true;
}

bool Cmd_GetNVSEVersion_Execute(COMMAND_ARGS)
{
	return Cmd_GetNVSEVersion_Eval(thisObj, 0, 0, result);
}

bool Cmd_GetNVSERevision_Eval(COMMAND_ARGS_EVAL)
{
	*result = NVSE_VERSION_INTEGER_MINOR;
	if (IsConsoleMode()) {
		Console_Print("NVSE revision: %d", NVSE_VERSION_INTEGER_MINOR);
	}
	return true;
}

bool Cmd_GetNVSERevision_Execute(COMMAND_ARGS)
{
	return Cmd_GetNVSERevision_Eval(thisObj, 0, 0, result);
}

bool Cmd_GetNVSEBeta_Eval(COMMAND_ARGS_EVAL)
{
	*result = NVSE_VERSION_INTEGER_BETA;
	if (IsConsoleMode()) {
		Console_Print("NVSE beta: %d", NVSE_VERSION_INTEGER_BETA);
	}
	return true;
}

bool Cmd_GetNVSEBeta_Execute(COMMAND_ARGS)
{
	return Cmd_GetNVSEBeta_Eval(thisObj, 0, 0, result);
}

bool Cmd_DumpDocs_Execute(COMMAND_ARGS)
{
	if (IsConsoleMode()) {
		Console_Print("Dumping Command Docs");
	}
	g_scriptCommands.DumpCommandDocumentation();
	if (IsConsoleMode()) {
		Console_Print("Done Dumping Command Docs");
	}
	return true;
}

bool Cmd_tcmd_Execute(COMMAND_ARGS)
{
	_MESSAGE("tcmd");

	Console_Print("hello world");

	Debug_DumpMenus();
	Debug_DumpTraits();

	*result = 0;

	return true;
}

bool Cmd_tcmd2_Execute(COMMAND_ARGS)
{
	UInt32	arg;

	_MESSAGE("tcmd2");

	if(ExtractArgs(EXTRACT_ARGS, &arg))
	{
		Console_Print("hello args: %d", arg);
	}
	else
	{
		Console_Print("hello args: failed");
	}

	*result = 0;

	return true;
}

class Dumper {
	UInt32 m_sizeToDump;
public:
	Dumper(UInt32 sizeToDump = 512) : m_sizeToDump(sizeToDump) {}
	bool Accept(void *addr) {
		if (addr) {
			DumpClass(addr, m_sizeToDump);
		}
		return true;
	}
};

bool Cmd_tcmd3_Execute(COMMAND_ARGS)
{
	TESForm* pForm = NULL;
	_MESSAGE("tcmd3");

	if (ExtractArgs(EXTRACT_ARGS, &pForm)) {
		// we have a pForm
	} 
		
	if (!pForm && thisObj && thisObj->baseForm) {
		pForm = thisObj->baseForm;
	}

//	DataHandler* pDH = DataHandler::Get();
//	int x = 4;

//	GameSettingCollection* pGC = GameSettingCollection::GetSingleton();
//	IniSettingCollection* pISC = IniSettingCollection::GetIniSettings();
//	IniSettingCollection* pIPC = IniSettingCollection::GetIniPrefs();

	InterfaceManager* pIM = InterfaceManager::GetSingleton();
//	DumpClass(pIM);

//	UInt32 formID = 0x105228;
//	TESForm* pLookedUp = LookupFormByID(formID);
//	if(pLookedUp) {
//		UInt32 addr = 0x011B9C2C;
//		DumpClass((void*)pForm);
//	}

//	TESObjectWEAP* pWeap = DYNAMIC_CAST(pForm, TESForm, TESObjectWEAP);

	PlayerCharacter* pPC = PlayerCharacter::GetSingleton();
	if (pPC) {
		BaseProcess* pBaseProc = pPC->baseProcess;
		BaseProcess::AmmoInfo* pAmmoInfo = pBaseProc->GetAmmoInfo();
		DumpClass(pAmmoInfo);
		DumpClass(pAmmoInfo->unk00);
//		ExtraAmmo* pAmmo = GetByTypeCast(pPC->extraDataList, Ammo);
		int x = 4;
	}



//	MagicTarget* pTarget = DYNAMIC_CAST(pPC, PlayerCharacter, MagicTarget);
//	if(pTarget) {
//		EffectNode* pEffects = pTarget->GetEffectList();
//		UInt32 cnt = pEffects->Count();
//		ActiveEffect* pEffect = pEffects->GetNthItem(1);
//		UInt32 formID = 0x5C6C1;
//		TESForm* pForm = LookupFormByID(formID);
//		EffectSetting* pSetting = DYNAMIC_CAST(pForm, TESForm, EffectSetting);
//
//		int c = 0;
//
//		//pEffects->Visit(Dumper(32));
//
//	}


	return true;
}

#endif

static ParamInfo kTestArgCommand_Params[] =
{
	{	"int", kParamType_Integer, 0 },
};

static ParamInfo kTestDumpCommand_Params[] = 
{
	{	"form", kParamType_ObjectID, 1},
};

DEFINE_CMD_COND(GetNVSEVersion, returns the installed version of NVSE, 0, NULL);
DEFINE_CMD_COND(GetNVSERevision, returns the numbered revision of the installed version of NVSE, 0, NULL);
DEFINE_CMD_COND(GetNVSEBeta, returns the numbered beta of the installed version of NVSE, 0, NULL);
DEFINE_COMMAND(DumpDocs, , 0, 0, NULL);
DEFINE_COMMAND(tcmd, test, 0, 0, NULL);
DEFINE_CMD_ALT(tcmd2, testargcmd ,test, 0, 1, kTestArgCommand_Params);
DEFINE_CMD_ALT(tcmd3, testdump, dump info, 0, 1, kTestDumpCommand_Params);

#define ADD_CMD(command) Add(&kCommandInfo_ ## command )

CommandTable::CommandTable() { }
CommandTable::~CommandTable() { }

void CommandTable::Init(void)
{
	// read in the console commands
	g_consoleCommands.SetBaseID(0x0100);
	g_consoleCommands.Read((CommandInfo *)g_offsetConsoleCommandsStart, (CommandInfo *)g_offsetConsoleCommandsLast);

	// read in the script commands
	g_scriptCommands.SetBaseID(0x1000);
	g_scriptCommands.Read((CommandInfo *)g_offsetScriptCommandsStart, (CommandInfo *)g_offsetScriptCommandsLast);

	// pad to opcode 0x1400 to give Bethesda lots of room
	g_scriptCommands.PadTo(kNVSEOpcodeStart);

	// Add NVSE Commands
	g_scriptCommands.AddCommandsV1();

#if _DEBUG
	g_scriptCommands.AddDebugCommands();
#endif

	// register plugins
	g_pluginManager.Init();

	ApplyPatch(kPatch_ScriptCommands_Start, (UInt32)g_scriptCommands.GetStart());
	ApplyPatch(kPatch_ScriptCommands_End, (UInt32)g_scriptCommands.GetEnd());
	ApplyPatch(kPatch_ScriptCommands_MaxIdx, g_scriptCommands.GetMaxID());

	_MESSAGE("max id = %08X", g_scriptCommands.GetMaxID());

	g_scriptCommands.Dump();

	_MESSAGE("patched");
}

void CommandTable::Read(CommandInfo * start, CommandInfo * end)
{
	UInt32	numCommands = end - start;
	m_commands.reserve(m_commands.size() + numCommands);

	for(; start != end; ++start)
		Add(start);
}

void CommandTable::Add(CommandInfo * info, CommandReturnType retnType, UInt32 parentPluginOpcodeBase)
{
	UInt32	backCommandID = m_baseID + m_commands.size();	// opcode of the next command to add

	info->opcode = m_curID;

	if(m_curID == backCommandID)
	{
		// adding at the end?
		m_commands.push_back(*info);
	}
	else if(m_curID < backCommandID)
	{
		// adding to existing data?
		ASSERT(m_curID >= m_baseID);

		m_commands[m_curID - m_baseID] = *info;
	}
	else
	{
		HALT("CommandTable::Add: adding past the end");
	}

	m_curID++;

	CommandMetadata * metadata = &m_metadata[info->opcode];

	metadata->parentPlugin = parentPluginOpcodeBase;
	metadata->returnType = retnType;
}

static CommandInfo kPaddingCommand =
{
	"", "",
	0,
	"command used for padding",
	0,
	0,
	NULL,

	Cmd_Default_Execute,
	Cmd_Default_Parse,
	NULL,
	NULL
};

void CommandTable::PadTo(UInt32 id, CommandInfo * info)
{
	if(!info) info = &kPaddingCommand;

	while(m_baseID + m_commands.size() < id)
	{
		info->opcode = m_baseID + m_commands.size();
		m_commands.push_back(*info);
	}

	m_curID = id;
}

void CommandTable::Dump(void)
{
	for(CommandList::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter)
	{
		_MESSAGE("%08X %04X %s %s", iter->opcode, iter->needsParent, iter->longName, iter->shortName);
		gLog.Indent();

#if 0
		for(UInt32 i = 0; i < iter->numParams; i++)
		{
			ParamInfo	* param = &iter->params[i];
			_MESSAGE("%08X %08X %s", param->typeID, param->isOptional, param->typeStr);
		}
#endif

		gLog.Outdent();
	}
}


void CommandTable::DumpAlternateCommandNames(void)
{
	for (CommandList::iterator iter= m_commands.begin(); iter != m_commands.end(); ++iter)
	{
		if (iter->shortName)
			_MESSAGE("%s", iter->shortName);
	}
}

const char* StringForParamType(UInt32 paramType)
{
	switch(paramType) {
		case kParamType_String:				return "String";
		case kParamType_Integer:			return "Integer";
		case kParamType_Float:				return "Float";
		case kParamType_ObjectID:			return "ObjectID";
		case kParamType_ObjectRef:			return "ObjectRef";
		case kParamType_ActorValue:			return "ActorValue";
		case kParamType_Actor:				return "Actor";
		case kParamType_SpellItem:			return "SpellItem";
		case kParamType_Axis:				return "Axis";
		case kParamType_Cell:				return "Cell";
		case kParamType_AnimationGroup:		return "AnimationGroup";
		case kParamType_MagicItem:			return "MagicItem";
		case kParamType_Sound:				return "Sound";
		case kParamType_Topic:				return "Topic";
		case kParamType_Quest:				return "Quest";
		case kParamType_Race:				return "Race";
		case kParamType_Class:				return "Class";
		case kParamType_Faction:			return "Faction";
		case kParamType_Sex:				return "Sex";
		case kParamType_Global:				return "Global";
		case kParamType_Furniture:			return "Furniture";
		case kParamType_TESObject:			return "Object";
		case kParamType_VariableName:		return "VariableName";
		case kParamType_QuestStage:			return "QuestStage";
		case kParamType_MapMarker:			return "MapMarker";
		case kParamType_ActorBase:			return "ActorBase";
		case kParamType_Container:			return "Container";
		case kParamType_WorldSpace:			return "WorldSpace";
		case kParamType_CrimeType:			return "CrimeType";
		case kParamType_AIPackage:			return "AIPackage";
		case kParamType_CombatStyle:		return "CombatStyle";
		case kParamType_MagicEffect:		return "MagicEffect";
		case kParamType_FormType:			return "FormType";
		case kParamType_WeatherID:			return "WeatherID";
		case kParamType_NPC:				return "NPC";
		case kParamType_Owner:				return "Owner";
		case kParamType_EffectShader:		return "EffectShader";
		case kParamType_FormList:			return "FormList";
		case kParamType_MenuIcon:			return "MenuIcon";
		case kParamType_Perk:				return "Perk";
		case kParamType_Note:				return "Note";
		case kParamType_MiscellaneousStat:	return "MiscStat";
		case kParamType_ImageSpaceModifier:	return "ImageSpaceModifier";
		case kParamType_ImageSpace:			return "ImageSpace";
		case kParamType_Unhandled2C:		return "unk2C";
		case kParamType_Unhandled2D:		return "unk2D";
		case kParamType_Unhandled2E:		return "unk2E";
		case kParamType_EncounterZone:		return "EncounterZone";
		case kParamType_Unhandled30:		return "unk30";
		case kParamType_Message:			return "Message";
		case kParamType_InvObjOrFormList:	return "InvObjectOrFormList";
		case kParamType_Alignment:			return "Alignment";
		case kParamType_EquipType:			return "EquipType";
		case kParamType_NonFormList:		return "NonFormList";
		case kParamType_SoundFile:			return "SoundFile";
		case kParamType_CriticalStage:		return "CriticalStage";
		case kParamType_LeveledOrBaseChar:	return "LeveledOrBaseChar";
		case kParamType_LeveledOrBaseCreature:	return "LeveledOrBaseCreature";
		case kParamType_LeveledChar:		return "LeveledChar";
		case kParamType_LeveledCreature:	return "LeveledCreature";
		case kParamType_LeveledItem:		return "LeveledItem";
		case kParamType_AnyForm:			return "AnyForm";
		case kParamType_Reputation:			return "Reputation";
		case kParamType_Casino:				return "Casino";
		case kParamType_CasinoChip:			return "CasinoChip";
		case kParamType_Challenge:			return "Challenge";
		case kParamType_CaravanMoney:		return "CaravanMoney";
		case kParamType_CaravanCard:		return "CaravanCard";
		case kParamType_CaravanDeck:		return "CaravanDeck";
		case kParamType_Region:				return "Region";
		default: return "<unknown>";
	}
}

void CommandTable::DumpCommandDocumentation(UInt32 startWithID)
{
	_MESSAGE("NVSE Commands from: %#x", startWithID);

	_MESSAGE("<br><b>Function Quick Reference</b>");
	CommandList::iterator itEnd = m_commands.end();
	for (CommandList::iterator iter = m_commands.begin();iter != itEnd; ++iter) {
		if (iter->opcode >= startWithID) {
			iter->DumpFunctionDef();
		}
	}

	_MESSAGE("<hr><br><b>Functions In Detail</b>");
	for (CommandList::iterator iter = m_commands.begin();iter != itEnd; ++iter) {
		if (iter->opcode >= startWithID) {
			iter->DumpDocs();
		}
	}
}

void CommandInfo::DumpDocs() const
{
	_MESSAGE("<p><a name=\"%s\"></a><b>%s</b> ", longName, longName);
	_MESSAGE("<br><b>Alias:</b> %s<br><b>Parameters:</b>%d", (strlen(shortName) != 0) ? shortName : "none", numParams);
	if (numParams > 0) {
		for(UInt32 i = 0; i < numParams; i++)
		{
			ParamInfo	* param = &params[i];
			const char* paramTypeName = StringForParamType(param->typeID);
			if (param->isOptional != 0) {
				_MESSAGE("<br>&nbsp;&nbsp;&nbsp;<i>%s:%s</i> ", param->typeStr, paramTypeName);
			} else {
				_MESSAGE("<br>&nbsp;&nbsp;&nbsp;%s:%s ", param->typeStr, paramTypeName);
			}
		}
	}
	_MESSAGE("<br><b>Return Type:</b> FixMe<br><b>Opcode:</b> %#4x (%d)<br><b>Condition Function:</b> %s<br><b>Description:</b> %s</p>", opcode, opcode, eval ? "Yes" : "No",helpText);
}

void CommandInfo::DumpFunctionDef() const
{
	_MESSAGE("<br>(FixMe) %s<a href=\"#%s\">%s</a> ", needsParent > 0 ? "reference." : "", longName, longName);
	if (numParams > 0) {
		for(UInt32 i = 0; i < numParams; i++)
		{
			ParamInfo	* param = &params[i];
			const char* paramTypeName = StringForParamType(param->typeID);
			if (param->isOptional != 0) {
				_MESSAGE("<i>%s:%s</i> ", param->typeStr, paramTypeName);
			} else {
				_MESSAGE("%s:%s ", param->typeStr, paramTypeName);
			}
		}
	}
}


CommandInfo * CommandTable::GetByName(const char * name)
{
	for(CommandList::iterator iter = m_commands.begin(); iter != m_commands.end(); ++iter)
		if(!_stricmp(name, iter->longName) || (iter->shortName && !_stricmp(name, iter->shortName)))
			return &(*iter);

	return NULL;
}


void ImportConsoleCommand(const char * name)
{
	CommandInfo	* info = g_consoleCommands.GetByName(name);
	if(info)
	{
		CommandInfo	infoCopy = *info;

		std::string	newName;

		newName = std::string("con_") + name;

		infoCopy.shortName = "";
		infoCopy.longName = _strdup(newName.c_str());

		g_scriptCommands.Add(&infoCopy);

//		_MESSAGE("imported console command %s", name);
	}
	else
	{
		_WARNING("couldn't find console command (%s)", name);

		// pad it
		g_scriptCommands.Add(&kPaddingCommand);
	}
}

// internal commands added at the end
void CommandTable::AddDebugCommands()
{
	ADD_CMD(tcmd);
	ADD_CMD(tcmd2);
	ADD_CMD(tcmd3);
	ADD_CMD(DumpDocs);
}

void CommandTable::AddCommandsV1()
{
	// beta 1
	ADD_CMD(GetNVSEVersion);
	ADD_CMD(GetNVSERevision);
	ADD_CMD(GetNVSEBeta);
	ADD_CMD(GetBaseObject);
	ADD_CMD(GetWeight);
	ADD_CMD(GetHealth);
	ADD_CMD(GetValue);
	ADD_CMD(SetWeight);
	ADD_CMD(SetHealth);
	ADD_CMD(SetBaseItemValue);
	ADD_CMD(GetType);
	ADD_CMD(GetRepairList);
	ADD_CMD(GetEquipType);
	ADD_CMD(GetWeaponAmmo);
	ADD_CMD(GetWeaponClipRounds);
	ADD_CMD(GetAttackDamage);
	ADD_CMD(GetWeaponType);
	ADD_CMD(GetWeaponMinSpread);
	ADD_CMD(GetWeaponSpread);
	ADD_CMD(GetWeaponProjectile);
	ADD_CMD(GetWeaponSightFOV);
	ADD_CMD(GetWeaponMinRange);
	ADD_CMD(GetWeaponMaxRange);
	ADD_CMD(GetWeaponAmmoUse);
	ADD_CMD(GetWeaponActionPoints);
	ADD_CMD(GetWeaponCritDamage);
	ADD_CMD(GetWeaponCritChance);
	ADD_CMD(GetWeaponCritEffect);
	ADD_CMD(GetWeaponFireRate);
	ADD_CMD(GetWeaponAnimAttackMult);
	ADD_CMD(GetWeaponRumbleLeftMotor);
	ADD_CMD(GetWeaponRumbleRightMotor);
	ADD_CMD(GetWeaponRumbleDuration);
	ADD_CMD(GetWeaponRumbleWavelength);
	ADD_CMD(GetWeaponAnimShotsPerSec);
	ADD_CMD(GetWeaponAnimReloadTime);
	ADD_CMD(GetWeaponAnimJamTime);
	ADD_CMD(GetWeaponSkill);
	ADD_CMD(GetWeaponResistType);
	ADD_CMD(GetWeaponFireDelayMin);
	ADD_CMD(GetWeaponFireDelayMax);
	ADD_CMD(GetWeaponAnimMult);
	ADD_CMD(GetWeaponReach);
	ADD_CMD(GetWeaponIsAutomatic);
	ADD_CMD(GetWeaponHandGrip);
	ADD_CMD(GetWeaponReloadAnim);
	ADD_CMD(GetWeaponBaseVATSChance);
	ADD_CMD(GetWeaponAttackAnimation);
	ADD_CMD(GetWeaponNumProjectiles);
	ADD_CMD(GetWeaponAimArc);
	ADD_CMD(GetWeaponLimbDamageMult);
	ADD_CMD(GetWeaponSightUsage);
	ADD_CMD(GetWeaponHasScope);
	ImportConsoleCommand("SetGameSetting");
	ImportConsoleCommand("SetINISetting");
	ImportConsoleCommand("GetINISetting");
	ImportConsoleCommand("RefreshINI");
	ImportConsoleCommand("Save");
	ImportConsoleCommand("SaveINI");
	ImportConsoleCommand("QuitGame");
	ImportConsoleCommand("LoadGame");
	ImportConsoleCommand("CloseAllMenus");
	ImportConsoleCommand("SetVel");
	ADD_CMD(ListGetCount);
	ADD_CMD(ListGetNthForm);
	ADD_CMD(ListGetFormIndex);
	ADD_CMD(ListAddForm);
	ADD_CMD(ListAddReference);
	ADD_CMD(ListRemoveNthForm);
	ADD_CMD(ListRemoveForm);
	ADD_CMD(ListReplaceNthForm);
	ADD_CMD(ListReplaceForm);
	ADD_CMD(ListClear);
	ADD_CMD(GetEquippedObject);
	ADD_CMD(GetEquippedCurrentHealth);
	ADD_CMD(CompareNames);
	ADD_CMD(SetName);
	ADD_CMD(GetHotkeyItem);
	ADD_CMD(GetNumItems);
	ADD_CMD(GetInventoryObject);
	ADD_CMD(SetEquippedCurrentHealth);
	ADD_CMD(GetCurrentHealth);
	ADD_CMD(SetCurrentHealth);
	ADD_CMD(IsKeyPressed);
	ADD_CMD(TapKey);
	ADD_CMD(HoldKey);
	ADD_CMD(ReleaseKey);
	ADD_CMD(DisableKey);
	ADD_CMD(EnableKey);
	ADD_CMD(GetNumKeysPressed);
	ADD_CMD(GetKeyPress);
	ADD_CMD(GetNumMouseButtonsPressed);
	ADD_CMD(GetMouseButtonPress);
	ADD_CMD(GetControl);
	ADD_CMD(GetAltControl);
	ADD_CMD(MenuTapKey);
	ADD_CMD(MenuHoldKey);
	ADD_CMD(MenuReleaseKey);
	ADD_CMD(DisableControl);
	ADD_CMD(EnableControl);
	ADD_CMD(TapControl);
	ADD_CMD(SetControl);
	ADD_CMD(SetAltControl);
	ADD_CMD(SetIsControl);
	ADD_CMD(IsControl);
	ADD_CMD(IsKeyDisabled);
	ADD_CMD(IsControlDisabled);
	ADD_CMD(IsControlPressed);
	ADD_CMD(IsPersistent);
	ADD_CMD(GetParentCell);
	ADD_CMD(GetParentWorldspace);
	ADD_CMD(GetTeleportCell);
	ADD_CMD(GetLinkedDoor);
	ADD_CMD(GetFirstRef);
	ADD_CMD(GetNextRef);
	ADD_CMD(GetNumRefs);
	ADD_CMD(GetFirstRefInCell);
	ADD_CMD(GetNumRefsInCell);
	ADD_CMD(GetRefCount);
	ADD_CMD(SetRefCount);
	ADD_CMD(GetArmorAR);
	ADD_CMD(IsPowerArmor);
	ADD_CMD(SetIsPowerArmor);
	ADD_CMD(SetRepairList);
	ADD_CMD(IsQuestItem);
	ADD_CMD(SetQuestItem);
	ADD_CMD(GetObjectEffect);
	ADD_CMD(SetWeaponAmmo);
	ADD_CMD(SetWeaponClipRounds);
	ADD_CMD(SetAttackDamage);
	ADD_CMD(SetWeaponType);
	ADD_CMD(SetWeaponMinSpread);
	ADD_CMD(SetWeaponSpread);
	ADD_CMD(SetWeaponProjectile);
	ADD_CMD(SetWeaponSightFOV);
	ADD_CMD(SetWeaponMinRange);
	ADD_CMD(SetWeaponMaxRange);
	ADD_CMD(SetWeaponAmmoUse);
	ADD_CMD(SetWeaponActionPoints);
	ADD_CMD(SetWeaponCritDamage);
	ADD_CMD(SetWeaponCritChance);
	ADD_CMD(SetWeaponCritEffect);
	ADD_CMD(SetWeaponAnimAttackMult);
	ADD_CMD(SetWeaponAnimMult);
	ADD_CMD(SetWeaponReach);
	ADD_CMD(SetWeaponIsAutomatic);
	ADD_CMD(SetWeaponHandGrip);
	ADD_CMD(SetWeaponReloadAnim);
	ADD_CMD(SetWeaponBaseVATSChance);
	ADD_CMD(SetWeaponAttackAnimation);
	ADD_CMD(SetWeaponNumProjectiles);
	ADD_CMD(SetWeaponAimArc);
	ADD_CMD(SetWeaponLimbDamageMult);
	ADD_CMD(SetWeaponSightUsage);
	ADD_CMD(GetNumericGameSetting);
	ADD_CMD(SetNumericGameSetting);
	ADD_CMD(GetNumericIniSetting);
	ADD_CMD(SetNumericIniSetting);
	ADD_CMD(Label);
	ADD_CMD(Goto);
	ADD_CMD(PrintToConsole);
	ADD_CMD(DebugPrint);
	ADD_CMD(SetDebugMode);
	ADD_CMD(GetDebugMode);

	// beta 2
	ADD_CMD(GetUIFloat);
	ADD_CMD(SetUIFloat);
	ADD_CMD(SetUIString);
	ADD_CMD(GetCrosshairRef);
	ADD_CMD(GetGameRestarted);
	ImportConsoleCommand("ToggleMenus");
	ImportConsoleCommand("TFC");	// changed from ToggleFreeCamera
	ImportConsoleCommand("TCL");	// changed from ToggleCollision
	ADD_CMD(GetGameLoaded);
	ADD_CMD(GetWeaponItemMod);
	ADD_CMD(IsModLoaded);
	ADD_CMD(GetModIndex);
	ADD_CMD(GetNumLoadedMods);
	ADD_CMD(GetSourceModIndex);
	ADD_CMD(GetDebugSelection);
	ADD_CMD(GetArmorDT);
	ADD_CMD(SetArmorAR);
	ADD_CMD(SetArmorDT);

	// beta 3
	ADD_CMD(IsScripted);
	ADD_CMD(GetScript);
	ADD_CMD(RemoveScript);
	ADD_CMD(SetScript);
	ADD_CMD(IsFormValid);
	ADD_CMD(IsReference);

	// beta 4 - compat with 1.1.1.280
	// oh, sorry, compat with 1.1.1.285
	// oh, I am bad at reading, compat with 1.2.0.285

	// beta 5
	ADD_CMD(GetWeaponRequiredStrength);
	ADD_CMD(GetWeaponRequiredSkill);
	ADD_CMD(SetWeaponRequiredStrength);
	ADD_CMD(SetWeaponRequiredSkill);
	ADD_CMD(SetWeaponResistType);
	ADD_CMD(SetWeaponSkill);
	ADD_CMD(GetAmmoSpeed);
	ADD_CMD(GetAmmoConsumedPercent);
	ADD_CMD(GetAmmoCasing);
	ADD_CMD(GetPlayerCurrentAmmoRounds);
	ADD_CMD(SetPlayerCurrentAmmoRounds);
	ADD_CMD(GetPlayerCurrentAmmo);

	// beta 6 - compat with 1.2.0.314
	ADD_CMD(GetOpenKey);
	ADD_CMD(Exp);
	ADD_CMD(Log10);
	ADD_CMD(Floor);
	ADD_CMD(Ceil);
	ADD_CMD(LeftShift);
	ADD_CMD(RightShift);
	ADD_CMD(LogicalAnd);
	ADD_CMD(LogicalOr);
	ADD_CMD(LogicalXor);
	ADD_CMD(LogicalNot);
	ADD_CMD(Pow);
	ADD_CMD(Fmod);
	ADD_CMD(Rand);

	// beta 7 - compat with 1.2.0.352
	
	// beta 8 - rewrite loader to work around steam bugs
	
	// beta 9 - compat with 1.3.0.452
	ADD_CMD(SortUIListBox);
	ADD_CMD(GetOwner);

	// beta 10 - compat with editor 1.3.0.452

	// 2 beta 1
	ADD_CMD(GetLocalRefIndex);
	ADD_CMD(BuildRef);
	ADD_CMD(SetNameEx);
	ADD_CMD(MessageEx);
	ADD_CMD(MessageBoxEx);
	ADD_CMD(TempCloneForm);
	ADD_CMD(IsClonedForm);
	ADD_CMD(GetParentCellOwner);
	ADD_CMD(GetOwningFactionRequiredRank);
	ADD_CMD(GetParentCellOwningFactionRequiredRank);
	
	// 2 beta 2
	ADD_CMD(SetUIStringEx);

	// 2 beta 3
	// 2 beta 4

	// 2 beta 5
	ImportConsoleCommand("SetUFOCamSpeedMult");
	ImportConsoleCommand("TDT");
	ADD_CMD(SetWeaponFireRate);
	ADD_CMD(GetWeaponLongBursts);
	ADD_CMD(SetWeaponLongBursts);

	// 2 beta 6 - compat with 1.4.0.525
	ADD_CMD(GetWeaponFlags1);
	ADD_CMD(GetWeaponFlags2);
	ADD_CMD(SetWeaponFlags1);
	ADD_CMD(SetWeaponFlags2);
	ADD_CMD(GetActorBaseFlagsLow);
	ADD_CMD(SetActorBaseFlagsLow);
	ADD_CMD(GetActorBaseFlagsHigh);
	ADD_CMD(SetActorBaseFlagsHigh);
	ADD_CMD(ClearBit);
	ADD_CMD(SetBit);

	// 2 beta 7 - quick fix for InterfaceManager ptr

	// 2 beta 8 - compat with editor 1.4.0.518

	// 2 beta 9 - quick fix for IsControlDisabled

	// 2 beta 10 - compat with nogore runtime 1.4.0.525
	ADD_CMD(GetEquippedWeaponModFlags);
	ADD_CMD(SetEquippedWeaponModFlags);
	ADD_CMD(GetWeaponItemModEffect);
	ADD_CMD(GetWeaponItemModValue1);
	ADD_CMD(GetWeaponItemModValue2);

	// 2 beta 11 - fixed TESObjectWEAP struct (mod value 2 off by one)
	// 2 beta 12 - fixed GetWeaponItemModEffect etc. using 0-based indexing
}
