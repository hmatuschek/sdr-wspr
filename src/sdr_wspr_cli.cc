#include <iostream>
#include <getopt.h>
#include <csignal>

#include "logger.hh"
#include "portaudio.hh"
#include "wavfile.hh"
#include "subsample.hh"
#include "rtlsource.hh"
#include "autocast.hh"
#include "baseband.hh"
#include "demod.hh"
#include "wspr2.hh"

using namespace sdr;

static void __sigint_handler(int signo) {
  std::cerr << "Stop Queue..." << std::endl;
  // On SIGINT -> stop queue properly
  Queue::get().stop();
  Queue::get().wait();
}


class WSPRMessageLogger {
public:
  WSPRMessageLogger(WSPR2 &wspr) : _wspr(wspr) {
    _wspr.addMsgReceived(this, &WSPRMessageLogger::log);
  }

  void log() {
    std::list<WSPRMessage>::iterator msg = _wspr.messages().begin();
    for (; msg != _wspr.messages().end(); msg++) {
      std::cout << "Received: '" << msg->msg << " @ SNR=" << msg->snr << std::endl;
    }
  }

protected:
  WSPR2 &_wspr;
};


int main(int argc, char *argv[])
{
  Logger::get().addHandler(
        new StreamLogHandler(std::cerr, sdr::LOG_DEBUG));

  if (argc<2) {
    std::cerr << "USAGE: sdr_wspr_cli (file|audio|RTL) (FILENAME|FREQUENCY)" << std::endl;
    return -1;
  }

  // Register handler:
  signal(SIGINT, __sigint_handler);

  std::string type = argv[1];
  std::string arg;
  if (argc > 2) { arg = argv[2]; }
  // Check if input is a file
  bool is_file = (access( arg.c_str(), F_OK ) != -1);
  // or a floating point number...
  bool is_freq = (0 != atof(arg.c_str()));
  double Fbfo = 1.5e3;
  if (("audio" == type) && is_freq) {
    Fbfo = atof(arg.c_str());
  }

  PortAudio::init();

  WavSource *wav_source = 0;
  PortSource<int16_t> *audio_source = 0;
  InpolSubSampler<int16_t> *subsample = 0;
  RTLSource *rtl_source = 0;
  AutoCast< int16_t > *rcast = 0;
  AutoCast< std::complex<int16_t> > *ccast = 0;
  IQBaseBand< int16_t > *baseband = 0;
  USBDemod<int16_t> *demod = 0;
  WSPR2 wspr(Fbfo);  // BFO freq 800 Hz
  WSPRMessageLogger msglog(wspr);

  if ( ("file" == type) && is_file) {
    wav_source = new WavSource(arg);
    rcast = new AutoCast< int16_t >();
    subsample = new InpolSubSampler<int16_t>(wav_source->sampleRate()/12e3);
    wav_source->connect(rcast);
    rcast->connect(subsample);
    subsample->connect(&wspr);
    Queue::get().addIdle(wav_source, &WavSource::next);
    wav_source->addEOS(&(Queue::get()), &Queue::stop);
    wspr.startRX();
  } else if ( ("RTL" == type) && is_freq) {
    rtl_source = new RTLSource(atof(arg.c_str()), 240000);
    ccast = new AutoCast< std::complex<int16_t> >();
    baseband = new IQBaseBand<int16_t>(0, 500, 16, 1, 12e3);
    demod = new USBDemod<int16_t>();
    rtl_source->connect(ccast);
    ccast->connect(baseband);
    demod->connect(demod);
    demod->connect(&wspr);
    Queue::get().addStart(rtl_source, &RTLSource::start);
    Queue::get().addStop(rtl_source, &RTLSource::stop);
  } else if ("audio" == type) {
    audio_source = new PortSource<int16_t>(12e3, 512);
    audio_source->connect(&wspr);
    Queue::get().addIdle(audio_source, &PortSource<int16_t>::next);
    wspr.startRX();
  } else {
    std::cerr << "Invalid source specification '" << type
              << "', expected 'audio', 'file' or 'RTL'." << std::endl;
    return -1;
  }


  Queue::get().start();
  Queue::get().wait();

  wspr.join();
  msglog.log();

  PortAudio::terminate();


  if (wav_source) { delete wav_source; }
  if (subsample) { delete subsample; }
  if (audio_source) { delete audio_source; }
  if (rtl_source) { delete rtl_source; }
  if (ccast) { delete ccast; }
  if (baseband) { delete baseband; }
  if (demod) { delete demod; }

  return 0;
}
