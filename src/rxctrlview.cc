#include "rxctrlview.hh"
#include <QFormLayout>
#include <QDoubleValidator>
#include <QGroupBox>


RXCtrlView::RXCtrlView(Receiver *rx, QWidget *parent)
  : QWidget(parent), _rx(rx)
{
  _band = new QComboBox();
  _band->addItem("160m", 1836.6);
  _band->addItem("80m", 3592.6);
  _band->addItem("60m", 5287.2);
  _band->addItem("40m", 7038.6);
  _band->addItem("30m", 10138.7);
  _band->addItem("20m", 14095.6);
  _band->addItem("17m", 18104.6);
  _band->addItem("15m", 21094.6);
  _band->addItem("12m", 24924.6);
  _band->addItem("10m", 28124.6);
  _band->addItem("6m", 50293.00);
  _band->addItem("2m", 144488.5);
  _band->setCurrentIndex(0);

  double F = _band->currentData().toDouble()*1000;
  _rx->setFrequency(F);

  _freq = new QLineEdit(QString::number((_rx->frequency()+_rx->freqCorrection())/1e3));
  _freq->setEnabled(false);

  _Fcorr = new QLineEdit(QString::number(_rx->freqCorrection()));
  QDoubleValidator *fval = new QDoubleValidator();
  _Fcorr->setValidator(fval);

  _Fbfo = new QLineEdit(QString::number(_rx->bfoFrequency()));
  _Fbfo->setEnabled(false);

  _rtl_agc = new QCheckBox();
  _rtl_agc->setChecked(_rx->sourceAGCEnabled());

  _audio_agc = new QCheckBox();
  _audio_agc->setChecked(_rx->audioAGCEnabled());

  _monitor = new QCheckBox();
  _monitor->setChecked(_rx->monitorEnabled());

  _log = new QTableView();
  _log->setModel(_rx->messages());

  QGroupBox *rx_box = new QGroupBox("Receiver");
  QFormLayout *rx_layout = new QFormLayout();
  rx_layout->addRow("Band", _band);
  rx_layout->addRow("Freq. (kHz)", _freq);
  rx_layout->addRow("Freq. cor. (Hz)", _Fcorr);
  rx_layout->addRow("BFO (Hz)", _Fbfo);
  rx_layout->addRow("AGC", _rtl_agc);
  rx_box->setLayout(rx_layout);

  QGroupBox *cfg_box = new QGroupBox("Settings");
  QFormLayout *cfg_layout = new QFormLayout();
  cfg_layout->addRow("Audio AGC", _audio_agc);
  cfg_layout->addRow("Monitor", _monitor);
  cfg_box->setLayout(cfg_layout);

  QHBoxLayout *layout1 = new QHBoxLayout();
  layout1->addWidget(rx_box);
  layout1->addWidget(cfg_box);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(layout1);
  layout->addWidget(_log);
  this->setLayout(layout);

  QObject::connect(_band, SIGNAL(currentIndexChanged(int)), this, SLOT(onBandSelected(int)));
  QObject::connect(_Fcorr, SIGNAL(editingFinished()), this, SLOT(onFreqCorrEdited()));
  QObject::connect(_rx, SIGNAL(frequencyChanged(double)), this, SLOT(onRXFreqChanged(double)));
  QObject::connect(_rx, SIGNAL(frequencyCorrectionChanged(double)), this, SLOT(onRXFreqCorrChanged(double)));
  QObject::connect(_rtl_agc, SIGNAL(toggled(bool)), this, SLOT(onSrcAGCToggled(bool)));
  QObject::connect(_audio_agc, SIGNAL(toggled(bool)), this, SLOT(onAudioAGCToggled(bool)));
  QObject::connect(_monitor, SIGNAL(toggled(bool)), this, SLOT(onMonitorToggled(bool)));
}

void
RXCtrlView::onBandSelected(int idx) {
  double F = _band->currentData().toDouble()*1e3;
  _rx->setFrequency(F);
}

void
RXCtrlView::onRXFreqChanged(double F) {
  // Update frequency display
  _freq->setText(QString::number((_rx->frequency()+_rx->freqCorrection())/1e3));
}

void
RXCtrlView::onRXFreqCorrChanged(double F) {
  // Update frequency display
  _freq->setText(QString::number((_rx->frequency()+_rx->freqCorrection())/1e3));
  _Fcorr->setText(QString::number(_rx->freqCorrection()));
}

void
RXCtrlView::onFreqCorrEdited() {
  double Fcorr = _Fcorr->text().toDouble();
  _rx->setFreqCorrection(Fcorr);
}

void
RXCtrlView::onSrcAGCToggled(bool enabled) {
  _rx->enableSourceAGC(enabled);
}

void
RXCtrlView::onAudioAGCToggled(bool enabled) {
  _rx->enableAudioAGC(enabled);
}

void
RXCtrlView::onMonitorToggled(bool enabled) {
  _rx->enableMonitor(enabled);
}
