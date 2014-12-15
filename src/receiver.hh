#ifndef RECEIVER_HH
#define RECEIVER_HH

#include <QObject>
#include <QTimer>
#include <QStandardItemModel>
#include <QSettings>

#include "sources.hh"
#include "portaudio.hh"
#include "utils.hh"
#include "wspr2.hh"

/** Central receiver object. */
class Receiver : public QObject
{
  Q_OBJECT

public:
  /** Enumerates the possible sources. */
  typedef enum {
    RTL_SOURCE,   ///< A RTL2832 USB dongle.
    AUDIO_SOURCE, ///< The sound card
    FILE_SOURCE   ///< A WAV file.
  } SourceType;

public:
  /** Constructor.
   * @param type Specifies the input source to use.
   * @param parent Specifies the QObject parent. */
  explicit Receiver(SourceType type, QObject *parent = 0);
  /** Destructor. */
  virtual ~Receiver();

  /** Returns a weak reference to the spectrum provider. */
  sdr::gui::SpectrumProvider *spectrum();

  /** Wait for the WSPR decoder threads to stop. */
  void join();

  /** Returns the current source type. */
  SourceType sourceType() const;
  /** Sets the current source type. */
  void setSourceType(SourceType type);

  /** Creates a control view for the current source. */
  QWidget *createSourceControl();

  /** Returns the current frequency. */
  double frequency() const;
  /** Set current frequency. If the source is tunable (i.e. RTL2832) the source frequency gets
   * updated too. */
  void setFrequency(double F);

  /** Returns the BFO frequency. */
  double bfoFrequency() const;
  /** Sets the BFO frequency. */
  void setBfoFrequency(double F);

  /** Returns @c true if the audio AGC is enabled. */
  bool audioAGCEnabled() const;
  /** Enables/disables the audio AGC. */
  void enableAudioAGC(bool enabled);

  /** Returns @c true if audio monitoring is enabled. */
  bool monitorEnabled() const;
  /** Enables/disables audio monitoring. */
  void enableMonitor(bool enabled);

  /** Returns the QTH locator. */
  QString locator() const;
  /** Sets the QTH locator. */
  void setLocator(const QString &loc);

  /** Returns the list model of spots. */
  QStandardItemModel *messages();

signals:
  /** Gets emitted on new spots. */
  void messagesReceived();

protected:
  /** Gets called on Queue start. This will start a timer that starts the reception on an even
   * UTC minute. */
  void _onQueueStart();
  /** Gets called on Queue stop. */
  void _onQueueStop();
  /** Gets called once some messages are received. */
  void _onMessagesReceived();

protected slots:
  /** Gets called by on an even minute. */
  void _onStartRX();

protected:
  /** BFO frequency. */
  double _Fbfo;
  /** The currently selected source. */
  SourceType _sourceType;
  /** The source instance. */
  WSPRSource *_source;
  /** Audio AGC instance. */
  sdr::AGC<int16_t> _agc;
  /** WSPR node. */
  sdr::WSPR2 _wspr;
  /** Audio sink. */
  sdr::PortSink _audio;
  /** A weak reference to the queue. */
  sdr::Queue &_queue;
  /** If @c true, audio monitoring is enabled. */
  bool _monitor;
  /** List model of spots. */
  QStandardItemModel *_messages;
  /** Timer to start reception on even minutes. */
  QTimer _timer;

  /** Persistent settings. */
  QSettings _settings;
};

#endif // RECEIVER_HH
