// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Filter.h"

#include <QMap>
#include <QObject>
#include <QString>

namespace CuteCI
{
namespace Filter
{
bool IsWidgetBlacklisted(QObject* widget)
{
  const QString className = widget->metaObject()->className();

  // These widgets provide access to the desktop, so we ignore them
  if (className == "QDesktopWidget")
    return true;
  if (className == "QDesktopScreenWidget")
    return true;

  return false;
}

bool IsWidgetUnique(QObject* widget)
{
  const QString className = widget->metaObject()->className();

  if (className == "QMainWindow")
    return true;
  // We can assume that QMenuBar / QToolBar can be identified using their parent
  if (className == "QMenuBar" || className == "QToolBar")
    return true;

  return false;
}

int GetWidgetNumber(QObject* widget)
{
  const QString className = widget->metaObject()->className();
  static QMap<QString, int> widget_count;

  if (widget_count.count(className) == 0)
    widget_count[className] = 0;

  return widget_count[className]++;
}

} // namespace Filter
} // namespace CuteCI
