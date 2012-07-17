#include "Commands_MiscRef.h"

#include "GameForms.h"
#include "GameObjects.h"
#include "GameAPI.h"
#include "GameRTTI.h"
#include "GameBSExtraData.h"
#include "GameExtraData.h"


bool Cmd_GetBaseObject_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (thisObj && thisObj->baseForm) {
		*refResult = thisObj->baseForm->refID;
		if (IsConsoleMode())
			Console_Print("GetBaseObject >> %08x (%s)", thisObj->baseForm->refID, GetFullName(thisObj->baseForm));
	}
	return true;
}

bool Cmd_IsPersistent_Execute(COMMAND_ARGS)
{
	*result = 0;

	if (thisObj)
		*result = (thisObj->IsPersistent() ? 1 : 0);

	return true;
}

bool Cmd_GetLinkedDoor_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	ExtraTeleport* xTele = GetByTypeCast(thisObj->extraDataList, Teleport);
	if (xTele)
		*refResult = xTele->data->linkedDoor->refID;

	return true;
}

bool Cmd_GetTeleportCell_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)
		return true;

	ExtraTeleport* xTele = GetByTypeCast(thisObj->extraDataList, Teleport);
	if (xTele) {
		*refResult = xTele->data->linkedDoor->parentCell->refID;
	}

	return true;
}

bool Cmd_GetParentCell_Execute(COMMAND_ARGS)
{
	UInt32	* refResult = (UInt32 *)result;
	*refResult = 0;

	if(!thisObj) return true;

	if (thisObj->parentCell) {
		*refResult = thisObj->parentCell->refID;
	}

	return true;
}

bool Cmd_GetParentWorldspace_Execute(COMMAND_ARGS)
{
	UInt32	* refResult = (UInt32 *)result;
	*refResult = 0;

	if(!thisObj) return true;

	if (thisObj->parentCell && thisObj->parentCell->worldSpace) {
		*refResult = thisObj->parentCell->worldSpace->refID;
	}

	return true;
}

struct CellScanInfo
{
	TESObjectCELL::RefList::Iterator	prev;	//last ref returned to script
	const	TESObjectCELL * curCell;					//cell currently being scanned
	const	TESObjectCELL * cell;						//player's current cell
	const	TESWorldSpace * world;
	SInt8	curX;										//offsets of curCell from player's cell
	SInt8	curY;
	UInt8	formType;									//form type to scan for
	UInt8	cellDepth;									//depth of adjacent cells to scan
	bool	includeTakenRefs;

	CellScanInfo() : curCell(NULL), cell(NULL), world(NULL), curX(0), curY(0), formType(0), cellDepth(0), includeTakenRefs(false)
	{	}

	CellScanInfo(UInt8 _cellDepth, UInt8 _formType, bool _includeTaken, TESObjectCELL* _cell) 
					:	curCell(NULL), cell(_cell), world(NULL), curX(0), curY(0), formType(_formType), cellDepth(_cellDepth), includeTakenRefs(_includeTaken)
	{
		world = cell->worldSpace;

		if (world && cellDepth)		//exterior, cell depth > 0
		{
			curX = cell->coords->x - cellDepth;
			curY = cell->coords->y - cellDepth;
			UInt32 key = (curX << 16) + ((curY << 16) >> 16);
			curCell = world->cellMap->Lookup(key);
		}
		else
		{
			cellDepth = 0;
			curCell = cell;
			curX = cell->coords->x;
			curY = cell->coords->y;
		}
	}

	bool NextCell()		//advance to next cell in area
	{
		if (!world || !cellDepth)
		{
			curCell = NULL;
			return false;
		}

		do
		{
			if (curX - cell->coords->x == cellDepth)
			{
				if (curY - cell->coords->y == cellDepth)
				{
					curCell = NULL;
					return false;
				}
				else
				{
					curY++;
					curX -= cellDepth * 2;
					UInt32 key = (curX << 16) + ((curY << 16) >> 16);
					curCell = world->cellMap->Lookup(key);
				}
			}
			else
			{
				curX++;
				UInt32 key = (curX << 16) + ((curY << 16) >> 16);
				curCell = world->cellMap->Lookup(key);
			}
		}while (!curCell);
		
		return true;
	}

	void FirstCell()	//init curCell to point to first valid cell
	{
		if (!curCell)
			NextCell();
	}

};

class RefMatcherAnyForm
{
	bool m_includeTaken;
public:
	RefMatcherAnyForm(bool includeTaken) : m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (m_includeTaken || !(refr->IsTaken()))
			return true;
		else
			return false;
	}
};

