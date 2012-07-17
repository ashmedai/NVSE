#include "nvse_extender.h"

#include "SafeWrite.h"
#include "nvse/GameAPI.h"
#include "nvse/ParamInfos.h"
#include "nvse/GameObjects.h"
#include "nvse/GameRTTI.h"
#include <string>
#include <unordered_map>
#include <utility>

typedef std::unordered_map<UInt32, std::unordered_map<std::string, float>> NVSE_EXEVRefFloatMap;
typedef std::unordered_map<UInt32, std::unordered_map<std::string, UInt32>> NVSE_EXEVRefFormMap;

NVSE_EXEVRefFloatMap nvse_ex_evfloatmap;
NVSE_EXEVRefFormMap nvse_ex_evformmap;

PluginHandle	g_pluginHandle = kPluginHandle_Invalid;
//This only works on 1.4.0.518
NVSEMessagingInterface* g_msg;

void MessageHandler(NVSEMessagingInterface::Message* msg)
{
	switch (msg->type)
	{
	case NVSEMessagingInterface::kMessage_LoadGame:
		_MESSAGE("Plugin Example received load message with file path %s", msg->data);
		break;
	case NVSEMessagingInterface::kMessage_SaveGame:
		_MESSAGE("Plugin Example received save message with file path %s", msg->data);
		break;
	default:
		_MESSAGE("Plugin Example received unknown message");
		break;
	}
}


bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info)
{
  info->infoVersion = PluginInfo::kInfoVersion;
  info->name        = NVSE_EXTENDER_NAME;
  info->version     = NVSE_EXTENDER_VER;
  if (nvse->isEditor ) 
  {
	  if ( nvse->editorVersion != 0x04002060) {
		_MESSAGE("GECK version not supported: %d",  nvse->editorVersion);
		return false;
	  } else {
		_MESSAGE("loading extender in editor 1_4_0_518 (%d)",  nvse->editorVersion);
	  }
  }
  else
  {
	  if (nvse->runtimeVersion != 0x040020D0)
	  {
		_MESSAGE("loading extender in unsupported runtime %d",  nvse->runtimeVersion);
		return false;
	  } else {
		_MESSAGE("loading extender in runtime 1_4_0_525 (%d)",  nvse->runtimeVersion);
	  }
  }
  return true;
}

bool NVSEPlugin_Load(const NVSEInterface * nvse)
{
	_MESSAGE("extender plugin load editor: %d", nvse->isEditor);
  g_pluginHandle = nvse->GetPluginHandle();

  nvse->SetOpcodeBase(NVSE_EXTENDER_OPCODE_BASE);

  /* START published plugins, do not change order */
  nvse->RegisterCommand(&kCommandInfo_NX_GetVersion);
  nvse->RegisterCommand(&kCommandInfo_NX_IsUsingSkeleton);
  nvse->RegisterCommand(&kCommandInfo_NX_IsInList);
  nvse->RegisterCommand(&kCommandInfo_NX_SetEVFl);
  nvse->RegisterCommand(&kCommandInfo_NX_GetEVFl);
  nvse->RegisterCommand(&kCommandInfo_NX_SetEVFo);
  nvse->RegisterCommand(&kCommandInfo_NX_GetEVFo);
  /* END published plugins, do not change order */


// Cannot find the flag for these
//  nvse->RegisterCommand(&kCommandInfo_NX_CanEquipItem);
//  nvse->RegisterCommand(&kCommandInfo_NX_CanUnequipItem);


//  nvse->RegisterCommand(&kCommandInfo_NX_ChangeRace);
//  nvse->RegisterCommand(&kCommandInfo_NX_IsFIKEnabled);
//  nvse->RegisterCommand(&kCommandInfo_NX_TFIK);
//  nvse->RegisterCommand(&kCommandInfo_NX_IsUFOEnabled);
//  nvse->RegisterCommand(&kCommandInfo_NX_RunBatchScript);

	// register to receive messages from OBSE
	NVSEMessagingInterface* msgIntfc = (NVSEMessagingInterface*)nvse->QueryInterface(kInterface_Messaging);
	msgIntfc->RegisterListener(g_pluginHandle, "NVSE", MessageHandler);
	g_msg = msgIntfc;

  _MESSAGE("NVSEEx ready");
  return true;
}

// Implemented in-use functions
bool Cmd_NX_GetVersion_Execute(COMMAND_ARGS)
{
  *result = NVSE_EXTENDER_VER;
  if (IsConsoleMode())
  {
      Console_Print("SEXOUT NVSE version: %d", NVSE_EXTENDER_VER);
  }
  return true;
}

