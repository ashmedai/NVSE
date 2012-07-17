#pragma once

#include "GameForms.h"
#include "Utilities.h"

class TESReputation;
class TESChallenge;
class TESRecipe;
class TESRecipeCategory;
class TESAmmoEffect;
class TESCasino;
class TESCaravanDeck;
class TESTopicInfo;
class BGSDehydrationStage;
class BGSHungerStage;
class BGSSleepDeprivationStage;
class TESLoadScreenType;
class MediaSet;
class MediaLocationController;
class TESRegionList;
class TESRegionManager;
class BSFile;


// 10
class BoundObjectListHead
{
public:
	BoundObjectListHead();
	~BoundObjectListHead();

	UInt32			boundObjectCount;	// 0
	TESBoundObject	* first;			// 4
	TESBoundObject	* last;				// 8
	UInt32			unkC;				// C
};

struct FormRecordData
{
	UInt8		typeID;		// corresponds to kFormType_XXX
	UInt32		typeCode;	// i.e. 'GMST', 'FACT'
	UInt32		unk08;		// only seen zero
};

struct ChunkHeader
{
	UInt32	type : 4;	// i.e. 'XGRD', 'DATA'
	UInt16	size : 2;
};

struct ModInfo		// referred to by game as TESFile
{
	ModInfo();
	~ModInfo();

	// 18 info about currently loading form
	struct FormInfo
	{
		UInt32		recordType;			// 00 i.e. 'FACT', 'GMST'
		UInt32		unk04;				// 04 looks like size of entire record
		UInt32		formFlags;			// 08 copied to TESForm->flags
		UInt32		formID;				// 0C 
		UInt32		unk10;				// 10
		UInt16		unk14;				// 14 always initialized to 0F on SaveForm. 
		UInt16		unk16;
	};

	tList<UInt32>						unkList;			// 000
	UInt32 /*NiTPointerMap<TESFile*>*/	* pointerMap;		// 008
	UInt32								unk00C;				// 00C
	BSFile*								unkFile;			// 010
	UInt32								unk014;				// 014 
	void								* unk018;			// 018 seen all zeroes. size unknown
	void								* unk01C;			// 01C as above
	char								name[0x104];		// 020
	char								filepath[0x104];	// 124
	UInt32								unk228;				// 228
	UInt32								unk22C;				// init'd to dword_F469CC (0x2800) same val as BSFile+10?
	UInt32								unk230;				// 230
	UInt32								unk234;				// 234
	UInt32								unk238;				// 238
	UInt32								unk23C;				// 23C
	FormInfo							formInfo;			// 240
	ChunkHeader							subRecord;			// 258
	UInt32								unk260;				// 260
	UInt32								fileOffset;			// 264
	UInt32								dataOffset;			// 268 index into dataBuf
	UInt32								subrecordBytesRead;	// 26C generates error on Read if != expected length
	UInt32						unk268[(0x298-0x270) >> 2];	// 270
	UInt8								unk298;				// 298
	UInt8								bIsBigEndian;		// 299
	UInt8								unk29A;				// 29A
	UInt8								pad29B;
	WIN32_FIND_DATA						fileData;			// 29C
	float								unk3DC;				// 3DC init'd to 0.94
	UInt32								unk3E0;				// 3E0
	UInt32								flags;				// 3E4 init'd to 0x00000800. 4000 and 40000 do stuff
	UInt8								unk3E8;				// 3E8
	UInt8								pad3E9[3];
	UInt32								unk3EC;				// 3EC
	UInt32								unk3F0;				// 3F0
	UInt32								unk3F4;				// 3F4
	UInt32								unk3F8;				// 3F8
	UInt32								numRefMods;			// 3FC related to modindex; see 4472D0
																// formIDs in mod are as saved in GECK, must fix up at runtime
	ModInfo								** refModInfo;		// 400 used to look up modInfo based on fixed mod index, double-check
	UInt32								unk404;				// 404
	UInt32								unk408;				// 408
	UInt8								modIndex;			// 40C init to 0xFF
	UInt8								pad40D[3];
	String								author;				// 410
	String								description;		// 418
	void								* dataBuf;			// 420 
	UInt32								unk424;				// 424 looks like size of entire record
	UInt8								unk428;				// 428
	UInt8								pad429[3];
	
	bool IsLoaded() const { return true; }

	/*** used by TESForm::LoadForm() among others ***/
//	MEMBER_FN_PREFIX(ModInfo);
//	DEFINE_MEMBER_FN(GetNextChunk, UInt32, _ModInfo_GetNextChunk);	// returns chunk type
//	DEFINE_MEMBER_FN(GetChunkData, bool, _ModInfo_GetChunkData, UInt8* buf, UInt32 bufSize); // max size, not num to read
//	DEFINE_MEMBER_FN(Read32, void, _ModInfo_Read32, void* out);
//	DEFINE_MEMBER_FN(HasMoreSubrecords, bool, _ModInfo_HasMoreSubrecords);
};

STATIC_ASSERT(sizeof(WIN32_FIND_DATA) == 0x140);
STATIC_ASSERT(offsetof(ModInfo, name) == 0x20);
STATIC_ASSERT(offsetof(ModInfo, subRecord) == 0x258);
STATIC_ASSERT(sizeof(ModInfo) == 0x42C);



