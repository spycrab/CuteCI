// Copyright 2018 spycrab
// Licensed under GPLv3
// Refer to the LICENSE.txt file included.

#include <QApplication>
#include <QThread>

namespace CuteCI
{
class CIThread : public QThread
{
  Q_OBJECT
public:
  explicit CIThread(QObject* parent);
  ~CIThread();
  QWidgetList m_widgets;

  void run() override;
};
} // namespace CuteCI