bool Cmd_NX_IsUsingSkeleton_Execute(COMMAND_ARGS)
{
  TESActorBase *pActor;
  char skelName[512];
  char actorModel[512];
  char *pch    = NULL;
  char *dummy = NULL;
  char **parts = NULL;
  int  cnt     = 0;
  int  idx;

  _MESSAGE("START IsUsingSkeleton");

  *result = 0;

  if (ExtractArgs(EXTRACT_ARGS, &skelName))
  {
    pActor = DYNAMIC_CAST(thisObj->baseForm, TESForm, TESActorBase);
    if (pActor)
    {
      strcpy_s(actorModel, 512, pActor->model.GetPath());
      pch = strtok_s(actorModel, "/\\", &dummy);
      while (NULL != pch)
      {
        parts = (char **) realloc(parts, (cnt + 1) * (sizeof(char*)));
        parts[cnt] = pch;
        cnt++;
        pch = strtok_s(NULL, "/\\", &dummy);
      }

      for (idx = 0; idx < cnt; idx++)
      {
        _MESSAGE("IDX %d is %s", idx, parts[idx]);
      }

      if (cnt >= 2)
      {
        _MESSAGE("SIC %s %s is %d", skelName, parts[cnt - 2], _stricmp(skelName, parts[cnt - 2]));

        if (0 == _stricmp(skelName, parts[cnt - 2]))
        {
          *result = 1;
        }
      }

      // Free our memory
      free(parts);

      if (IsConsoleMode())
      {
        Console_Print("IsUsingSkeleton %.0f", *result);
      }
    }
  }

  _MESSAGE("END IsUsingSkeleton %0.0f", *result);
  return true;
}

TESCreature* GetRootCreature(TESCreature *cre)
{
  _MESSAGE("GetRootCreature %x", cre->refID);

  while ((GetObjectClassName(cre) != "<no rtti>") && (NULL != cre->creatureTemplate))
  {
    if (GetObjectClassName(cre->creatureTemplate) == "<no rtti>")
      break;

    _MESSAGE("  %x (%s) -> %x (%s)", cre->refID, GetObjectClassName(cre), cre->creatureTemplate->refID, GetObjectClassName(cre->creatureTemplate));
    cre = cre->creatureTemplate;
  }

  return cre;
}


bool Cmd_NX_IsInList_Execute(COMMAND_ARGS)
{
  BGSListForm  *pFormList     = NULL; // the formlist
  TESForm      *pItem         = NULL; // item we are looking for
  TESForm      *pListItem     = NULL; // current item from list
  TESForm      *pBaseItem     = NULL; // item we are looking for
  TESForm      *pBaseListItem = NULL; // current item from list

  // need to handle creatures in a special fashion
  TESCreature  *pCreature   = NULL;
  TESCreature  *pLCreature  = NULL;
  TESCreature  *pCreatureB  = NULL;
  TESCreature  *pLCreatureB = NULL;

  UInt32       n          = 0;    // do not do recursion
  int i;

  _MESSAGE("START IsInList");
  *result = 0;

  ExtractArgs(EXTRACT_ARGS, &pFormList, &pItem, &n);
  if (pFormList && pItem)
  {
    pBaseItem = pItem->TryGetREFRParent();
    pCreature = DYNAMIC_CAST(pBaseItem, TESForm, TESCreature);
    
    _MESSAGE("  Item %s (%x) p %s (%x)", GetObjectClassName(pItem), pItem->refID, GetObjectClassName(pBaseItem), pBaseItem->refID);
    if (NULL != pCreature)
    {
      _MESSAGE("  Item is a creature (%s)", pCreature->fullName.name);
    }

    for (i = 0; i < pFormList->Count(); i++)
    {
      pListItem = pFormList->GetNthForm(i);
      if (pListItem)
      {
        if (NULL != pCreature)
        {
          pCreatureB = GetRootCreature(pCreature);
          pLCreature = DYNAMIC_CAST(pListItem, TESForm, TESCreature);
          if (NULL != pLCreature)
          {
            pLCreatureB = GetRootCreature(pLCreature);
            if (pCreature->refID == pLCreature->refID)
            {
              *result = 1;
              break;
            }
            else if (pCreatureB->refID == pLCreature->refID)
            {
              *result = 2;
              break;
            }
            else if (pCreature->refID == pLCreatureB->refID)
            {
              *result = 3;
              break;
            }
            else if (pCreatureB->refID == pLCreatureB->refID)
            {
              *result = 4;
              break;
            }
          }
        }
        else
        {
          pBaseListItem = pListItem->TryGetREFRParent();

          if (pItem->refID == pListItem->refID)
          {
            *result = 1;
            break;
          }
          else if (pBaseItem->refID == pListItem->refID)
          {
            *result = 2;
            break;
          }
          else if (pItem->refID == pBaseListItem->refID)
          {
            *result = 3;
            break;
          }
          else if (pBaseItem->refID == pBaseListItem->refID)
          {
            *result = 4;
            break;
          }
        }
      }
    }

    _MESSAGE("  IsInList Result %1.0f", *result);
  }
  else
  {
    if (!pFormList)
    {
      _MESSAGE(" invalid formlist arg");
    }

    if (!pItem)
    {
      _MESSAGE(" invalid item arg");
    }
  }
  _MESSAGE("END IsInList");
  return true;
}

// TBD functions