struct ModList
{
	tList<ModInfo>		modInfoList;
	UInt32				loadedModCount;
	ModInfo*			loadedMods[0xFF];
};

// 5B8
class DataHandler
{
public:
	DataHandler();
	~DataHandler();

	UInt32							unk00;					// 000
	BoundObjectListHead				* boundObjectList;		// 004
	tList<TESPackage>				packageList;			// 008
	tList<TESWorldSpace>			worldSpaceList;			// 010
	tList<TESClimate>				climateList;			// 019
	tList<TESImageSpace>			imageSpaceList;			// 020
	tList<TESImageSpaceModifier>	imageSpaceModList;		// 028
	tList<TESWeather>				weatherList;			// 030
	tList<EnchantmentItem>			enchantmentItemList;	// 038
	tList<SpellItem>				spellItemList;			// 040
	tList<BGSHeadPart>				headPartList;			// 048
	tList<TESHair>					hairList;				// 050
	tList<TESEyes>					eyeList;				// 058
	tList<TESRace>					raceList;				// 060
	tList<BGSEncounterZone>			encounterZoneList;		// 068
	tList<TESLandTexture>			landTextureList;		// 070
	tList<BGSCameraShot>			cameraShotList;			// 078
	tList<TESClass>					classList;				// 080
	tList<TESFaction>				factionList;			// 088
	tList<TESReputation>			reputationList;			// 090
	tList<TESChallenge>				challengeList;			// 098
	tList<TESRecipe>				recipeList;				// 0A0
	tList<TESRecipeCategory>		recipeCategoryList;		// 0A8
	tList<TESAmmoEffect>			ammoEffectList;			// 0B0
	tList<TESCasino>				casinoList;				// 0B8
	tList<TESCaravanDeck>			caravanDeckList;		// 0C0
	tList<Script>					scriptList;				// 0C8
	tList<TESSound>					soundList;				// 0D0
	tList<BGSAcousticSpace>			acousticSpaceList;		// 0D8
	tList<BGSRagdoll>				ragdollList;			// 0E0
	tList<TESGlobal>				globalList;				// 0E8
	tList<BGSVoiceType>				voiceTypeList;			// 0F0
	tList<BGSImpactData>			impactDataList;			// 0F8
	tList<BGSImpactDataSet>			impactDataSetList;		// 100
	tList<TESTopic>					topicList;				// 108
	tList<TESTopicInfo>				topicInfoList;			// 110
	tList<TESQuest>					questList;				// 118
	tList<TESCombatStyle>			combatStyleList;		// 120
	tList<TESLoadScreen>			loadScreenList;			// 128
	tList<TESWaterForm>				waterFormList;			// 130
	tList<TESEffectShader>			effectShaderList;		// 138
	tList<BGSProjectile>			projectileList;			// 140
	tList<BGSExplosion>				explosionList;			// 148
	tList<BGSRadiationStage>		radiationStageList;		// 150
	tList<BGSDehydrationStage>		dehydrationStageList;	// 158
	tList<BGSHungerStage>			hungerStageList;		// 160
	tList<BGSSleepDeprivationStage>	sleepDepriveStageList;	// 168
	tList<BGSDebris>				debrisList;				// 170
	tList<BGSPerk>					perkList;				// 178
	tList<BGSBodyPartData>			bodyPartDataList;		// 180
	tList<BGSNote>					noteList;				// 188
	tList<BGSListForm>				listFormList;			// 190
	tList<BGSMenuIcon>				menuIconList;			// 198
	tList<TESObjectANIO>			anioList;				// 1A0
	tList<BGSMessage>				messageList;			// 1A8
	tList<BGSLightingTemplate>		lightningTemplateList;	// 1B0
	tList<BGSMusicType>				musicTypeList;			// 1B8
	tList<TESLoadScreenType>		loadScreenTypeList;		// 1C0
	tList<MediaSet>					mediaSetList;			// 1C8
	tList<MediaLocationController>	mediaLocControllerList;	// 1D0
	TESRegionList					* regionList;			// 1D8
	NiTArray<TESObjectCELL*>		cellArray;				// 1DC
	NiTArray<BGSAddonNode*>			addonArray;				// 1EC

	UInt32	unk188[0x5];									// 188
	ModList							modList;				// 19C
	UInt32							unk5A4;					// 5A4
	UInt32							unk5A8;					// 5A8
	UInt32							unk5AC;					// 5AC
	TESRegionManager				* regionManager;		// 5B0
	UInt32							unk5B4;					// 5B4
	
	static DataHandler* Get();
	const ModInfo* LookupModByName(const char* modName);
	UInt8 GetModIndex(const char* modName);
	UInt8 GetActiveModCount() const;

	MEMBER_FN_PREFIX(DataHandler);
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	DEFINE_MEMBER_FN(DoAddForm, UInt32, 0x004603B0, TESForm * pForm);	// stupid name is because AddForm is redefined in windows header files
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	DEFINE_MEMBER_FN(DoAddForm, UInt32, 0x00461160, TESForm * pForm);	// stupid name is because AddForm is redefined in windows header files
#elif EDITOR
#else
#error
#endif

	TESQuest* GetQuestByName(const char* questName);
};
