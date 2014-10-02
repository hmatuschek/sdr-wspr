#include "receiver.hh"
#include <QTime>

using namespace sdr;

Receiver::Receiver(double F, QObject *parent)
  : QObject(parent), _src(F, 240000), _cast(), _baseband(0, 1500, 800, 16, 1, 12e3),
    _demod(), _agc(), _wspr(), _audio(), _queue(Queue::get()), _timer()
{
  _src.connect(&_cast, true);
  _cast.connect(&_baseband, true);
  _baseband.connect(&_demod, true);
  _demod.connect(&_agc);
  _agc.connect(&_wspr);
  _agc.connect(&_audio);

  _queue.addStart(&_src, &RTLSource::start);
  _queue.addStop(&_src, &RTLSource::stop);

  _queue.addStart(this, &Receiver::_onQueueStart);
  _queue.addStop(this, &Receiver::_onQueueStop);

  _timer.setSingleShot(true);
  QObject::connect(&_timer, SIGNAL(timeout()), this, SLOT(_onStartRX()));
}

Receiver::~Receiver() {
  // pass...
}

gui::SpectrumProvider *
Receiver::spectrum() {
  return &_wspr;
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
Receiver::join() {
  _wspr.join();
}


