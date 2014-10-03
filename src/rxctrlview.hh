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
  void onBandSelected(int idx);
  void onRXFreqChanged(double F);
  void onRXFreqCorrChanged(double F);
  void onFreqCorrEdited();
  void onSrcAGCToggled(bool enabled);
  void onAudioAGCToggled(bool enabled);
  void onMonitorToggled(bool enabled);

protected:
  Receiver *_rx;

  QComboBox *_band;
  QLineEdit *_freq;
  QLineEdit *_Fcorr;
  QLineEdit *_Fbfo;
  QCheckBox *_rtl_agc;

  QCheckBox *_audio_agc;
  QCheckBox *_monitor;
  QTableView *_log;
};

#endif // RXCTRLVIEW_HH
