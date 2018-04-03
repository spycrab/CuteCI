// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Render.h"

#include <QAbstractEventDispatcher>
#include <QApplication>
#include <QMap>
#include <QObject>
#include <QString>
#include <QWidget>

#include "Log.h"

template <typename F>
static void postToThread(F&& fun, QThread* thread = qApp->thread())
{
  QObject* obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QObject src;
  QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(fun),
                   Qt::QueuedConnection);
}

static bool IsWidgetBlacklisted(QObject* widget)
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

static int GetWidgetNumber(QObject* widget)
{
  const QString className = widget->metaObject()->className();
  static QMap<QString, int> widget_count;

  if (widget_count.count(className) == 0)
    widget_count[className] = 0;

  return widget_count[className]++;
}

static QString GetWidgetIdentifier(QObject* widget)
{
  QString name;

  for (QObject* parent = widget; parent != nullptr; parent = parent->parent()) {
    if (parent == nullptr)
      break;

    QString part;
    part.append(parent->metaObject()->className());

    if (!parent->objectName().isEmpty()) {
      part.append("(" + parent->objectName() + ")");
    } else if (!IsWidgetUnique(parent) && parent == widget) {
      part.append("No" + QString::number(GetWidgetNumber(parent)));
    }

    part.append("_");

    name.prepend(part);
  }

  name.chop(1);
  return name;
}

namespace CuteCI
{
void Render(QWidget* widget, const char* name)
{
  postToThread(
      [widget, name] {
        Log("Rendering " + QString(name));
        if (!widget->grab().save(QString(name) + ".bmp")) {
          Error("Failed to render " + QString(name));
        }
      },
      widget->thread());
}
void AutoRender(QWidget* widget)
{
  Render(widget, GetWidgetIdentifier(widget).toStdString().c_str());
}
} // namespace CuteCI
