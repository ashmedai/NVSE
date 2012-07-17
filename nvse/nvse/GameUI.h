#pragma once

#include "GameTiles.h"
#include "GameTypes.h"


class Menu;
class SceneGraph;
class FOPipboyManager;
class NiObject;
class TESObjectREFR;
class NiNode;

// 584
class InterfaceManager
{
public:
	InterfaceManager();
	~InterfaceManager();

	static InterfaceManager *	GetSingleton(void);
	static bool					IsMenuVisible(UInt32 menuType);
	static Menu *				GetMenuByType(UInt32 menuType);
	static TileMenu *			GetMenuByPath(const char * componentPath, const char ** slashPos);
	static Tile::Value *		GetMenuComponentValue(const char * componentPath);
	static Tile *				GetMenuComponentTile(const char * componentPath);




	UInt32				flags;							// 000
	SceneGraph			* sceneGraph004;				// 004
	SceneGraph			* sceneGraph008;				// 008
	UInt32				unk00C[(0x028 - 0x00C) >> 2];	// 00C
	Tile				* cursor;						// 028
	UInt32				unk02C[(0x070 - 0x02C) >> 2];	// 02C
	TESObjectREFR		* debugSelection;				// 070
	UInt32				unk074;							// 074
	UInt32				unk078;							// 078
	UInt32				unk07C;							// 07C
	NiNode				* niNode080;					// 080
	NiNode				* niNode084;					// 084
	UInt32				unk088[(0x09C - 0x088) >> 2];	// 088
	Tile				* menuRoot;						// 09C
	Tile				* unk0A0;						// 0A0 seen "Strings"
	void				* unk0A4;						// 0A4 saw Tile? seen NiNode
	UInt32				unk0A8;							// 0A8
	NiObject			* unk0AC;						// 0AC seen NiAlphaProperty
	UInt32				unk0B0[(0x0CC - 0x0B0) >> 2];	// 0B0
	Tile				* activeTile;					// 0CC
	UInt32				unk0D0[(0x0E4 - 0x0D0) >> 2];	// 0D0
	UInt8				msgBoxButton;					// 0E4 -1 if no button pressed
	UInt8				pad0E5[3];
	UInt32				unk0E8;							// 0E8
	UInt32				unk0EC;							// 0EC
	UInt32				unk0F0;							// 0F0
	UInt32				unk0F4;							// 0F4
	UInt32				unk0F8;							// 0F8
	TESObjectREFR		* crosshairRef;					// 0FC
	UInt32				unk100[(0x174 - 0x100) >> 2];	// 100
	FOPipboyManager		* pipboyManager;				// 174
	UInt32				unk178;							// 178
	NiTArray<UInt32>	array17C;						// 17C NiTPrimitiveArray@PackedMenu@BGSMenuPacker
	UInt32				unk18C[(0x394 - 0x18C) >> 2];	// 18C
	NiObject			* unk394;						// 394 seen NiSourceTexture
	UInt32				unk398[(0x454 - 0x398) >> 2];	// 398
	NiTArray<UInt32>	array454;						// 454 NiTPrimitiveArray@TextureType@BSTextureManager
	NiTArray<UInt32>	array464;						// 464 NiTPrimitiveArray@FilterMode@NiTexturingProperty
	UInt32				unk468[(0x584 - 0x474) >> 2];	// 474
};
STATIC_ASSERT(sizeof(InterfaceManager) == 0x584);
STATIC_ASSERT(offsetof(InterfaceManager, debugSelection) == 0x070);
STATIC_ASSERT(offsetof(InterfaceManager, crosshairRef) == 0x0FC);
STATIC_ASSERT(offsetof(InterfaceManager, pipboyManager) == 0x174);

void Debug_DumpMenus(void);
