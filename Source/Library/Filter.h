// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#pragma once

class QObject;

namespace CuteCI
{
namespace Filter
{
bool IsWidgetBlacklisted(QObject* widget);
bool IsWidgetUnique(QObject* widget);
int GetWidgetNumber(QObject* widget);
} // namespace Filter
} // namespace CuteCI
