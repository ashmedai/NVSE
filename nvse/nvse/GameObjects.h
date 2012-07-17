#pragma once

#include "GameForms.h"
#include "GameBSExtraData.h"

class TESObjectCELL;
struct ScriptEventList;
class ActiveEffect;
class NiNode;

// 004?
class TESChildCell
{
public:
	TESChildCell();
	~TESChildCell();

	// no virtual destructor
	virtual TESObjectCELL *	GetChildCell(void);

//	void	** vtbl;	// 00
	UInt32	unk04;		// 04
};

// 68
class TESObjectREFR : public TESForm
{
public:
	MEMBER_FN_PREFIX(TESObjectREFR);

	TESObjectREFR();
	~TESObjectREFR();

	virtual void		Unk_4E(void);
	virtual void		Unk_4F(void);
	virtual void		Unk_50(void);
	virtual void		Unk_51(void);
	virtual void		Unk_52(void);
	virtual void		Unk_53(void);
	virtual void		Unk_54(void);
	virtual void		Unk_55(void);
	virtual void		Unk_56(void);
	virtual void		Unk_57(void);
	virtual void		Unk_58(void);
	virtual void		Unk_59(void);
	virtual void		Unk_5A(void);
	virtual void		Unk_5B(void);
	virtual void		Unk_5C(void);
	virtual void		Unk_5D(void);
	virtual void		Unk_5E(void);
	virtual void		Unk_5F(void);
	virtual void		Unk_60(void);
	virtual void		Unk_61(void);
	virtual void		Unk_62(void);
	virtual void		Unk_63(void);
	virtual void		Unk_64(void);
	virtual void		Unk_65(void);
	virtual void		Unk_66(void);
	virtual void		Unk_67(void);
	virtual void		Unk_68(void);
	virtual UInt32		Unk_69(void);
	virtual void		Unk_6A(UInt32 arg0);
	virtual void		Unk_6B(void);
	virtual void		Unk_6C(void);
	virtual void		Unk_6D(void);
	virtual void		Unk_6E(void);
	virtual void		Unk_6F(void);
	virtual void		Unk_70(void);
	virtual void		Unk_71(void);
	virtual void		Unk_72(void);
	virtual void		Unk_73(void);
	virtual NiNode *	GetNiNode(void);
	virtual void		Unk_75(void);
	virtual void		Unk_76(void);
	virtual void		Unk_77(void);
	virtual void		Unk_78(void);
	virtual void		Unk_79(void);
	virtual UInt32		Unk_7A(void);
	virtual void		Unk_7B(void);
	virtual void		Unk_7C(UInt32 arg0);
	virtual void		Unk_7D(void);
	virtual void		Unk_7E(UInt32 arg0);
	virtual void		Unk_7F(void);
	virtual void		Unk_80(UInt32 arg0);
	virtual void		Unk_81(UInt32 arg0);
	virtual void		Unk_82(void);
	virtual UInt32		Unk_83(void);
	virtual void		Unk_84(UInt32 arg0);
	virtual UInt32		Unk_85(void);
	virtual bool		Unk_86(void);
	virtual bool		Unk_87(void);
	virtual bool		Unk_88(void);
	virtual void		Unk_89(void);
	virtual void		Unk_8A(void);
	virtual void		Unk_8B(void);
	virtual bool		Unk_8C(void);
	virtual bool		Unk_8D(void);
	virtual void		Unk_8E(void);
	virtual void		Unk_8F(void);

	enum {
		kFlags_Taken		= 0x00000022,		// 2 bits TODO: which bit does what?
		kFlags_Persistent	= 0x00000400,		//shared bit with kFormFlags_QuestItem
	};

	struct RenderState
	{
		UInt32	unk00;
		UInt32	unk04;
		UInt32	unk08;
		float	unk0C;
		UInt32	unk10;
		NiNode	* niNode;
		// possibly more, need to find alloc
	};

	TESChildCell	childCell;				// 018

	TESForm			* baseForm;				// 020
	
	float			rotX, rotY, rotZ;		// 024 - either public or accessed via simple inline accessor common to all child classes
	float			posX, posY, posZ;		// 030 - seems to be private
	float			scale;					// 03C 

	TESObjectCELL	* parentCell;			// 040
	ExtraDataList	extraDataList;			// 044
	RenderState		* renderState;			// 064

	ScriptEventList *	GetEventList() const;

	bool IsTaken() const { return (flags & kFlags_Taken) == kFlags_Taken; } // Need to implement
	bool IsPersistent() const { return (flags & kFlags_Persistent) != 0; }
};

