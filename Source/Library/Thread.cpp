// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Thread.h"

#include <QAbstractEventDispatcher>
#include <QDebug>
#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

#include "CuteCI.h"
#include "Filter.h"
#include "Flags.h"

template <typename F>
static void postToThread(F&& fun, QThread* thread = qApp->thread())
{
  QObject* obj = QAbstractEventDispatcher::instance(thread);
  Q_ASSERT(obj);
  QObject src;
  QObject::connect(&src, &QObject::destroyed, obj, std::forward<F>(fun),
                   Qt::QueuedConnection);
}

namespace CuteCI
{
void Log(const QString msg)
{
  if (!(flags & DISABLE_LOGGING))
    qDebug().noquote() << "[LOG]" << msg;
}

CIThread::CIThread(QObject* parent) : QThread(parent)
{
  connect(this, &QThread::finished, this, &QObject::deleteLater);
  connect(this, &QObject::destroyed, this, &QObject::deleteLater);

  // Automatically quit the application after ten seconds
  if (flags & AUTO_QUIT) {
    auto* timer = new QTimer(this);

    connect(timer, &QTimer::timeout, this, [] { QApplication::quit(); });

    timer->setSingleShot(true);
    timer->start(1000 * 10);
  }
}

CIThread::~CIThread()
{
  quit();
  requestInterruption();
  wait();
}

QString GetWidgetIdentifier(QObject* widget)
{
  QString name;

  for (QObject* parent = widget; parent != nullptr; parent = parent->parent()) {
    if (parent == nullptr)
      break;

    QString part;
    part.append(parent->metaObject()->className());

    if (!parent->objectName().isEmpty()) {
      part.append("(" + parent->objectName() + ")");
    } else if (!Filter::IsWidgetUnique(parent) && parent == widget) {
      part.append("No" + QString::number(Filter::GetWidgetNumber(parent)));
    }

    part.append("_");

    name.prepend(part);
  }

  name.chop(1);
  return name;
}

void CIThread::run()
{
  Log("CuteCI engaged (Qt " + QString(qVersion()) + ")");

  while (!isInterruptionRequested()) {
    // Check for updated widgets
    auto now = QApplication::allWidgets();
    if (now != m_widgets) {
      for (auto* widget : now) {

        if (m_widgets.indexOf(widget) == -1) {
          Log("New " + QString(widget->metaObject()->className()));

          if ((flags & TAKE_SCREENSHOTS) &&
              !Filter::IsWidgetBlacklisted(widget)) {
            postToThread(
                [widget] {
                  Log("Taking screenshot of " + GetWidgetIdentifier(widget));
                  if (!widget->grab().save(GetWidgetIdentifier(widget) +
                                           ".bmp")) {
                    Log("Failed to save screenshot :/");
                  }
                },
                widget->thread());
          }
        }
      }

      m_widgets = now;
    }
  }
}
} // namespace CuteCI
