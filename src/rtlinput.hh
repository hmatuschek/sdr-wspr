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
  explicit Input(double F, double Fbfo, QObject *parent=0);

  virtual QWidget *createView() = 0;
  virtual sdr::Source *source() = 0;

  virtual double frequency() const;
  virtual void setFrequency(double F);
  virtual double bfoFrequency() const;
  virtual void setBfoFrequency(double F);

protected:
  double _F;
  double _Fbfo;
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
  double _Fif;
  double _Fcorr;

  sdr::RTLSource *_src;
  sdr::AutoCast< std::complex<int16_t> > _cast;
  sdr::IQBaseBand<int16_t> _baseband;
  sdr::USBDemod<int16_t> _demod;

  sdr::Queue &_queue;

  QWidget *_view;
};


class RTLInputView: public QWidget
{
  Q_OBJECT

public:
  explicit RTLInputView(RTLInput *input, QWidget *parent=0);

protected slots:
  void onRXFreqCorrChanged(double F);
  void onFreqCorrEdited();
  void onSrcAGCToggled(bool enabled);

protected:
  RTLInput *_input;
  QLineEdit *_Fcorr;
  QCheckBox *_rtl_agc;
};

class AudioInput: public Input
{
  Q_OBJECT

public:
  explicit AudioInput(double F, double Fbfo, QObject *parent=0);
  virtual ~AudioInput();

  QWidget *createView();
  sdr::Source *source();

protected:
  sdr::PortSource<int16_t> _src;
  QWidget *_view;
};


#endif // RTLINPUT_HH
