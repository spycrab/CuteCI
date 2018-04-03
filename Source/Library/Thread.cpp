// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include "Thread.h"

#include <QString>
#include <QTimer>
#include <QWidget>
#include <QtGlobal>

#include "CuteCI.h"
#include "Flags.h"
#include "Log.h"
#include "Render.h"

namespace CuteCI
{
CIThread::CIThread(QObject* parent) : QThread(parent)
{
  connect(this, &QThread::finished, this, &QObject::deleteLater);
  connect(this, &QObject::destroyed, this, &QObject::deleteLater);

  // Automatically quit the application after ten seconds
  if (TestFlag(AUTO_QUIT)) {
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

          if (TestFlag(AUTO_RENDER))
            AutoRender(widget);
        }
      }

      m_widgets = now;
    }
  }
}
} // namespace CuteCI
