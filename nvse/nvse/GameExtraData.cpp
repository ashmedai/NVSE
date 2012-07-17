#include "GameExtraData.h"
#include "GameBSExtraData.h"

struct GetMatchingEquipped {
	FormMatcher& m_matcher;
	EquipData m_found;

	GetMatchingEquipped(FormMatcher& matcher) : m_matcher(matcher) {
		m_found.pForm = NULL;
		m_found.pExtraData = NULL;
	}

	bool Accept(ExtraContainerChanges::EntryData* pEntryData) {
		if (pEntryData) {
			// quick check - needs an extendData or can't be equipped
			ExtraContainerChanges::ExtendDataList* pExtendList = pEntryData->extendData;
			if (pExtendList && m_matcher.Matches(pEntryData->type)) { 
				SInt32 n = 0;
				ExtraDataList* pExtraDataList = pExtendList->GetNthItem(n);
				while (pExtraDataList) {
					if (pExtraDataList->HasType(kExtraData_Worn) || pExtraDataList->HasType(kExtraData_WornLeft)) {
						m_found.pForm = pEntryData->type;
						m_found.pExtraData = pExtraDataList;
						return false;
					}
					n++;
					pExtraDataList = pExtendList->GetNthItem(n);
				}
			}
		}
		return true;
	}

	EquipData Found() {
		return m_found;
	}
};


EquipData ExtraContainerChanges::FindEquipped(FormMatcher& matcher) const
{
	FoundEquipData equipData;
	if (data && data->objList) {
		GetMatchingEquipped getEquipped(matcher);
		data->objList->Visit(getEquipped);
		equipData = getEquipped.Found();
	}
	return equipData;
};

STATIC_ASSERT(sizeof(ExtraHealth) == 0x10);
STATIC_ASSERT(sizeof(ExtraLock) == 0x10);
STATIC_ASSERT(sizeof(ExtraCount) == 0x10);
STATIC_ASSERT(sizeof(ExtraTeleport) == 0x10);

#if RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525

static const UInt32 s_ExtraHealthVtbl = 0x010158E4;
static const UInt32 s_ExtraLockVtbl = 0x0101589C;
static const UInt32 s_ExtraCountVtbl = 0x010158D8;
static const UInt32 s_ExtraTeleportVtbl = 0x010158A8;
static const UInt32 s_ExtraWeaponModFlagsVtbl = 0x010159A4;

#elif RUNTIME_VERSION == RUNTIME_VERSION_1_4_0_525ng

static const UInt32 s_ExtraHealthVtbl = 0x010158D4;
static const UInt32 s_ExtraLockVtbl = 0x0101588C;
static const UInt32 s_ExtraCountVtbl = 0x010158C8;
static const UInt32 s_ExtraTeleportVtbl = 0x01015898;
static const UInt32 s_ExtraWeaponModFlagsVtbl = 0x01015994;

#else
#error
#endif

// static
BSExtraData* BSExtraData::Create(UInt8 xType, UInt32 size, UInt32 vtbl)
{
	void* memory = FormHeap_Allocate(size);
	memset(memory, 0, size);
	((UInt32*)memory)[0] = vtbl;
	BSExtraData* xData = (BSExtraData*)memory;
	xData->type = xType;
	return xData;
}

ExtraHealth* ExtraHealth::Create() 
{
	ExtraHealth* xHealth = (ExtraHealth*)BSExtraData::Create(kExtraData_Health, sizeof(ExtraHealth), s_ExtraHealthVtbl);
	return xHealth;
}

ExtraLock* ExtraLock::Create()
{
	ExtraLock* xLock = (ExtraLock*)BSExtraData::Create(kExtraData_Lock, sizeof(ExtraLock), s_ExtraLockVtbl);
	ExtraLock::Data* lockData = (ExtraLock::Data*)FormHeap_Allocate(sizeof(ExtraLock::Data));
	memset(lockData, 0, sizeof(ExtraLock::Data));
	xLock->data = lockData;
	return xLock;
}

ExtraCount* ExtraCount::Create()
{
	ExtraCount* xCount = (ExtraCount*)BSExtraData::Create(kExtraData_Count, sizeof(ExtraCount), s_ExtraCountVtbl);
	xCount->count = 0;
	return xCount;
}

ExtraTeleport* ExtraTeleport::Create()
{
	ExtraTeleport* tele = (ExtraTeleport*)BSExtraData::Create(kExtraData_Teleport, sizeof(ExtraTeleport), s_ExtraTeleportVtbl);
	
	// create data
	ExtraTeleport::Data* data = (ExtraTeleport::Data*)FormHeap_Allocate(sizeof(ExtraTeleport::Data));
	data->linkedDoor = NULL;
	data->yRot = -0.0;
	data->xRot = 0.0;
	data->x = 0.0;
	data->y = 0.0;
	data->z = 0.0;
	data->zRot = 0.0;

	tele->data = data;
	return tele;
}

ExtraWeaponModFlags* ExtraWeaponModFlags::Create()
{
	ExtraWeaponModFlags* xWeaponModFlags = (ExtraWeaponModFlags*)BSExtraData::Create(kExtraData_WeaponModFlags, sizeof(ExtraWeaponModFlags), s_ExtraWeaponModFlagsVtbl);

	xWeaponModFlags->flags = 0;

	return xWeaponModFlags;
}
