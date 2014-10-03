#ifndef RECEIVER_HH
#define RECEIVER_HH

#include <QObject>
#include <QTimer>
#include <QStandardItemModel>

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
  explicit Receiver(double F, double IF, QObject *parent = 0);
  virtual ~Receiver();

  sdr::gui::SpectrumProvider *spectrum();

  void join();

  double frequency() const;
  void setFrequency(double F);
  double freqCorrection() const;
  void setFreqCorrection(double F);
  double intermediateFrequency() const;
  void setIntermediateFrequency(double F);
  double bfoFrequency() const;
  void setBfoFrequency(double F);

  bool sourceAGCEnabled() const;
  void enableSourceAGC(bool enabled);
  bool audioAGCEnabled() const;
  void enableAudioAGC(bool enabled);
  bool monitorEnabled() const;
  void enableMonitor(bool enabled);

  QStandardItemModel *messages();

signals:
  void frequencyChanged(double F);
  void frequencyCorrectionChanged(double F);
  void messagesReceived();

protected:
  void _onQueueStart();
  void _onQueueStop();
  void _onMessagesReceived();

protected slots:
  void _onStartRX();

protected:
  /** The receiver frequency. */
  double _F;
  /** Frequency correction. */
  double _Fcorr;
  /** Intermediate frequency. */
  double _IF;
  /** BFO frequency. */
  double _Fbfo;

  sdr::RTLSource _src;
  sdr::AutoCast< std::complex<int16_t> > _cast;
  sdr::IQBaseBand<int16_t> _baseband;
  sdr::USBDemod<int16_t> _demod;
  sdr::AGC<int16_t> _agc;
  sdr::WSPR2 _wspr;
  sdr::PortSink _audio;
  sdr::Queue &_queue;
  bool _monitor;

  QStandardItemModel *_messages;
  QTimer _timer;
};

#endif // RECEIVER_HH