bool Cmd_NX_ChangeRace_Execute(COMMAND_ARGS)
{
  UInt32        *refResult = NULL;
  Actor         *pActor    = NULL; // item we are looking for
  TESRace       *pRace     = NULL; // current item from list

  _MESSAGE("START ChangeRace");
//  refResult = (UInt32*)result;

  if (ExtractArgs(EXTRACT_ARGS, &pActor, &pRace))
  {
    _MESSAGE("  SUCCESS %x %x %x", pActor, pRace, pActor->GetTypeID());
    if (pActor)
    {
//      _MESSAGE("    Actor is a Character");
      // needs a TESRaceForm not TESRace
//      pActor->race.race = *pRace;
    }
  }
  else
  {
    _MESSAGE("  FAIL");
  }

  _MESSAGE("END ChangeRace");
  return true;
}

bool Cmd_NX_SetEVFl_Execute(COMMAND_ARGS)
{
  std::string key;
  char keyName[512];
  float fValue = 0;
  UInt32 iPersist = 0;

  _MESSAGE("START SetEVFl");
  if (ExtractArgs(EXTRACT_ARGS, &keyName, &fValue, &iPersist))
  {
    if (thisObj)
    {
      key = keyName;
      nvse_ex_evfloatmap[thisObj->refID][key] = fValue;
    }
  }

  _MESSAGE("END SetEVFl");
  return true;
}

bool Cmd_NX_GetEVFl_Execute(COMMAND_ARGS)
{
  std::string key;
  char keyName[512];
  float fValue = 0;
  UInt32 iPersist = 0;

  _MESSAGE("START GetEVFl");
  *result = 0;

  if (ExtractArgs(EXTRACT_ARGS, &keyName))
  {
    if (thisObj)
    {
      key = keyName;
      *result = nvse_ex_evfloatmap[thisObj->refID][key];
      if (IsConsoleMode())
      {
        Console_Print("GetEVFl: %.5f", *result);
      }
    }
  }

  _MESSAGE("END GetEVFl");

  return true;
}

bool Cmd_NX_SetEVFlEx_Execute(COMMAND_ARGS)
{
  std::string key;
  char buffer[kMaxMessageLength];

  *result = 0;

  if (ExtractFormatStringArgs(0, buffer, paramInfo, scriptData, opcodeOffsetPtr, scriptObj, eventList, 19))
  {
  }

  _MESSAGE("START SetEVFlEx");

  _MESSAGE("END SetEVFlEx");
  return true;
}

bool Cmd_NX_SetEVFo_Execute(COMMAND_ARGS)
{
  std::string key;
  char keyName[512];
  TESForm *fValue = 0;
  UInt32 iPersist = 0;

  _MESSAGE("START SetEVFo");
  if (ExtractArgs(EXTRACT_ARGS, &keyName, &fValue, &iPersist))
  {
    if (thisObj)
    {
      key = keyName;
      nvse_ex_evformmap[thisObj->refID][key] = fValue->refID;
    }
  }

  _MESSAGE("END SetEVFo");
  return true;
}

bool Cmd_NX_GetEVFo_Execute(COMMAND_ARGS)
{
  std::string key;
  char keyName[512];
  float fValue = 0;
  UInt32 iPersist = 0;

  _MESSAGE("START GetEVFo");
  *result = 0;

  if (ExtractArgs(EXTRACT_ARGS, &keyName))
  {
    if (thisObj)
    {
      key = keyName;
      *((UInt32 *)result) = nvse_ex_evformmap[thisObj->refID][key];
      if (IsConsoleMode())
      {
        Console_Print("GetEVFo: %x", *((UInt32 *)result));
      }
    }
  }

  _MESSAGE("END GetEVFo");

  return true;
}

bool Cmd_NX_ClrEVFl_Execute(COMMAND_ARGS)
{
  std::string key = "";
  char keyName[512];
  int doMatch = 1;

  _MESSAGE("START ClrEVFl");
  *result = 0;

  if (ExtractArgs(EXTRACT_ARGS, &keyName, &doMatch))
  {
    key = keyName;
  }

  if (thisObj)
  {
    if (key != "")
    {
      nvse_ex_evfloatmap[thisObj->refID].erase(key);
    }
    else
    {
      nvse_ex_evfloatmap.clear();
    }
  }
  _MESSAGE("END ClrEVFl");
  return true;
}

bool Cmd_NX_ClrEVFo_Execute(COMMAND_ARGS)
{
  std::string key = "";
  char keyName[512];
  int doMatch = 1;

  _MESSAGE("START ClrEVFl");
  *result = 0;

  if (ExtractArgs(EXTRACT_ARGS, &keyName, &doMatch))
  {
    key = keyName;
  }

  if (thisObj)
  {
    if (key != "")
    {
      nvse_ex_evformmap[thisObj->refID].erase(key);
    }
    else
    {
      nvse_ex_evfloatmap.clear();
    }
  }
  _MESSAGE("END ClrEVFl");
  return true;
}
