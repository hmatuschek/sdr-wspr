#include "rtlinput.hh"
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>


using namespace sdr;

/* ********************************************************************************************* *
 * Implementation of Input
 * ********************************************************************************************* */
Input::Input(QObject *parent)
  : QObject(parent)
{
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of RTLInput
 * ********************************************************************************************* */
RTLInput::RTLInput(double F, double Fbfo, QObject *parent)
  : Input(parent), _F(F), _Fbfo(Fbfo), _Fcorr(0), _src(0), _cast(),
    _baseband(0, _Fbfo, 800, 31, 1 , 12e3), _demod(), _queue(Queue::get())
{
  try {
    _src = new RTLSource(_F-_Fbfo, 240000);
    _src->connect(&_cast, true);
    _queue.addStart(_src, &RTLSource::start);
    _queue.addStop(_src, &RTLSource::stop);
  } catch (sdr::ConfigError &err) {
    LogMessage msg(LOG_INFO);
    msg << "Can not create RTL device: " << err.what();
    Logger::get().log(msg);
  }
  _cast.connect(&_baseband, true);
  _baseband.connect(&_demod, true);
}

RTLInput::~RTLInput() {
  if (_src) { delete _src; }
}

QWidget *
RTLInput::createView() {
  return 0;
}

sdr::Source *
RTLInput::source() {
  return &_demod;
}


double
RTLInput::frequency() const {
  return _F;
}

void
RTLInput::setFrequency(double F) {
  _F = F;
  if (_src ) {
    _src->setFrequency(_F+_Fcorr-_Fbfo);
    emit frequencyChanged(_F);
  }
}

double
RTLInput::freqCorrection() const {
  return _Fcorr;
}

void
RTLInput::setFreqCorrection(double F) {
  _Fcorr = F;
  if (_src) {
    _src->setFrequency(_F+_Fbfo+_Fcorr);
    emit frequencyCorrectionChanged(_Fcorr);
  }
}

double
RTLInput::bfoFrequency() const {
  return _Fbfo;
}

void
RTLInput::setBfoFrequency(double F) {
  _Fbfo = F;
  bool isRunning = _queue.isRunning();
  if (isRunning) { _queue.stop(); _queue.wait(); }
  if (_src) {
    _src->setFrequency(_F+_Fcorr-_Fbfo);
  }
  _baseband.setCenterFrequency(_Fbfo);
  if (isRunning) { _queue.start(); }
}

bool
RTLInput::sourceAGCEnabled() const {
  if (_src) {
    return _src->agcEnabled();
  }
  return false;
}

void
RTLInput::enableSourceAGC(bool enabled) {
  if (_src) {
    _src->enableAGC(enabled);
  }
}


/* ********************************************************************************************* *
 * Implementation of RTLInputView
 * ********************************************************************************************* */
RTLInputView::RTLInputView(RTLInput *input, QWidget *parent)
  : QWidget(parent), _input(input)
{
  _band = new QComboBox();
  _band->addItem("160m", 1836.6);
  _band->addItem("80m", 3592.6);
  _band->addItem("60m", 5287.2);
  _band->addItem("40m", 7038.6);
  _band->addItem("30m", 10138.7);
  _band->addItem("20m", 14095.6);
  _band->addItem("17m", 18104.6);
  _band->addItem("15m", 21094.6);
  _band->addItem("12m", 24924.6);
  _band->addItem("10m", 28124.6);
  _band->addItem("6m", 50293.00);
  _band->addItem("2m", 144488.5);
  _band->setCurrentIndex(0);

  double F = _band->currentData().toDouble()*1000;
  _input->setFrequency(F);

  _freq = new QLineEdit(QString::number((_input->frequency()+_input->freqCorrection())/1e3));
  _freq->setEnabled(false);

  _Fcorr = new QLineEdit(QString::number(_input->freqCorrection()));
  QDoubleValidator *fval = new QDoubleValidator();
  _Fcorr->setValidator(fval);

  _Fbfo = new QLineEdit(QString::number(_input->bfoFrequency()));
  _Fbfo->setEnabled(false);

  _rtl_agc = new QCheckBox();
  _rtl_agc->setChecked(_input->sourceAGCEnabled());

  QGroupBox *rx_box = new QGroupBox("Receiver");
  QFormLayout *rx_layout = new QFormLayout();
  rx_layout->addRow("Band", _band);
  rx_layout->addRow("Freq. (kHz)", _freq);
  rx_layout->addRow("Freq. cor. (Hz)", _Fcorr);
  rx_layout->addRow("BFO (Hz)", _Fbfo);
  rx_layout->addRow("AGC", _rtl_agc);
  rx_box->setLayout(rx_layout);
}

void
RTLInputView::onBandSelected(int idx) {
  _input->setFrequency(_band->itemData(idx).toDouble());
}

void
RTLInputView::onRXFreqChanged(double F) {
  _input->setFrequency(F);
}

void
RTLInputView::onRXFreqCorrChanged(double F) {
  _input->setFreqCorrection(F);
}

void
RTLInputView::onFreqCorrEdited() {
  bool ok;
  double F = _Fcorr->text().toDouble(&ok);
  if (ok) { _input->setFreqCorrection(F); }
}

void
RTLInputView::onSrcAGCToggled(bool enabled) {
  _input->enableSourceAGC(enabled);
}


/* ********************************************************************************************* *
 * Implementation of AudioInput
 * ********************************************************************************************* */
AudioInput::AudioInput(QObject *parent)
  : Input(parent), _src(12e3, 512)
{
  sdr::Queue::get().addIdle(&_src, &PortSource<int16_t>::next);
}

AudioInput::~AudioInput() {
  //pass...
}

QWidget *
AudioInput::createView() {
  QLabel *view = new QLabel("Nothing to set for Audio input.");
  return view;
}

sdr::Source *
AudioInput::source() {
  return &_src;
}
