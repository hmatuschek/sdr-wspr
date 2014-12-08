#ifndef RXCTRLVIEW_HH
#define RXCTRLVIEW_HH

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QTableView>
#include <QVBoxLayout>

#include "receiver.hh"


class RXCtrlView : public QWidget
{
  Q_OBJECT

public:
  explicit RXCtrlView(Receiver *rx, QWidget *parent = 0);

protected slots:
  void onSourceSelected(int idx);
  void onQTHChanged();
  void onBandSelected(int idx);
  void onRXFreqChanged(double F);
  void onBfoFrequencyChanged();

  void onAudioAGCToggled(bool enabled);
  void onMonitorToggled(bool enabled);

protected:
  Receiver *_rx;

  QVBoxLayout *_rx_layout;
  QComboBox *_input;

  QLineEdit *_qth;
  QComboBox *_band;
  QLineEdit *_freq;
  QLineEdit *_Fbfo;

  QCheckBox *_audio_agc;
  QCheckBox *_monitor;
};

#endif // RXCTRLVIEW_HH
