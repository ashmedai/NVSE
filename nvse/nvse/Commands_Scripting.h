#pragma once

#include "CommandTable.h"
#include "ParamInfos.h"

DEFINE_COMMAND(Label, set a label, 0, 1, kParams_OneOptionalInt);
DEFINE_COMMAND(Goto, branch to a label, 0, 1, kParams_OneOptionalInt);
