#include "GameObjects.h"
#include "GameRTTI.h"
#include "GameExtraData.h"

ScriptEventList* TESObjectREFR::GetEventList() const
{
	BSExtraData* xData = extraDataList.GetByType(kExtraData_Script);
	if (xData)
	{
		ExtraScript* xScript = DYNAMIC_CAST(xData, BSExtraData, ExtraScript);
		if (xScript)
			return xScript->eventList;
	}

	return 0;
}

static PlayerCharacter** g_thePlayer = (PlayerCharacter **)0x011DEA3C;

PlayerCharacter* PlayerCharacter::GetSingleton()
{
	return *g_thePlayer;
}
