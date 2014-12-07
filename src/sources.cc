#include "sources.hh"
#include <QGroupBox>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileDialog>


using namespace sdr;

/* ********************************************************************************************* *
 * Implementation of Input
 * ********************************************************************************************* */
WSPRSource::WSPRSource(double F, double Fbfo, QObject *parent)
  : QObject(parent), _F(F), _Fbfo(Fbfo)
{
  // pass...
}

double
WSPRSource::frequency() const {
  return _F;
}

void
WSPRSource::setFrequency(double F) {
  _F = F;
}

double
WSPRSource::bfoFrequency() const {
  return _Fbfo;
}

void
WSPRSource::setBfoFrequency(double F) {
  _Fbfo = F;
}


/* ********************************************************************************************* *
 * Implementation of RTLInput
 * ********************************************************************************************* */
WsprRtlSource::WsprRtlSource(double F, double Fbfo, QObject *parent)
  : WSPRSource(F, Fbfo, parent), _Fif(100e6), _Fcorr(0), _src(0), _cast(),
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

WsprRtlSource::~WsprRtlSource() {
  if (_src) {
    _queue.remStart(_src);
    _queue.remStop(_src);
    delete _src;
  }
  if (_view) { _view->deleteLater(); }
}

QWidget *
WsprRtlSource::createView() {
  if (0 == _view) { _view = new WsprRtlSourceView(this); }
  return _view;
}

sdr::Source *
WsprRtlSource::source() {
  return &_demod;
}


double
WsprRtlSource::frequency() const {
  return _F;
}

void
WsprRtlSource::setFrequency(double F) {
  _F = F;
  if (_src ) {
    std::cerr << "Tune to " << int(_Fif+_F+_Fcorr-_Fbfo) << "Hz" << std::endl;
    _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
    emit frequencyChanged(_F);
  }
}

double
WsprRtlSource::freqCorrection() const {
  return _Fcorr;
}

void
WsprRtlSource::setFreqCorrection(double F) {
  _Fcorr = F;
  if (_src) {
    std::cerr << "Tune to " << int(_Fif+_F+_Fcorr-_Fbfo) << "Hz" << std::endl;
    _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
    emit frequencyCorrectionChanged(_Fcorr);
  }
}

double
WsprRtlSource::bfoFrequency() const {
  return _Fbfo;
}

void
WsprRtlSource::setBfoFrequency(double F) {
  _Fbfo = F;
  if (_src) {
    _src->setFrequency(_Fif+_F+_Fcorr-_Fbfo);
  }
  _baseband.setCenterFrequency(_Fbfo);
}

bool
WsprRtlSource::sourceAGCEnabled() const {
  if (_src) {
    return _src->agcEnabled();
  }
  return false;
}

void
WsprRtlSource::enableSourceAGC(bool enabled) {
  if (_src) {
    _src->enableAGC(enabled);
  }
}


/* ********************************************************************************************* *
 * Implementation of RTLInputView
 * ********************************************************************************************* */
WsprRtlSourceView::WsprRtlSourceView(WsprRtlSource *input, QWidget *parent)
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

  QObject::connect(_Fcorr, SIGNAL(returnPressed()), this, SLOT(onFreqCorrEdited()));
  QObject::connect(_rtl_agc, SIGNAL(toggled(bool)), this, SLOT(onSrcAGCToggled(bool)));
}

void
WsprRtlSourceView::onRXFreqCorrChanged(double F) {
  _input->setFreqCorrection(F);
}

void
WsprRtlSourceView::onFreqCorrEdited() {
  bool ok;
  double F = _Fcorr->text().toDouble(&ok);
  if (ok) { _input->setFreqCorrection(F); }
}

void
WsprRtlSourceView::onSrcAGCToggled(bool enabled) {
  _input->enableSourceAGC(enabled);
}


/* ********************************************************************************************* *
 * Implementation of AudioInput
 * ********************************************************************************************* */
WsprAudioSource::WsprAudioSource(double F, double Fbfo, QObject *parent)
  : WSPRSource(F, Fbfo, parent), _src(12e3, 512), _view(0)
{
  sdr::Queue::get().addIdle(&_src, &PortSource<int16_t>::next);
}

WsprAudioSource::~WsprAudioSource() {
  sdr::Queue::get().remIdle(&_src);
  if (_view) { _view->deleteLater(); }
}

QWidget *
WsprAudioSource::createView() {
  if (0 == _view) { _view = new QLabel("No Settings for audio source."); }
  return _view;
}

sdr::Source *
WsprAudioSource::source() {
  return &_src;
}


/* ********************************************************************************************* *
 * Implementation of WsprFileSource
 * ********************************************************************************************* */
WsprFileSource::WsprFileSource(double F, double Fbfo, QObject *parent)
  : WSPRSource(F, Fbfo, parent), _src(), _autocast(), _subsampler(12e3), _view(0)
{
  _src.connect(&_autocast);
  _autocast.connect(&_subsampler, true);
  sdr::Queue::get().addIdle(&_src, &WavSource::next);
}

WsprFileSource::~WsprFileSource() {
  sdr::Queue::get().remIdle(&_src);
  if (_view) { _view->deleteLater(); }
}

QWidget *
WsprFileSource::createView() {
  if (0 == _view) {
    _view = new WsprFileSourceView(this);
  }
  return _view;
}

sdr::Source *
WsprFileSource::source() {
  return &_src;
}

bool
WsprFileSource::openFile(const QString &filename) {
  _src.open(filename.toStdString());
  return _src.isOpen();
}


/* ********************************************************************************************* *
 * Implementation of WsprFileSourceView
 * ********************************************************************************************* */
WsprFileSourceView::WsprFileSourceView(WsprFileSource *src, QWidget *parent)
  : QWidget(parent), _source(src)
{
  _filename = new QLineEdit();
  QPushButton *select = new QPushButton("...");
  QHBoxLayout *hbox = new QHBoxLayout();
  hbox->addWidget(_filename, 1);
  hbox->addWidget(select, 0);

  QFormLayout *layout = new QFormLayout();
  layout->addRow("File", hbox);
  setLayout(layout);

  QObject::connect(select, SIGNAL(clicked()), this, SLOT(onSelectClicked()));
  QObject::connect(_filename, SIGNAL(returnPressed()), this, SLOT(onFileSelected()));
}

void
WsprFileSourceView::onSelectClicked() {
  QString path = QFileDialog::getOpenFileName(0, "Select file", "", "Wave (*.wav)");
  if ("" == path) { return; }
  _filename->setText(path);
  onFileSelected();
}

void
WsprFileSourceView::onFileSelected() {
  bool isRunning = sdr::Queue::get().isRunning();
  if (isRunning) { sdr::Queue::get().stop(); sdr::Queue::get().wait(); }
  _source->openFile(_filename->text());
  if (isRunning) { sdr::Queue::get().start(); }
}
