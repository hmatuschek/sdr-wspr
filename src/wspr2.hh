#ifndef __SDR_WSPR2_HH__
#define __SDR_WSPR2_HH__

#include "buffer.hh"
#include "fftplan.hh"
#include "freqshift.hh"
#include "gui/spectrum.hh"
#include "../libwspr/wspr.hh"


namespace sdr {

/** @todo Implement spectrum provider. */
class WSPR2: public Sink<int16_t>, public gui::SpectrumProvider
{
public:
  typedef enum {
    STATE_RX,
    STATE_WAIT
  } State;

public:
  WSPR2();
  virtual ~WSPR2();

  bool isInputReal() const;
  double sampleRate() const;
  size_t fftSize() const;
  const Buffer<double> & spectrum() const;

  inline State state() const { return _state; }
  inline void startRX() {
    if (STATE_RX == _state) { return; }
    _N_rx = 0; _state = STATE_RX;
  }
  inline void stopRX() {
    _N_rx = 0; _state = STATE_WAIT;
  }
  void join();

  virtual void config(const Config &src_cfg);

  virtual void process(const Buffer<int16_t> &buffer, bool allow_overwrite);

  /** Adds a callback to the "messages received" signal. */
  template <class T>
  void addMsgReceived(T* instance, void (T::*function)()) {
    _rx_evt.push_back(new Delegate<T>(instance, function));
  }

  inline std::list<WSPRMessage> &messages() { return _messages; }


protected:
  /** Starts a new thread for the decoding. */
  void start_decode();
  /** Just a callback for the pthread API. */
  static void *_pthread_decode_func(void *ctx);
  void decode_signal();
  void signalMessagesReceived();

protected:
  FreqShiftBase<int16_t> _fshift;
  /** Current average value. */
  std::complex<float> _curr_avg;
  /** Current average count. */
  size_t _avg_count;
  /** The state of the WSPR RX. */
  State _state;
  /** The number of samples received. */
  size_t _N_rx;
  /** The reception buffer. */
  Buffer<int16_t> _rx_buff;
  /** A copy of the last RX buffer. */
  Buffer<int16_t> _work;
  /** The number of samples in the @c _fft_in buffer. */
  size_t _N_fft;
  /** The fft input buffer. */
  Buffer< std::complex<float> > _fft_in;
  /** The output buffer of the FFT. */
  Buffer< std::complex<float> > _fft_out;
  /** The FFT plan. */
  FFTPlan<float> _fft;
  /** The current PSD. */
  Buffer<double> _currPSD;
  /** The list of the received messages. */
  std::list<WSPRMessage> _messages;
  std::list<DelegateInterface *> _rx_evt;
  /** The set of running threads. */
  std::list<pthread_t> _threads;
};


}

#endif // __SDR_WSPR2_HH__
