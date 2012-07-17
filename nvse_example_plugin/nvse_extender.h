#ifndef __NVSE_EXTENDER_H__
#define __NVSE_EXTENDER_H__

#define NVSE_EXTENDER_NAME "Extensions for NVSE"
#define NVSE_EXTENDER_VER 6
#define NVSE_EXTENDER_OPCODE_BASE 0x2140 // range 0x2140 - 0x21BF (

#include "nvse/PluginAPI.h"
#include "nvse/CommandTable.h"
#include "nvse/ParamInfos.h"

#ifdef __cplusplus
extern "C"
{
#endif

  bool NVSEPlugin_Query(const NVSEInterface * nvse, PluginInfo * info);
  bool NVSEPlugin_Load(const NVSEInterface * nvse);

#ifdef __cplusplus
};
#endif

bool Cmd_NX_GetVersion_Execute(COMMAND_ARGS);
bool Cmd_NX_IsUsingSkeleton_Execute(COMMAND_ARGS);
bool Cmd_NX_IsInList_Execute(COMMAND_ARGS);
bool Cmd_NX_SetEVFl_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFl_Execute(COMMAND_ARGS);
bool Cmd_NX_SetEVFo_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFo_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFlRnd_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFoRnd_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFlCnt_Execute(COMMAND_ARGS);
bool Cmd_NX_GetEVFoCnt_Execute(COMMAND_ARGS);

bool Cmd_NX_ChangeRace_Execute(COMMAND_ARGS);

/*** Command definitions ***/

// Published functions
DEFINE_COMMAND_PLUGIN(NX_GetVersion, "Returns the version of this plugin", 0, 0, NULL);
DEFINE_COMMAND_PLUGIN(NX_IsUsingSkeleton, "Returns 1 if the actor is using the given skeleton", 1, 1, kParams_OneString);

static ParamInfo kParams_NX_IsInList[3] = 
{
	{ "formlist", kParamType_FormList, 0 },
	{ "form",     kParamType_AnyForm, 0  },
	{ "recurse",  kParamType_Integer, 1  }
};

DEFINE_COMMAND_PLUGIN(NX_IsInList, "Returns 1 if the item is in the list", 0, 3, kParams_NX_IsInList);


// WIP functions
static ParamInfo kParams_NX_OneActorOneRace[2] =
{
  { "actor", kParamType_Actor, 0 },
  { "race",  kParamType_Race,  0 }
};
DEFINE_COMMAND_PLUGIN(NX_ChangeRace, "Changes to a new race and returns the old", 0, 2, kParams_NX_OneActorOneRace);

static ParamInfo kParams_NX_OneStringOneFloatOneInt[3] =
{
  { "key",     kParamType_String,  0 },
  { "val",     kParamType_Float,   0 },
  { "persist", kParamType_Integer, 1 }
};
DEFINE_COMMAND_PLUGIN(NX_SetEVFl, "Sets an extended actor float value", 1, 3, kParams_NX_OneStringOneFloatOneInt);

static ParamInfo kParams_NX_OneString[1] =
{
  { "key",     kParamType_String, 0 }
};
DEFINE_COMMAND_PLUGIN(NX_GetEVFl, "Gets an extended actor float value", 1, 1, kParams_NX_OneString);

static ParamInfo kParams_NX_VariableKey[20] =
{
  { "key", kParamType_String, 0},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "var", kParamType_Float, 1},
  { "val", kParamType_Float, 0}
};
DEFINE_COMMAND_PLUGIN(NX_SetEVFlEx, "Sets an extended actor float value", 1, 20, kParams_NX_VariableKey);

static ParamInfo kParams_NX_OneStringOneFormOneInt[3] =
{
  { "key",     kParamType_String,  0 },
  { "val",     kParamType_AnyForm, 0 },
  { "persist", kParamType_Integer, 1 }
};
DEFINE_COMMAND_PLUGIN(NX_SetEVFo, "Sets an extended actor form value", 1, 3, kParams_NX_OneStringOneFormOneInt);

DEFINE_COMMAND_PLUGIN(NX_GetEVFo, "Gets an extended actor form value", 1, 1, kParams_NX_OneString);

static ParamInfo kParams_NX_OneOptionalStringOptionalInt[2] =
{
  { "key",   kParamType_String, 1 },
  { "match", kParamType_Integer, 1 }
};
DEFINE_COMMAND_PLUGIN(NX_ClrEVFl, "Clears an EV entry or the whole array", 1, 2, kParams_NX_OneOptionalStringOptionalInt);
DEFINE_COMMAND_PLUGIN(NX_ClrEVFo, "Clears an EV entry or the whole array", 1, 2, kParams_NX_OneOptionalStringOptionalInt);


#endif