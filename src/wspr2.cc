#include "wspr2.hh"
#include "../libwspr/wspr.hh"

using namespace sdr;


WSPR2::WSPR2()
  : Sink<int16_t>(), _fshift(-1500,12000), _state(STATE_WAIT),
    _N_rx(0), _rx_buff(1440000), _work(1440000),
    _N_fft(0), _fft_in(1024), _fft_out(1024), _fft(_fft_in, _fft_out, FFT::FORWARD), _currPSD(1024)
{
  // pass...
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
  return 750;
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
  _fshift.setFrequencyShift(-1500);
  _fshift.setSampleRate(src_cfg.sampleRate());
  // Sub sampling
  _curr_avg = 0; _avg_count = 0; _N_fft = 0;
  _state = STATE_WAIT; _N_rx = 0;

  emit spectrumConfigured();
}


void
WSPR2::process(const Buffer<int16_t> &buffer, bool allow_overwrite)
{
  /// @todo Perform interpolation sub-sampling to get exactly 12kHz sample rate here!

  size_t i=0;
  while (i<buffer.size())
  {
    // Store samples in RX buffer if RX is enabled
    if ((STATE_RX == _state) && (1440000 > _N_rx)) {
      _rx_buff[_N_rx] = buffer[i]; _N_rx++;
    }

    // Shift frequency and sub-sample by 16 (for spectrum)
    _curr_avg += _fshift.applyFrequencyShift(buffer[i]); _avg_count++; i++;
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
  std::list<pthread_t>::iterator thread = _threads.begin();
  for(; thread!=_threads.end(); thread++) {
    void *ret=0;
    pthread_join(*thread, &ret);
  }
}

void
WSPR2::start_decode() {
  pthread_t thread;
  if (pthread_create(&thread, 0, &WSPR2::_pthread_decode_func, this)) {
    RuntimeError err;
    err << "WSPR2: Can not create decoding thread!";
    throw err;
  }
  _threads.push_back(thread);
}

void *
WSPR2::_pthread_decode_func(void *ctx)
{
  WSPR2 *self = reinterpret_cast<WSPR2 *>(ctx);
  self->decode_signal();

  // Remove from list of running threads:
  std::list<pthread_t>::iterator thread = self->_threads.begin();
  for (; thread != self->_threads.end(); thread++) {
    if ((*thread) == pthread_self()) {
      self->_threads.erase(thread);
      return 0;
    }
  }
  return 0;
}

void
WSPR2::decode_signal()
{
  LogMessage msg(LOG_DEBUG);
  msg << "WSPR-2: Start decoding...";
  Logger::get().log(msg);

  wspr_decode(_work, _messages);

  // Signal received messages
  if (_messages.size()) {
    signalMessagesReceived();
    LogMessage msg(LOG_DEBUG);
    msg << "WSPR-2: Received something...";
    Logger::get().log(msg);
  } else {
    LogMessage msg(LOG_DEBUG);
    msg << "WSPR-2: Found nothing...";
    Logger::get().log(msg);
  }
}

void
WSPR2::signalMessagesReceived() {
  std::list<DelegateInterface *>::iterator item = _rx_evt.begin();
  for (; item != _rx_evt.end(); item++) { (**item)(); }
}
