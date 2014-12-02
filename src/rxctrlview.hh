#ifndef RXCTRLVIEW_HH
#define RXCTRLVIEW_HH

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTableView>

#include "receiver.hh"


class RXCtrlView : public QWidget
{
  Q_OBJECT

public:
  explicit RXCtrlView(Receiver *rx, QWidget *parent = 0);

protected slots:
  void onAudioAGCToggled(bool enabled);
  void onMonitorToggled(bool enabled);

protected:
  Receiver *_rx;


  QCheckBox *_audio_agc;
  QCheckBox *_monitor;
  QTableView *_log;
};

#endif // RXCTRLVIEW_HH
