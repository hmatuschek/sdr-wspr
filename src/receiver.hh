#ifndef RECEIVER_HH
#define RECEIVER_HH

#include <QObject>
#include <QTimer>

#include "rtlsource.hh"
#include "baseband.hh"
#include "demod.hh"
#include "portaudio.hh"
#include "subsample.hh"
#include "utils.hh"
#include "wspr2.hh"

class Receiver : public QObject
{
  Q_OBJECT

public:
  explicit Receiver(double F, QObject *parent = 0);
  virtual ~Receiver();

  sdr::gui::SpectrumProvider *spectrum();

  void join();

protected:
  void _onQueueStart();
  void _onQueueStop();

protected slots:
  void _onStartRX();

protected:
  sdr::RTLSource _src;
  sdr::AutoCast< std::complex<int16_t> > _cast;
  sdr::IQBaseBand<int16_t> _baseband;
  sdr::USBDemod<int16_t> _demod;
  sdr::AGC<int16_t> _agc;
  sdr::WSPR2 _wspr;
  sdr::PortSink _audio;
  sdr::Queue &_queue;
  QTimer _timer;
};

#endif // RECEIVER_HH
