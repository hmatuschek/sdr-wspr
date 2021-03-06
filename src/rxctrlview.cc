#include "rxctrlview.hh"
#include <QFormLayout>
#include <QDoubleValidator>
#include <QGroupBox>
#include <QDoubleValidator>


RXCtrlView::RXCtrlView(Receiver *rx, QWidget *parent)
  : QWidget(parent), _rx(rx)
{
  _startStop = new QPushButton();
  _startStop->setCheckable(true);
  if (sdr::Queue::get().isRunning()) {
    _startStop->setChecked(true);
    _startStop->setText("Stop");
  } else {
    _startStop->setChecked(false);
    _startStop->setText("Start");
  }
  sdr::Queue::get().addStart(this, &RXCtrlView::onQueueStart);
  sdr::Queue::get().addStop(this, &RXCtrlView::onQueueStop);

  _input = new QComboBox();
  _input->addItem("Audio", Receiver::AUDIO_SOURCE);
  _input->addItem("RTL2832", Receiver::RTL_SOURCE);
  _input->addItem("File", Receiver::FILE_SOURCE);
  switch (_rx->sourceType()) {
  case Receiver::AUDIO_SOURCE: _input->setCurrentIndex(0); break;
  case Receiver::RTL_SOURCE: _input->setCurrentIndex(1); break;
  case Receiver::FILE_SOURCE: _input->setCurrentIndex(2); break;
  }

  _qth = new QLineEdit(_rx->locator());

  _band = new QComboBox();
  _band->addItem("160m", 1838.1);
  _band->addItem("80m", 3594.1);
  _band->addItem("60m", 5288.7);
  _band->addItem("40m", 7040.1);
  _band->addItem("30m", 10140.2);
  _band->addItem("20m", 14097.1);
  _band->addItem("17m", 18106.1);
  _band->addItem("15m", 21096.1);
  _band->addItem("12m", 24926.1);
  _band->addItem("10m", 28126.1);
  _band->addItem("6m", 50294.50);
  _band->addItem("2m", 144490.1);
  _band->setCurrentIndex(0);

  double F = _band->itemData(0).toDouble()*1000;
  _rx->setFrequency(F);

  _freq = new QLineEdit(QString::number(_rx->frequency()/1e3));
  _freq->setEnabled(false);

  _Fbfo = new QLineEdit(QString::number(_rx->bfoFrequency()));
  _Fbfo->setEnabled(true);
  QDoubleValidator *bfo_val = new QDoubleValidator(100, 6e3, 1);
  _Fbfo->setValidator(bfo_val);

  _audio_agc = new QCheckBox();
  _audio_agc->setChecked(_rx->audioAGCEnabled());

  _monitor = new QCheckBox();
  _monitor->setChecked(_rx->monitorEnabled());

  QGroupBox *cfg_box = new QGroupBox("Receiver Settings");
  QFormLayout *cfg_layout = new QFormLayout();
  cfg_layout->addRow("Band", _band);
  cfg_layout->addRow("Freq. (kHz)", _freq);
  cfg_layout->addRow("BFO (Hz)", _Fbfo);
  cfg_layout->addRow("Audio AGC", _audio_agc);
  cfg_layout->addRow("Monitor", _monitor);
  cfg_layout->addRow("QTH", _qth);
  cfg_box->setLayout(cfg_layout);

  QGroupBox *rx_box = new QGroupBox("Source Settings");
  _rx_layout = new QVBoxLayout();
  _rx_layout->addWidget(_input);
  _rx_layout->addWidget(_rx->createSourceControl());
  rx_box->setLayout(_rx_layout);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addWidget(_startStop);
  layout->addWidget(rx_box);
  layout->addWidget(cfg_box);
  this->setLayout(layout);

  QObject::connect(_input, SIGNAL(currentIndexChanged(int)), this, SLOT(onSourceSelected(int)));
  QObject::connect(_qth, SIGNAL(returnPressed()), this, SLOT(onQTHChanged()));
  QObject::connect(_band, SIGNAL(currentIndexChanged(int)), this, SLOT(onBandSelected(int)));
  QObject::connect(_Fbfo, SIGNAL(editingFinished()), this, SLOT(onBfoFrequencyChanged()));
  QObject::connect(_audio_agc, SIGNAL(toggled(bool)), this, SLOT(onAudioAGCToggled(bool)));
  QObject::connect(_monitor, SIGNAL(toggled(bool)), this, SLOT(onMonitorToggled(bool)));
  QObject::connect(_startStop, SIGNAL(toggled(bool)), this, SLOT(onStartStopToggled(bool)));
}

void
RXCtrlView::onStartStopToggled(bool enabled) {
  if (enabled) {
    sdr::Queue::get().start();
    _startStop->setText("Stop");
  } else {
    sdr::Queue::get().stop();
    sdr::Queue::get().wait();
    _startStop->setText("Start");
  }
}

void
RXCtrlView::onSourceSelected(int idx) {
  Receiver::SourceType type = Receiver::SourceType(_input->itemData(idx).toUInt());
  _rx->setSourceType(type);
  QWidget *widget = _rx->createSourceControl();
  _rx_layout->addWidget(widget);
}

void
RXCtrlView::onQTHChanged() {
  _rx->setLocator(_qth->text().simplified());
}

void
RXCtrlView::onBandSelected(int idx) {
  bool ok;
  double F = _band->itemData(idx).toDouble(&ok);
  if (ok) {
    _rx->setFrequency(F*1000);
    _freq->setText(QString::number(F));
  }
}

void
RXCtrlView::onRXFreqChanged(double F) {
  _rx->setFrequency(F);
}

void
RXCtrlView::onBfoFrequencyChanged() {
  bool ok;
  double F = _Fbfo->text().toDouble(&ok);
  if (ok) { _rx->setBfoFrequency(F); };
}


void
RXCtrlView::onAudioAGCToggled(bool enabled) {
  _rx->enableAudioAGC(enabled);
}

void
RXCtrlView::onMonitorToggled(bool enabled) {
  _rx->enableMonitor(enabled);
}

void
RXCtrlView::onQueueStart() {
  _startStop->setChecked(true);
  _startStop->setText("Stop");
}

void
RXCtrlView::onQueueStop() {
  _startStop->setChecked(false);
  _startStop->setText("Start");
}
