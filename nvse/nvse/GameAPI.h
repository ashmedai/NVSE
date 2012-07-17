#pragma once

#include "GameTypes.h"

struct ParamInfo;
class TESObjectREFR;
class Script;
struct ScriptEventList;
class TESForm;

void Console_Print(const char * fmt, ...);

typedef bool (* _ExtractArgs)(ParamInfo * paramInfo, void * scriptData, UInt32 * arg2, TESObjectREFR * arg3, TESObjectREFR * arg4, Script * script, ScriptEventList * eventList, ...);
extern const _ExtractArgs ExtractArgs;

typedef void * (* _FormHeap_Allocate)(UInt32 size);
extern const _FormHeap_Allocate FormHeap_Allocate;

typedef void (* _FormHeap_Free)(void * ptr);
extern const _FormHeap_Free FormHeap_Free;

typedef TESForm * (* _LookupFormByID)(UInt32 id);
extern const _LookupFormByID LookupFormByID;

typedef TESForm * (* _CreateFormInstance)(UInt8 type);
extern const _CreateFormInstance CreateFormInstance;

bool IsConsoleMode();
const char * GetFullName(TESForm * baseForm);
const char* GetActorValueString(UInt32 actorValue); // not working yet

typedef void (* _ShowMessageBox_Callback)(void);
extern const _ShowMessageBox_Callback ShowMessageBox_Callback;

// unk1 = 0
// unk2 = 0
// callback = may be NULL apparently
// unk4 = 0
// unk5 = 0x17 (why?)
// unk6 = 0
// unk7 = 0
// then buttons
// then NULL
typedef bool (* _ShowMessageBox)(const char * message, UInt32 unk1, UInt32 unk2, _ShowMessageBox_Callback callback, UInt32 unk4, UInt32 unk5, float unk6, float unk7, ...);
extern const _ShowMessageBox ShowMessageBox;

// set to scriptObj->refID after calling ShowMessageBox()
// GetButtonPressed checks this before returning a value, if it doesn't match it returns -1
typedef UInt32 * _ShowMessageBox_pScriptRefID;
extern const _ShowMessageBox_pScriptRefID ShowMessageBox_pScriptRefID;
typedef UInt8 * _ShowMessageBox_button;
extern const _ShowMessageBox_button ShowMessageBox_button;

// unk1 = 0
// unk3 = 0, "UIVATSInsufficientAP" (sound?)
// duration = 2
// unk5 = 0
typedef bool (* _QueueUIMessage)(const char * string, UInt32 unk1, const char * icon, const char * unk3, float duration, UInt8 unk5);
extern const _QueueUIMessage QueueUIMessage;

const UInt32 kMaxMessageLength = 0x4000;

bool ExtractArgsEx(ParamInfo * paramInfo, void * scriptData, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, ...);

// only records individual objects if there's a block that matches it
// ### how can it tell?
struct ScriptEventList
{
	struct Event
	{
		TESForm	* object;
		UInt32	eventMask;
	};

	struct VarEntry;

	struct Var
	{
		UInt32		id;
		VarEntry	* nextEntry;
		double		data;
	};

	struct VarEntry
	{
		Var			* var;
		VarEntry	* next;
	};

	Script			* m_script;
	UInt32			m_unk1;
	tList<Event>	* m_eventList;
	VarEntry		* m_vars;

	void	Dump(void);
	Var *	GetVariable(UInt32 id);
	UInt32	ResetAllVariables();
};

struct ExtractedParam
{
	// float/double types are kept as pointers
	// this avoids problems with storing invalid floats/doubles in to the fp registers which has a side effect
	// of corrupting data

	enum
	{
		kType_Unknown = 0,
		kType_String,		// str
		kType_Imm32,		// imm
		kType_Imm16,		// imm
		kType_Imm8,			// imm
		kType_ImmDouble,	// immDouble
		kType_Form,			// form
	};

	UInt8	type;
	bool	isVar;	// if true, data is stored in var, otherwise it's immediate

	union
	{
		// immediate
		UInt32			imm;
		const double	* immDouble;
		TESForm			* form;
		struct
		{
			const char	* buf;
			UInt32		len;
		} str;

