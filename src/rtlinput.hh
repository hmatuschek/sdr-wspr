#ifndef RTLINPUT_HH
#define RTLINPUT_HH

#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include "rtlsource.hh"
#include "baseband.hh"
#include "demod.hh"
#include "autocast.hh"
#include "portaudio.hh"


class Input: public QObject
{
  Q_OBJECT

public:
  explicit Input(QObject *parent=0);

  virtual QWidget *createView() = 0;
  virtual sdr::Source *source() = 0;
};


class RTLInput : public Input
{
  Q_OBJECT

public:
  explicit RTLInput(double F, double Fbfo, QObject *parent = 0);
  virtual ~RTLInput();

  QWidget *createView();
  sdr::Source *source();

  double frequency() const;
  void setFrequency(double F);
  double freqCorrection() const;
  void setFreqCorrection(double F);
  double bfoFrequency() const;
  void setBfoFrequency(double F);

  bool sourceAGCEnabled() const;
  void enableSourceAGC(bool enabled);

signals:
  void frequencyChanged(double F);
  void frequencyCorrectionChanged(double F);

protected:
  double _F;
  double _Fbfo;
  double _Fcorr;

  sdr::RTLSource *_src;
  sdr::AutoCast< std::complex<int16_t> > _cast;
  sdr::IQBaseBand<int16_t> _baseband;
  sdr::USBDemod<int16_t> _demod;
  sdr::Queue &_queue;
};


class RTLInputView: public QWidget
{
  Q_OBJECT

public:
  explicit RTLInputView(RTLInput *input, QWidget *parent=0);

protected slots:
  void onBandSelected(int idx);
  void onRXFreqChanged(double F);
  void onRXFreqCorrChanged(double F);
  void onFreqCorrEdited();
  void onSrcAGCToggled(bool enabled);

protected:
  RTLInput *_input;
  QComboBox *_band;
  QLineEdit *_freq;
  QLineEdit *_Fcorr;
  QLineEdit *_Fbfo;
  QCheckBox *_rtl_agc;

};

class AudioInput: public Input
{
  Q_OBJECT

public:
  explicit AudioInput(QObject *parent=0);
  virtual ~AudioInput();

  QWidget *createView();
  sdr::Source *source();

protected:
  sdr::PortSource<int16_t> _src;
};


#endif // RTLINPUT_HH
