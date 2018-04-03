// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#ifndef CUTECI_H
#define CUTECI_H

#include <QWidget>

#define CUTECI_DEFAULT_FLAGS CuteCI::AUTO_RENDER | CuteCI::AUTO_QUIT

#ifdef CUTECI
#define CUTECI_INIT(parent, flags) CuteCI::Init(parent, flags);
#define CUTECI_RENDER(widget, name)                                            \
  [&]() -> decltype(widget) {                                                  \
    CuteCI::Render(widget, name);                                              \
    return widget;                                                             \
  }()
#else
#define CUTECI_INIT(parent, flags)
#define CUTECI_RENDER(widget, name) widget
#endif

namespace CuteCI
{
enum Flags {
  DISABLE_LOGGING = 1 << 0,
  AUTO_RENDER = 1 << 2,
  AUTO_QUIT = 1 << 4
};

void Init(QObject* parent, int flags);
void Render(QWidget* widget, const char* name);
} // namespace CuteCI

#endif
