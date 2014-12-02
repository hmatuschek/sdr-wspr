#include "rxctrlview.hh"
#include <QFormLayout>
#include <QDoubleValidator>
#include <QGroupBox>


RXCtrlView::RXCtrlView(Receiver *rx, QWidget *parent)
  : QWidget(parent), _rx(rx)
{
  _audio_agc = new QCheckBox();
  _audio_agc->setChecked(_rx->audioAGCEnabled());

  _monitor = new QCheckBox();
  _monitor->setChecked(_rx->monitorEnabled());

  _log = new QTableView();
  _log->setModel(_rx->messages());

  QGroupBox *cfg_box = new QGroupBox("Settings");
  QFormLayout *cfg_layout = new QFormLayout();
  cfg_layout->addRow("Audio AGC", _audio_agc);
  cfg_layout->addRow("Monitor", _monitor);
  cfg_box->setLayout(cfg_layout);

  QHBoxLayout *layout1 = new QHBoxLayout();
  layout1->addWidget(_rx->createSourceControl());
  layout1->addWidget(cfg_box);

  QVBoxLayout *layout = new QVBoxLayout();
  layout->addLayout(layout1);
  layout->addWidget(_log);
  this->setLayout(layout);

  QObject::connect(_audio_agc, SIGNAL(toggled(bool)), this, SLOT(onAudioAGCToggled(bool)));
  QObject::connect(_monitor, SIGNAL(toggled(bool)), this, SLOT(onMonitorToggled(bool)));
}

void
RXCtrlView::onAudioAGCToggled(bool enabled) {
  _rx->enableAudioAGC(enabled);
}

void
RXCtrlView::onMonitorToggled(bool enabled) {
  _rx->enableMonitor(enabled);
}