		// variable
		struct 
		{
			ScriptEventList::Var	* var;
			ScriptEventList			* parent;
		} var;
	} data;
};

bool ExtractArgsRaw(ParamInfo * paramInfo, void * scriptDataIn, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, ...);

enum EActorVals {
	eActorVal_Aggression			= 0,
	eActorVal_Confidence			= 1,
	eActorVal_Energy				= 2,
	eActorVal_Responsibility		= 3,
	eActorVal_Mood					= 4,

	eActorVal_Strength				= 5,
	eActorVal_Perception			= 6,
	eActorVal_Endurance				= 7,
	eActorVal_Charisma				= 8,
	eActorVal_Intelligence			= 9,
	eActorVal_Agility				= 10,
	eActorVal_Luck					= 11,
	eActorVal_SpecialStart = eActorVal_Strength,
	eActorVal_SpecialEnd = eActorVal_Luck,

	eActorVal_ActionPoints			= 12,
	eActorVal_CarryWeight			= 13,
	eActorVal_CritChance			= 14,
	eActorVal_HealRate				= 15,
	eActorVal_Health				= 16,
	eActorVal_MeleeDamage			= 17,
	eActorVal_DamageResistance		= 18,
	eActorVal_PoisonResistance		= 19,
	eActorVal_RadResistance			= 20,
	eActorVal_SpeedMultiplier		= 21,
	eActorVal_Fatigue				= 22,
	eActorVal_Karma					= 23,
	eActorVal_XP					= 24,

	eActorVal_Head					= 25,
	eActorVal_Torso					= 26,
	eActorVal_LeftArm				= 27,
	eActorVal_RightArm				= 28,
	eActorVal_LeftLeg				= 29,
	eActorVal_RightLeg				= 30,
	eActorVal_Brain					= 31,
	eActorVal_BodyPartStart = eActorVal_Head,
	eActorVal_BodyPartEnd = eActorVal_Brain,

	eActorVal_Barter				= 32,
	eActorVal_BigGuns				= 33,
	eActorVal_EnergyWeapons			= 34,
	eActorVal_Explosives			= 35,
	eActorVal_Lockpick				= 36,
	eActorVal_Medicine				= 37,
	eActorVal_MeleeWeapons			= 38,
	eActorVal_Repair				= 39,
	eActorVal_Science				= 40,
	eActorVal_SmallGuns				= 41,
	eActorVal_Sneak					= 42,
	eActorVal_Speech				= 43,
	eActorVal_Throwing				= 44,
	eActorVal_Unarmed				= 45,
	eActorVal_SkillsStart = eActorVal_Barter,
	eActorVal_SkillsEnd = eActorVal_Unarmed,

	eActorVal_InventoryWeight		= 46,
	eActorVal_Paralysis				= 47,
	eActorVal_Invisibility			= 48,
	eActorVal_Chameleon				= 49,
	eActorVal_NightEye				= 50,
	eActorVal_DetectLifeRange		= 51,
	eActorVal_FireResistance		= 52,
	eActorVal_WaterBreathing		= 53,
	eActorVal_RadLevel				= 54,
	eActorVal_BloodyMess			= 55,
	eActorVal_UnarmedDamage			= 56,
	eActorVal_Assistance			= 57,

	eActorVal_ElectricResistance	= 58,

	eActorVal_EnergyResistance		= 60,
	eActorVal_EMPResistance			= 61,
	eActorVal_Var1Medical			= 62,

	eActorVal_IgnoreCrippledLimbs	= 72,
	eActorVal_FalloutMax = eActorVal_IgnoreCrippledLimbs,
};

class ConsoleManager
{
public:
	MEMBER_FN_PREFIX(ConsoleManager);
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	DEFINE_MEMBER_FN(Print, void, 0x0071D0A0, const char * fmt, va_list args);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	DEFINE_MEMBER_FN(Print, void, 0x0071D070, const char * fmt, va_list args);
#elif EDITOR
#else
#error
#endif

	ConsoleManager();
	~ConsoleManager();

	static ConsoleManager * GetSingleton(void);

	void	* scriptContext;	// 00
};

extern bool ExtractFormatStringArgs(UInt32 fmtStringPos, char* buffer, ParamInfo * paramInfo, void * scriptDataIn, UInt32 * scriptDataOffset, Script * scriptObj, ScriptEventList * eventList, UInt32 maxParams, ...);


