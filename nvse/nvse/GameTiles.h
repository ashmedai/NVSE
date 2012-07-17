#pragma once

#include "nvse/GameTypes.h"

typedef UInt32 (* _TraitNameToID)(const char* traitName);
extern const _TraitNameToID TraitNameToID;

const char * TraitIDToName(int id);	// slow

//	Tile			
//		TileRect		3C
//			TileMenu	40
//		TileImage		48
//		TileText		4C
//		Tile3D			50

class Action_Tile;
class NiNode;
class Menu;

// 38+
class Tile
{
public:
	Tile();
	~Tile();

	MEMBER_FN_PREFIX(Tile);
#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525
	DEFINE_MEMBER_FN(SetStringValue, void, 0x00A01350, UInt32 valueID, const char* str, bool bPropagate);
	DEFINE_MEMBER_FN(SetFloatValue, void, 0x00A012D0, UInt32 valueID, float num, bool bPropagate);
#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng
	DEFINE_MEMBER_FN(SetStringValue, void, 0x00A01220, UInt32 valueID, const char* str, bool bPropagate);
	DEFINE_MEMBER_FN(SetFloatValue, void, 0x00A011A0, UInt32 valueID, float num, bool bPropagate);
#elif EDITOR
#else
#error
#endif

	virtual void			Destroy(bool noDealloc);
	virtual void			Init(Tile * parent, const char * name, UInt32 arg2);
	virtual UInt32			Unk_02(void);
	virtual UInt32			GetType(void);		// returns one of kTileValue_XXX
	virtual const char *	GetTypeStr(void);	// 4-byte id
	virtual bool			Unk_05(UInt32 arg0, UInt32 arg1);
	virtual UInt32			Unk_06(UInt32 arg0, UInt32 arg1, UInt32 arg2);
	virtual void			Unk_07(void);
	virtual UInt32			Unk_08(void);
	virtual void			Unk_09(UInt32 arg0, UInt32 arg1, UInt32 arg2);

	// 14
	struct Value
	{
		UInt32		id;			// 00
		Tile		* parent;	// 04
		float		num;		// 08
		char		* str;		// 0C
		Action_Tile	* action;	// 10
	};

	struct ChildNode
	{
		ChildNode	* next;
		ChildNode	* prev;
		Tile		* child;
	};

	tList <ChildNode>			childList;	// 04
	UInt32						unk0C;		// 0C looks like childcount
	BSSimpleArray <Value *>		values;		// 10
	String						name;		// 20
	Tile						* parent;	// 28
	NiNode						* node;		// 2C
	UInt32						flags;		// 30
	UInt8						unk34;		// 34
	UInt8						unk35;		// 35
	UInt8						pad35[2];	// 36

	static UInt32	TraitNameToID(const char * traitName);
	Value *			GetValue(UInt32 typeID);
	Value *			GetValue(const char * valueName);
	Tile *			GetChild(const char * childName);
	Tile *			GetComponent(const char * componentTile, std::string * trait);
	Tile *			GetComponentTile(const char * componentTile);
	Value *			GetComponentValue(const char * componentPath);
	std::string		GetQualifiedName(void);

	void			Dump(void);
};

// 3C
class TileRect : public Tile
{
public:
	UInt32	unk38;	// 38
};

// 40
class TileMenu : public TileRect
{
public:
	Menu	* menu;	// 3C - guess
};

void Debug_DumpTraits(void);
