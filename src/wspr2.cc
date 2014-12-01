#include "wspr2.hh"
#include "../libwspr/wspr.hh"

using namespace sdr;


WSPR2::WSPR2(double Fbfo)
  : Sink<int16_t>(), _Fbfo(Fbfo), _fshift(-_Fbfo, 12000), _state(STATE_WAIT),
    _N_rx(0), _rx_buff(1440000), _work(1440000),
    _N_fft(0), _fft_in(1024), _fft_out(1024), _fft(_fft_in, _fft_out, FFT::FORWARD),
    _currPSD(1024), _threads_lock(), _threads_cond(), _num_threads(0)
{
  pthread_mutex_init(&_threads_lock, 0);
  pthread_cond_init(&_threads_cond, 0);
}

WSPR2::~WSPR2() {
  // pass...
}


bool
WSPR2::isInputReal() const {
  return false;
}

double
WSPR2::sampleRate() const {
  return 750; //375;
}

size_t
WSPR2::fftSize() const {
  return 1024;
}

const Buffer<double> &
WSPR2::spectrum() const {
  return _currPSD;
}


void
WSPR2::config(const Config &src_cfg) {
  // Requires type, sample-rate and buffer size
  if (!src_cfg.hasType() || ! src_cfg.hasSampleRate() || !src_cfg.hasBufferSize()) { return; }

  // check buffer type
  if (Config::typeId<int16_t>() != src_cfg.type()) {
    ConfigError err;
    err << "Can not configure WSPR2 node: Invalid buffer type " << src_cfg.type()
        << ", expected " << Config::typeId<int16_t>();
    throw err;
  }

  // Config frequency shift
  _fshift.setFrequencyShift(-_Fbfo);
  _fshift.setSampleRate(src_cfg.sampleRate());

  // Sub sampling
  _curr_avg = 0; _avg_count = 0; _N_fft = 0;
  // Receiver state
  _state = STATE_WAIT; _N_rx = 0;

  LogMessage msg(LOG_DEBUG);
  msg << "Configure WSPR2 node:" << std::endl
      << " sample-rate: " << src_cfg.sampleRate() << std::endl
      << " F_bfo: " << _Fbfo;
  Logger::get().log(msg);

  emit spectrumConfigured();
}


void
WSPR2::process(const Buffer<int16_t> &buffer, bool allow_overwrite)
{
  /// @todo Perform interpolation sub-sampling to get exactly 12kHz sample rate here!

  for (size_t i=0; i<buffer.size(); i++)
  {
    // Store samples in RX buffer if RX is enabled
    if ((STATE_RX == _state) && (1440000 > _N_rx)) {
      _rx_buff[_N_rx] = buffer[i]; _N_rx++;
    }

    // Shift frequency and sub-sample by 16 (for spectrum)
    _curr_avg += _fshift.applyFrequencyShift(buffer[i]); _avg_count++;
    if (16 == _avg_count) {
      _fft_in[_N_fft] = _curr_avg/float( 16.0 * (1<<16) );
      _curr_avg=0; _avg_count=0; _N_fft++;
    }

    // If 512 samples have been received -> update spectrum
    if (1024 == _N_fft) {
      _N_fft = 0;
      // Compute FFT
      _fft();
      // Compute PSD and update avg PSD, PSDs
      for (size_t j=0; j<1024; j++) {
        _currPSD[j] = _fft_out[j].real()*_fft_out[j].real()
            + _fft_out[j].imag()*_fft_out[j].imag();
      }
      // Notify spectrum views about the new spectrum.
      emit spectrumUpdated();
    }

    // If 1440000 samples have been received (120s @ 12kHz) -> decode
    if (1440000 == _N_rx) {
      _N_rx=0;
      // Copy content to _work
      memcpy(_work.ptr(), _rx_buff.ptr(), sizeof(int16_t)*1440000);
      // Start decoding in another thread
      start_decode();
      //decode_signal();
    }
  }
}


void
WSPR2::join() {
  pthread_mutex_lock(&_threads_lock);
  while (_num_threads > 0) {
    pthread_cond_wait(&_threads_cond, &_threads_lock);
  }
  pthread_mutex_unlock(&_threads_lock);
}


void
WSPR2::start_decode() {
  pthread_t thread;
  if (pthread_create(&thread, 0, &WSPR2::_pthread_decode_func, this)) {
    RuntimeError err;
    err << "WSPR2: Can not create decoding thread!";
    throw err;
  }
  pthread_mutex_lock(&_threads_lock);
  _num_threads++;
  pthread_mutex_unlock(&_threads_lock);
}


void *
WSPR2::_pthread_decode_func(void *ctx)
{
  // Start decoding
  WSPR2 *self = reinterpret_cast<WSPR2 *>(ctx);
  self->decode_signal();

  // Remove thread from list of running threads:
  pthread_mutex_lock(&(self->_threads_lock));
  self->_num_threads--;
  pthread_cond_signal(&(self->_threads_cond));
  pthread_mutex_unlock(&(self->_threads_lock));
  return 0;
}


void
WSPR2::decode_signal()
{
  LogMessage msg(LOG_DEBUG);
  msg << "WSPR-2: Start decoding...";
  Logger::get().log(msg);

  // Decode signal and append messages to list
  wspr_decode(_work, _messages, _Fbfo);

  if (0 < _messages.size()) {
    // Signal received messages
    signalMessagesReceived();

    LogMessage msg(LOG_DEBUG);
    msg << "WSPR-2: Received " << _messages.size() << " messages.";
    Logger::get().log(msg);
  } else {
    LogMessage msg(LOG_DEBUG);
    msg << "WSPR-2: Found nothing...";
    Logger::get().log(msg);
  }
}


void
WSPR2::signalMessagesReceived() {
  // Just notify the delegates.
  std::list<DelegateInterface *>::iterator item = _rx_evt.begin();
  for (; item != _rx_evt.end(); item++) {
    (**item)();
  }
}
