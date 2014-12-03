#ifndef SOURCES_HH
#define SOURCES_HH

#include <QObject>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include "rtlsource.hh"
#include "baseband.hh"
#include "demod.hh"
#include "autocast.hh"
#include "portaudio.hh"
#include "wavfile.hh"
#include "subsample.hh"


/** Base class for all WSPR sources. */
class WSPRSource: public QObject
{
  Q_OBJECT

public:
  explicit WSPRSource(double F, double Fbfo, QObject *parent=0);

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



class WsprRtlSource : public WSPRSource
{
  Q_OBJECT

public:
  explicit WsprRtlSource(double F, double Fbfo, QObject *parent = 0);
  virtual ~WsprRtlSource();

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



class WsprRtlSourceView: public QWidget
{
  Q_OBJECT

public:
  explicit WsprRtlSourceView(WsprRtlSource *input, QWidget *parent=0);

protected slots:
  void onRXFreqCorrChanged(double F);
  void onFreqCorrEdited();
  void onSrcAGCToggled(bool enabled);

protected:
  WsprRtlSource *_input;
  QLineEdit *_Fcorr;
  QCheckBox *_rtl_agc;
};



class WsprAudioSource: public WSPRSource
{
  Q_OBJECT

public:
  explicit WsprAudioSource(double F, double Fbfo, QObject *parent=0);
  virtual ~WsprAudioSource();

  QWidget *createView();
  sdr::Source *source();

protected:
  sdr::PortSource<int16_t> _src;
  QWidget *_view;
};


class WsprFileSource: public WSPRSource
{
  Q_OBJECT

public:
  explicit WsprFileSource(double F, double Fbfo, QObject *parent=0);
  virtual ~WsprFileSource();

  QWidget *createView();
  sdr::Source *source();

  bool openFile(const QString &filename);

protected:
  sdr::WavSource _src;
  sdr::AutoCast<int16_t> _autocast;
  sdr::InpolSubSampler<int16_t> _subsampler;
  QWidget *_view;
};


class WsprFileSourceView: public QWidget
{
  Q_OBJECT

public:
  explicit WsprFileSourceView(WsprFileSource *src, QWidget *parent=0);

protected slots:
  void onSelectClicked();
  void onFileSelected();

protected:
  WsprFileSource *_source;
  QLineEdit *_filename;
};

#endif // SOURCES_HH
