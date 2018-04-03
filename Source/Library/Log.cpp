// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Log.h"

#include <QDebug>

#include "CuteCI.h"
#include "Flags.h"

static void WriteQDebug(const QString msg)
{
  if (!TestFlag(CuteCI::DISABLE_LOGGING))
    qDebug().noquote() << msg;
}

namespace CuteCI
{
void Log(const QString msg) { WriteQDebug("[LOG] " + msg); }
void Warn(const QString msg) { WriteQDebug("[LOG] " + msg); }
void Error(const QString msg) { WriteQDebug("[LOG] " + msg); }
}