// A plugin author requested the ability to use OBSE format specifiers to format strings with the args
// coming from a source other than script.
// So changed ExtractFormattedString to take an object derived from following class, containing the args
// Probably doesn't belong in GameAPI.h but utilizes a bunch of stuff defined here and can't think of a better place for it
class FormatStringArgs
{
public:
	enum argType {
		kArgType_Float,
		kArgType_Form		// TESForm*
	};

	virtual bool Arg(argType asType, void * outResult) = 0;	// retrieve next arg
	virtual bool SkipArgs(UInt32 numToSkip) = 0;			// skip specified # of args
	virtual bool HasMoreArgs() = 0;
	virtual std::string GetFormatString() = 0;						// return format string
};

// concrete class used for extracting script args
class ScriptFormatStringArgs : public FormatStringArgs
{
public:
	virtual bool Arg(argType asType, void* outResult);
	virtual bool SkipArgs(UInt32 numToSkip);
	virtual bool HasMoreArgs();
	virtual std::string GetFormatString();

	ScriptFormatStringArgs(UInt32 _numArgs, UInt8* _scriptData, Script* _scriptObj, ScriptEventList* _eventList);
	UInt32 GetNumArgs();
	UInt8* GetScriptData();

private:
	UInt32			numArgs;
	UInt8			* scriptData;
	Script			* scriptObj;
	ScriptEventList	* eventList;
	std::string fmtString;
};

class ChangesMap;
class InteriorCellNewReferencesMap;
class ExteriorCellNewReferencesMap;
class NumericIDBufferMap;

// 1C8 - only explicitly marked things are verified
class TESSaveLoadGame
{
public:
	TESSaveLoadGame();
	~TESSaveLoadGame();

	struct CreatedObject {
		UInt32			refID;
		CreatedObject	* next;
	};

	ChangesMap					* changesMap;		// 000
	UInt32						unk004;				// 004
	InteriorCellNewReferencesMap	* intRefMap;	// 008
	ExteriorCellNewReferencesMap	* extRefMap00C;	// 00C
	ExteriorCellNewReferencesMap	* extRefMap010;	// 010
	void						* saveLoadBuffer;	// 014
	UInt32						unk018;				// 018
	UInt8						unk01C;				// 01C
	UInt8						pad01D[3];
	UInt32						unk020;				// 020
	UInt32						unk024;				// 024
	UInt32						unk028;				// 028
	CreatedObject				createdObjectList;	// 02C data is formID - verified
	UInt32				unk034[(0x58-0x34) >> 2];	// 034
	NumericIDBufferMap			* idMap058;			// 058
	NumericIDBufferMap			* idMap05C;			// 05C
	NumericIDBufferMap			* idMap060;			// 060
	NumericIDBufferMap			* idMap064;			// 064
	UInt32						unk068;				// 068
	UInt32						unk06C;				// 06C
	UInt32						unk070;				// 070
	UInt8						unk074;				// 074
	UInt8						unk075;				//     init to 0x7D
	UInt8						pad076[2];
	NiTArray<UInt32>			* array078;			// 078 NiTLargePrimitiveArray<?>
	NiTArray<UInt32>			* array07C;			// 07C NiTLargePrimitiveArray<?>	
	UInt8						unk080;				// 080 version of save?
	UInt8						unk081;
	UInt8						pad082[2];
	UInt32				unk084[(0xAC-0x84) >> 2];	// 084
	UInt8						unk0AC;				// 0AC
	UInt8						unk0AD;				// 0AD
	UInt8						unk0AE;				// 0AE
	UInt8						unk0AF;				// 0AF
	UInt32				unk0B0[(0x1C8-0x0B0) >> 2];	// 0B0

	static TESSaveLoadGame* Get();

	MEMBER_FN_PREFIX(TESSaveLoadGame);
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	DEFINE_MEMBER_FN(AddCreatedForm, UInt32, 0x00861780, TESForm * pForm);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	DEFINE_MEMBER_FN(AddCreatedForm, UInt32, 0x00861330, TESForm * pForm);
#elif EDITOR
#else
#error
#endif
};
