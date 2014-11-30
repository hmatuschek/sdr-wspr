#include "receiver.hh"
#include <QTime>

using namespace sdr;

Receiver::Receiver(double F, double IF, QObject *parent)
  : QObject(parent), _F(F), _Fcorr(0), _IF(IF), _Fbfo(1500),
    _src(_F+_IF+_Fcorr, 240000), _cast(), _baseband(0, _Fbfo, 800, 31, 1, 12e3),
    _demod(), _agc(), _wspr(_Fbfo), _audio(), _queue(Queue::get()), _monitor(true), _timer()
{
  _src.connect(&_cast, true);
  _cast.connect(&_baseband, true);
  _baseband.connect(&_demod, true);
  _demod.connect(&_agc);
  _agc.connect(&_wspr);
  if (_monitor) {
    _agc.connect(&_audio);
  }

  _queue.addStart(&_src, &RTLSource::start);
  _queue.addStop(&_src, &RTLSource::stop);

  _queue.addStart(this, &Receiver::_onQueueStart);
  _queue.addStop(this, &Receiver::_onQueueStop);

  _timer.setSingleShot(true);
  QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(_onStartRX()));

  _messages = new QStandardItemModel();
  _messages->setColumnCount(5);
  QStringList headers; headers << "Time" << "Message" << "SNR" << "Frequency" << "Delay";
  _messages->setHorizontalHeaderLabels(headers);
}

Receiver::~Receiver() {
  // pass...
}

gui::SpectrumProvider *
Receiver::spectrum() {
  return &_wspr;
}

void
Receiver::join() {
  _wspr.join();
}

double
Receiver::frequency() const {
  return _F;
}

void
Receiver::setFrequency(double F) {
  _F = F;
  _src.setFrequency(_F+_IF+_Fcorr);
  emit frequencyChanged(_F);
}

double
Receiver::freqCorrection() const {
  return _Fcorr;
}

void
Receiver::setFreqCorrection(double F) {
  _Fcorr = F;
  _src.setFrequency(_F+_IF+_Fcorr);
  emit frequencyCorrectionChanged(_Fcorr);
}

double
Receiver::intermediateFrequency() const {
  return _IF;
}

void
Receiver::setIntermediateFrequency(double F) {
  _IF = F;
  _src.setFrequency(_F+_IF+_Fcorr);
}

double
Receiver::bfoFrequency() const {
  return _Fbfo;
}

void
Receiver::setBfoFrequency(double F) {
  _Fbfo = F;
  bool isRunning = _queue.isRunning();
  if (isRunning) { _queue.stop(); _queue.wait(); }
  _baseband.setCenterFrequency(_Fbfo);
  if (isRunning) { _queue.start(); }
}

bool
Receiver::sourceAGCEnabled() const {
  return _src.agcEnabled();
}

void
Receiver::enableSourceAGC(bool enabled) {
  _src.enableAGC(enabled);
}

bool
Receiver::audioAGCEnabled() const {
  return _agc.enabled();
}

void
Receiver::enableAudioAGC(bool enabled) {
  _agc.enable(enabled);
  if (! enabled) { _agc.setGain(1); }
}

bool
Receiver::monitorEnabled() const {
  return _monitor;
}

void
Receiver::enableMonitor(bool enabled) {
  if (_monitor && !enabled) {
    // disconnect AGC & audio sink
    _agc.disconnect(&_audio);
  } else if (!_monitor && enabled) {
    _agc.connect(&_audio);
  }
}

QStandardItemModel *
Receiver::messages() {
  return _messages;
}


void
Receiver::_onQueueStart() {
  _wspr.stopRX();
  QTime t = QTime::currentTime();

  // Compute seconds til next start
  int sec = 60-t.second();
  if (0 == (t.minute() % 2)) { sec += 60; }

  LogMessage msg(LOG_DEBUG);
  msg << "Wait for " << sec << " seconds.";
  Logger::get().log(msg);

  // Start timer to trigger recording
  _timer.start(sec*1000);
}

void
Receiver::_onQueueStop() {
  _wspr.stopRX();
  LogMessage msg(LOG_DEBUG);
  msg << "RX stopped...";
  Logger::get().log(msg);
}

void
Receiver::_onStartRX() {
  _wspr.startRX();
  LogMessage msg(LOG_DEBUG);
  msg << "RX Started...";
  Logger::get().log(msg);
}

void
Receiver::_onMessagesReceived() {
  while (_wspr.messages().size()) {
    WSPRMessage msg = _wspr.messages().front(); _wspr.messages().pop_front();
    QList<QStandardItem *> row;
    row.append(new QStandardItem(QTime::currentTime().toString()));
    row.append(new QStandardItem(QString(msg.msg)));
    row.append(new QStandardItem(QString::number(msg.snr)));
    row.append(new QStandardItem(QString::number( (_F + _Fcorr + msg.df)/1e3 )));
    row.append(new QStandardItem(QString::number(msg.dt)));
    _messages->appendRow(row);
  }
}