STATIC_ASSERT(offsetof(TESObjectREFR, baseForm) == 0x020);
STATIC_ASSERT(offsetof(TESObjectREFR, extraDataList) == 0x044);
STATIC_ASSERT(sizeof(TESObjectREFR) == 0x068);

class BaseProcess;

// 084
class MobileObject : public TESObjectREFR
{
public:
	MobileObject();
	~MobileObject();

	BaseProcess	* baseProcess;			// 068
	UInt32 unk06C[7];					// 06C
	// 084
};
STATIC_ASSERT(offsetof(MobileObject, baseProcess) == 0x068);
STATIC_ASSERT(sizeof(MobileObject) == 0x088);

// 00C
class MagicCaster
{
public:
	MagicCaster();
	~MagicCaster();

	UInt32 vtabl;
	UInt32 unk004[2];	// 004
};

STATIC_ASSERT(sizeof(MagicCaster) == 0x00C);

typedef tList<ActiveEffect> EffectNode;
// 010
class MagicTarget
{
public:
	MagicTarget();
	~MagicTarget();

	virtual void	Destructor(void);
	virtual TESObjectREFR *	GetParent(void);
	virtual EffectNode *	GetEffectList(void);

	UInt32 unk004[3];	// 004
};

STATIC_ASSERT(sizeof(MagicTarget) == 0x010);

class Actor : public MobileObject
{
public:
	Actor();
	~Actor();

	MagicCaster			magicCaster;			// 088
	MagicTarget			magicTarget;			// 094
	ActorValueOwner		avOwner;				// 0A4
	CachedValuesOwner	cvOwner;				// 0A8
};

STATIC_ASSERT(offsetof(Actor, magicCaster) == 0x088);

class Character : public Actor
{
public:
	Character();
	~Character();
};

class bhkRagdollController;
class bhkRagdollPenetrationUtil;
class PlayerMover;
class ImageSpaceModifierInstanceDOF;
class ImageSpaceModifierInstanceDRB;

// 9BC
class PlayerCharacter : public Character
{
public:
	PlayerCharacter();
	~PlayerCharacter();

	// used to flag controls as disabled in disabledControlFlags
	enum {
		kControlFlag_Movement		= 1 << 0,
		kControlFlag_Look			= 1 << 1,
		kControlFlag_Pipboy			= 1 << 2,
		kControlFlag_Fight			= 1 << 3,
		kControlFlag_POVSwitch		= 1 << 4,
		kControlFlag_RolloverText	= 1 << 5,
		kControlFlag_Sneak			= 1 << 6,
	};

	// lotsa data

	bhkRagdollController				* ragDollController;			// 0AC
	bhkRagdollPenetrationUtil			* ragDollPentrationUtil;		// 0B0
	UInt32								unk0B4[(0x190-0x0b4) >> 2];		// 0AC
	PlayerMover							* playerMover;					// 190
	UInt32								unk194[(0x5E4-0x194) >> 2];		// 184
	BGSNote								* note;							// 5E4
	UInt32								unk574;							// 5E8
	ImageSpaceModifierInstanceDOF		* unk5EC;						// 5EC
	ImageSpaceModifierInstanceDOF		* unk5F0;						// 5F0
	ImageSpaceModifierInstanceDRB		* unk5F4;						// 5F4
	UInt32								unk5F8[(0x64C - 0x5F8) >> 2];	// 5F8
	bool								bThirdPerson;					// 64C
	UInt8								unk64D[3];	
	UInt32								unk650[(0x694 - 0x650) >> 2];	// 650
	NiNode								* unk694;						// 694
	UInt32								unk698[(0x6A8-0x698) >> 2];		// 698
	TESTopic							* topic;						// 6A8
	UInt32								unk6AC[3];						// 6AC
	TESQuest							* quest;						// 6B8
	tList<BGSQuestObjective>			questObjectiveList;			// 6BC
	UInt32								unk6C4[(0x9BC - 0x6C4) >> 2];	// 61C


	static PlayerCharacter*	GetSingleton();
};

STATIC_ASSERT(sizeof(PlayerCharacter) == 0x9BC);
STATIC_ASSERT(offsetof(PlayerCharacter, questObjectiveList) == 0x6BC);
STATIC_ASSERT(offsetof(PlayerCharacter, bThirdPerson) == 0x64C);
STATIC_ASSERT(offsetof(PlayerCharacter, playerMover) == 0x190);
