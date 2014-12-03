#include "receiver.hh"
#include <QTime>

using namespace sdr;

Receiver::Receiver(SourceType type, const QString &qth, QObject *parent)
  : QObject(parent), _Fbfo(8e2), _qth(qth), _sourceType(type), _source(0),
    _agc(), _wspr(_Fbfo), _audio(), _queue(Queue::get()), _monitor(true), _timer()
{
  switch (_sourceType) {
  case AUDIO_SOURCE: _source = new WsprAudioSource(10.1402e6, 8e2); break;
  case RTL_SOURCE: _source = new WsprRtlSource(10.1402e6, 8e2); break;
  }

  _source->source()->connect(&_agc, true);
  _agc.connect(&_wspr);
  if (_monitor) {
    _agc.connect(&_audio);
  }
  // disable AGC by default
  _agc.enable(false);

  _queue.addStart(this, &Receiver::_onQueueStart);
  _queue.addStop(this, &Receiver::_onQueueStop);

  _timer.setSingleShot(true);
  QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(_onStartRX()));

  _messages = new QStandardItemModel();
  _messages->setColumnCount(5);
  QStringList headers; headers << "Time" << "Callsign" << "Locator" << "Power" << "Distance"
                               << "SNR" << "Frequency" << "Delay";
  _messages->setHorizontalHeaderLabels(headers);

  // Register message handler
  _wspr.addMsgReceived(this, &Receiver::_onMessagesReceived);
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

Receiver::SourceType
Receiver::sourceType() const {
  return _sourceType;
}

void
Receiver::setSourceType(SourceType type) {
  // Get the current frequency and BFO freq.
  double F = _source->frequency();
  double Fbfo = _source->bfoFrequency();
  // Stop queue (if running)
  bool is_running = sdr::Queue::get().isRunning();
  if (is_running) {
    sdr::Queue::get().stop();
    sdr::Queue::get().wait();
  }
  // Delete current source:
  delete _source;
  // Create new source
  _sourceType = type;
  switch (_sourceType) {
  case AUDIO_SOURCE: _source = new WsprAudioSource(F, Fbfo); break;
  case RTL_SOURCE: _source = new WsprRtlSource(F, Fbfo); break;
  }
  // Connect source
  _source->source()->connect(&_agc, true);
  // Restart queue (if running)
  if (is_running) { sdr::Queue::get().start(); }
}

double
Receiver::frequency() const {
  return _source->frequency();
}

void
Receiver::setFrequency(double F) {
  _source->setFrequency(F);
}

double
Receiver::bfoFrequency() const {
  return _Fbfo;
}

void
Receiver::setBfoFrequency(double F) {
  _Fbfo = F; _source->setBfoFrequency(F);
  _wspr.setBfoFrequency(F);
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
  } else if ( (!_monitor) && enabled) {
    _agc.connect(&_audio);
  }
  _monitor = enabled;
}

QStandardItemModel *
Receiver::messages() {
  return _messages;
}

const QString &
Receiver::qth() const {
  return _qth;
}

QWidget *
Receiver::createSourceControl() {
  return _source->createView();
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
    row.append(new QStandardItem(QString(msg.callsign().c_str())));
    row.append(new QStandardItem(QString(msg.locator().c_str())));
    row.append(new QStandardItem(QString::number(msg.powerW(), 'g', 3)));
    row.append(new QStandardItem(QString::number(loc_dist(msg.locator(), _qth.toStdString()), 'f', 0)));
    row.append(new QStandardItem(QString::number(msg.snr, 'f', 1)));
    row.append(new QStandardItem(QString::number(msg.df, 'f', 0)));
    row.append(new QStandardItem(QString::number(msg.dt, 'f', 1)));
    _messages->insertRow(0, row);
  }
}
