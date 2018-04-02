// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#ifndef CUTECI_H
#define CUTECI_H

class QObject;

#ifdef CUTECI
#define CUTECI_INIT(flags) CuteCI::Init(flags);
#else
#define CUTECI_INIT(flags)
#endif

namespace CuteCI
{
enum Flags {
  DISABLE_LOGGING = 1 << 0,
  TAKE_SCREENSHOTS = 1 << 2,
  AUTO_QUIT = 1 << 4
};

void Init(QObject* parent, int flags = TAKE_SCREENSHOTS | AUTO_QUIT);

extern int flags;
} // namespace CuteCI

#endif