class RefMatcherFormType
{
	UInt32 m_formType;
	bool m_includeTaken;
public:
	RefMatcherFormType(UInt32 formType, bool includeTaken) : m_formType(formType), m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (!m_includeTaken && refr->IsTaken())
			return false;
		else if (refr->baseForm->typeID == m_formType && refr->baseForm->refID != 7)	//exclude player for kFormType_NPC
			return true;
		else
			return false;
	}
};

class RefMatcherActor
{
public:
	RefMatcherActor()
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (refr->baseForm->typeID == kFormType_Creature)
			return true;
		else if (refr->baseForm->typeID == kFormType_NPC && refr->baseForm->refID != 7) //exclude the player
			return true;
		else
			return false;
	}
};

class RefMatcherItem
{
	bool m_includeTaken;
public:
	RefMatcherItem(bool includeTaken) : m_includeTaken(includeTaken)
		{ }

	bool Accept(const TESObjectREFR* refr)
	{
		if (!m_includeTaken && refr->IsTaken())
			return false;

		switch (refr->baseForm->typeID)
		{
			case kFormType_Armor:
			case kFormType_Book:
			case kFormType_Clothing:
			case kFormType_Ingredient:
			case kFormType_Misc:
			case kFormType_Weapon:
			case kFormType_Ammo:
			case kFormType_Key:
			case kFormType_AlchemyItem:
			case kFormType_ARMA:
				return true;

			case kFormType_Light:
				TESObjectLIGH* light = DYNAMIC_CAST(refr->baseForm, TESForm, TESObjectLIGH);
				if (light)
					if (light->icon.ddsPath.m_dataLen)	//temp hack until I find canCarry flag on TESObjectLIGH
						return true;
		}
		return false;
	}
};

static const TESObjectCELL::RefList::Iterator GetCellRefEntry(const TESObjectCELL::RefList& refList, UInt32 formType, TESObjectCELL::RefList::Iterator prev, bool includeTaken /*, ProjectileFinder* projFinder = NULL*/)
{
	TESObjectCELL::RefList::Iterator entry;
	switch(formType)
	{
	case 0:		//Any type
		entry = refList.Find(RefMatcherAnyForm(includeTaken), prev);
		break;
	case 200:	//Actor
		entry = refList.Find(RefMatcherActor(), prev);
		break;
	case 201:	//Inventory Item
		entry = refList.Find(RefMatcherItem(includeTaken), prev);
		break;
	//case 202:	//Owned Projectile
	//	if (projFinder)
	//		entry = visitor.Find(*projFinder, prev);
	//	break;
	default:
		entry = refList.Find(RefMatcherFormType(formType, includeTaken), prev);
	}

	return entry;
}

static TESObjectREFR* CellScan(Script* scriptObj, TESObjectCELL* cellToScan = NULL, UInt32 formType = 0, UInt32 cellDepth = 0, bool getFirst = false, bool includeTaken = false /*, ProjectileFinder* projFinder = NULL*/)
{
	static std::map<UInt32, CellScanInfo> scanScripts;
	UInt32 idx = scriptObj->refID;

	if (getFirst)
		scanScripts.erase(idx);

	if (scanScripts.find(idx) == scanScripts.end())
	{
		scanScripts[idx] = CellScanInfo(cellDepth, formType, includeTaken, cellToScan);
		scanScripts[idx].FirstCell();
	}

	CellScanInfo* info = &(scanScripts[idx]);

	bool bContinue = true;
	while (bContinue)
	{
		info->prev = GetCellRefEntry(info->curCell->objectList, info->formType, info->prev, info->includeTakenRefs /*, projFinder*/);
		if (info->prev.End() || !(*info->prev))				//no ref found
		{
			if (!info->NextCell())			//check next cell if possible
				bContinue = false;
		}
		else
			bContinue = false;			//found a ref
	}

	if ((*info->prev))
		return info->prev.Get();
	else
	{
		scanScripts.erase(idx);
		return NULL;
	}

}

