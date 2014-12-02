#include "rtlinput.hh"
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>


using namespace sdr;

/* ********************************************************************************************* *
 * Implementation of Input
 * ********************************************************************************************* */
Input::Input(double F, double Fbfo, QObject *parent)
  : QObject(parent), _F(F), _Fbfo(Fbfo)
{
  // pass...
}

double
Input::frequency() const {
  return _F;
}

void
Input::setFrequency(double F) {
  _F = F;
}

double
Input::bfoFrequency() const {
  return _Fbfo;
}

void
Input::setBfoFrequency(double F) {
  _Fbfo = F;
}


/* ********************************************************************************************* *
 * Implementation of RTLInput
 * ********************************************************************************************* */
RTLInput::RTLInput(double F, double Fbfo, QObject *parent)
  : Input(F, Fbfo, parent), _Fif(100e6), _Fcorr(0), _src(0), _cast(),
    _baseband(0, _Fbfo, 800, 31, 1 , 12e3), _demod(), _queue(Queue::get()), _view(0)
{
  try {
    std::cerr << "Tune to " << int(_Fif+_F+_Fcorr-_Fbfo) << "Hz" << std::endl;
    _src = new RTLSource(_Fif+_F+_Fcorr-_Fbfo, 240000);
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
  if (_src) {
    _queue.remStart(_src);
    _queue.remStop(_src);
    delete _src;
  }
  if (_view) { _view->deleteLater(); }
}

QWidget *
RTLInput::createView() {
  if (0 == _view) { _view = new RTLInputView(this); }
  return _view;
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
    std::cerr << "Tune to " << int(_Fif+_F+_Fcorr-_Fbfo) << "Hz" << std::endl;
    _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
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
    std::cerr << "Tune to " << int(_Fif+_F+_Fcorr-_Fbfo) << "Hz" << std::endl;
    _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
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
  _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
  _baseband.setCenterFrequency(_Fbfo);
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
  _Fcorr = new QLineEdit(QString::number(_input->freqCorrection()));
  QDoubleValidator *fval = new QDoubleValidator();
  _Fcorr->setValidator(fval);

  _rtl_agc = new QCheckBox();
  _rtl_agc->setChecked(_input->sourceAGCEnabled());

  QFormLayout *layout = new QFormLayout();
  layout->addRow("Freq. cor. (Hz)", _Fcorr);
  layout->addRow("AGC", _rtl_agc);
  setLayout(layout);

  QObject::connect(_Fcorr, SIGNAL(editingFinished()), this, SLOT(onFreqCorrEdited()));
  QObject::connect(_rtl_agc, SIGNAL(toggled(bool)), this, SLOT(onSrcAGCToggled(bool)));
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
AudioInput::AudioInput(double F, double Fbfo, QObject *parent)
  : Input(F, Fbfo, parent), _src(12e3, 512), _view(0)
{
  sdr::Queue::get().addIdle(&_src, &PortSource<int16_t>::next);
}

AudioInput::~AudioInput() {
  sdr::Queue::get().remIdle(&_src);
  if (_view) { _view->deleteLater(); }
}

QWidget *
AudioInput::createView() {
  if (0 == _view) { _view = new QLabel("No Settings."); }
  return _view;
}

sdr::Source *
AudioInput::source() {
  return &_src;
}
