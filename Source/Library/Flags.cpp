// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Flags.h"

namespace CuteCI
{
int flags = 0;
bool TestFlag(int flag) { return flags & flag; }
}