static bool GetFirstRef_Execute(COMMAND_ARGS, bool bUsePlayerCell = true)
{
	UInt32 formType = 0;
	UInt32 cellDepth = -1;
	UInt32 bIncludeTakenRefs = 0;
	UInt32* refResult = (UInt32*)result;
	TESObjectCELL* cell = NULL;
	*refResult = 0;
	
	PlayerCharacter* pc = PlayerCharacter::GetSingleton();
	if (!pc)
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	if (bUsePlayerCell)
	{
		if (ExtractArgs(EXTRACT_ARGS, &formType, &cellDepth, &bIncludeTakenRefs))
			cell = pc->parentCell;
		else
			return true;
	}
	else
		if (!ExtractArgs(EXTRACT_ARGS, &cell, &formType, &cellDepth, &bIncludeTakenRefs))
			return true;

	if (!cell)
		return true;

	if (cellDepth == -1)
		cellDepth = 0;

	TESObjectREFR* refr = CellScan(scriptObj, cell, formType, cellDepth, true, bIncludeTakenRefs ? true : false);
	if (refr)
		*refResult = refr->refID;

	if (IsConsoleMode())
		Console_Print("GetFirstRef >> %08x", *refResult);

	return true;
}

bool Cmd_GetFirstRef_Execute(COMMAND_ARGS)
{
	GetFirstRef_Execute(PASS_COMMAND_ARGS, true);
	return true;
}

bool Cmd_GetFirstRefInCell_Execute(COMMAND_ARGS)
{
	GetFirstRef_Execute(PASS_COMMAND_ARGS, false);
	return true;
}

bool Cmd_GetNextRef_Execute(COMMAND_ARGS)
{
	
	PlayerCharacter* pc = PlayerCharacter::GetSingleton();
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	TESObjectREFR* refr = CellScan(scriptObj);
	if (refr)
		*refResult = refr->refID;

	return true;
}

static bool GetNumRefs_Execute(COMMAND_ARGS, bool bUsePlayerCell = true)
{
	*result = 0;
	UInt32 formType = 0;
	UInt32 cellDepth = -1;
	UInt32 includeTakenRefs = 0;

	PlayerCharacter* pc = PlayerCharacter::GetSingleton();
	if (!pc || !(pc->parentCell))
		return true;						//avoid crash when these functions called in main menu before parentCell instantiated

	TESObjectCELL* cell = NULL;
	if (bUsePlayerCell)
		if (ExtractArgs(EXTRACT_ARGS, &formType, &cellDepth, &includeTakenRefs))
			cell = pc->parentCell;
		else
			return true;
	else
		if (!ExtractArgs(EXTRACT_ARGS, &cell, &formType, &cellDepth, &includeTakenRefs))
			return true;

	if (!cell)
		return true;

	bool bIncludeTakenRefs = includeTakenRefs ? true : false;
	if (cellDepth == -1)
		cellDepth = 0;

	CellScanInfo info(cellDepth, formType, bIncludeTakenRefs, cell);
	info.FirstCell();

	while (info.curCell)
	{
		const TESObjectCELL::RefList& refList = info.curCell->objectList;
		switch (formType)
		{
		case 0:
			*result += refList.CountIf(RefMatcherAnyForm(bIncludeTakenRefs));
			break;
		case 200:
			*result += refList.CountIf(RefMatcherActor());
			break;
		case 201:
			*result += refList.CountIf(RefMatcherItem(bIncludeTakenRefs));
			break;
		default:
			*result += refList.CountIf(RefMatcherFormType(formType, bIncludeTakenRefs));
		}
		info.NextCell();
	}

	return true;
}

bool Cmd_GetNumRefs_Execute(COMMAND_ARGS)
{
	return GetNumRefs_Execute(PASS_COMMAND_ARGS, true);
}

bool Cmd_GetNumRefsInCell_Execute(COMMAND_ARGS)
{
	return GetNumRefs_Execute(PASS_COMMAND_ARGS, false);
}

bool Cmd_GetRefCount_Execute(COMMAND_ARGS)
{
	if (!thisObj)
		return true;

	*result = 1;

	ExtraCount* pXCount = GetByTypeCast(thisObj->extraDataList, Count);
	if (pXCount) {
		*result = pXCount->count;
		if (IsConsoleMode())
			Console_Print("%s: %d", GetFullName(thisObj->baseForm), pXCount->count);
	}
	return true;
}

bool Cmd_SetRefCount_Execute(COMMAND_ARGS)
{
	UInt32 newCount = 0;
	if (!ExtractArgs(EXTRACT_ARGS, &newCount))
		return true;
	else if (!thisObj || newCount > 32767 || newCount < 1)
		return true;

	ExtraCount* pXCount = GetByTypeCast(thisObj->extraDataList, Count);
	if (!pXCount) {
		pXCount = ExtraCount::Create();
		if (!thisObj->extraDataList.Add(pXCount)) {
			FormHeap_Free(pXCount);
			return true;
		}
	}
	pXCount->count = newCount;

	return true;
}

