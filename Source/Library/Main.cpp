// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "CuteCI.h"

#include "Flags.h"
#include "Thread.h"

#include <iostream>

namespace CuteCI
{
void Init(QObject* parent, int flags)
{
  CuteCI::flags = flags;

  // Start the CuteCI thread in the background
  auto* ci_thread = new CIThread(parent);
  ci_thread->start();
}
} // namespace CuteCI