bool Cmd_GetOpenKey_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32*)result;
	*refResult = 0;

	if (!thisObj)	return true;
	
	ExtraLock* xLock = GetByTypeCast(thisObj->extraDataList, Lock);
	if (xLock && xLock->data && xLock->data->key)
		*refResult = xLock->data->key->refID;

#if _DEBUG
	if (IsConsoleMode())
		Console_Print("GetOpenKey >> %X", *result);
#endif
	return true;
}

bool Cmd_SetOpenKey_Execute(COMMAND_ARGS)
{
	TESForm* form;
	*result = 0;
	
	if (!thisObj)	
		return true;

	ExtractArgsEx(EXTRACT_ARGS_EX, &form);
	if (!form)	
		return true;

	TESKey* key = DYNAMIC_CAST(form, TESForm, TESKey);
	if (!key)	
		return true;

	ExtraLock* xLock = GetByTypeCast(thisObj->extraDataList, Lock);
	if (!xLock) {
		xLock = ExtraLock::Create();
		if (!thisObj->extraDataList.Add(xLock))
		{
			FormHeap_Free(xLock->data);
			FormHeap_Free(xLock);
			return true;
		}
	}

	if (xLock)
	{
		xLock->data->key = key;
		*result = 1;
	}

	return true;
}

static TESForm* GetOwner(BaseExtraList& xDataList)
{
	TESForm* owner = NULL;
	ExtraOwnership* xOwner = GetByTypeCast(xDataList, Ownership);
	if (xOwner)
		owner = xOwner->owner;

	return owner;
}

static UInt32 GetOwningFactionRequiredRank(BaseExtraList& xDataList)
{
	ExtraRank * xRank = GetByTypeCast(xDataList, Rank);
	if(xRank)
		return xRank->rank;

	return 0;
}

bool Cmd_GetOwner_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32 *)result;
	*refResult = 0;

	if(!thisObj)
		return true;

	TESForm * owner = GetOwner(thisObj->extraDataList);
	if(owner)
		*refResult = owner->refID;

	return true;
}

bool Cmd_GetParentCellOwner_Execute(COMMAND_ARGS)
{
	UInt32* refResult = (UInt32 *)result;
	*refResult = 0;

	if(!thisObj)
		return true;

	TESForm * owner = GetOwner(thisObj->parentCell->extraDataList);
	if(owner)
		*refResult = owner->refID;

	return true;
}

bool Cmd_GetOwningFactionRequiredRank_Execute(COMMAND_ARGS)
{
	*result = 0;

	if(!thisObj)
		return true;

	*result = GetOwningFactionRequiredRank(thisObj->extraDataList);

	return true;
}

bool Cmd_GetParentCellOwningFactionRequiredRank_Execute(COMMAND_ARGS)
{
	*result = 0;

	if(!thisObj)
		return true;

	*result = GetOwningFactionRequiredRank(thisObj->parentCell->extraDataList);

	return true;
}

bool Cmd_GetActorBaseFlagsLow_Execute(COMMAND_ARGS)
{
	TESActorBase	* obj = NULL;

	*result = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &obj)) return true;

	if(!obj && thisObj && thisObj->baseForm)
		obj = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESActorBase);

	if(obj)
		*result = obj->flags & 0xFFFF;

	return true;
}

bool Cmd_SetActorBaseFlagsLow_Execute(COMMAND_ARGS)
{
	TESActorBase	* obj = NULL;
	UInt32			data;

	*result = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &data, &obj)) return true;

	if(!obj && thisObj && thisObj->baseForm)
		obj = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESActorBase);

	if(obj)
		obj->flags = (data & 0x0000FFFF) | (obj->flags & 0xFFFF0000);

	return true;
}

bool Cmd_GetActorBaseFlagsHigh_Execute(COMMAND_ARGS)
{
	TESActorBase	* obj = NULL;

	*result = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &obj)) return true;

	if(!obj && thisObj && thisObj->baseForm)
		obj = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESActorBase);

	if(obj)
		*result = (obj->flags >> 16) & 0xFFFF;

	return true;
}

bool Cmd_SetActorBaseFlagsHigh_Execute(COMMAND_ARGS)
{
	TESActorBase	* obj = NULL;
	UInt32			data;

	*result = 0;

	if(!ExtractArgs(EXTRACT_ARGS, &data, &obj)) return true;

	if(!obj && thisObj && thisObj->baseForm)
		obj = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESActorBase);

	if(obj)
		obj->flags = ((data << 16) & 0xFFFF0000) | (obj->flags & 0x0000FFFF);

	return true;
}
