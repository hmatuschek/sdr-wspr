#include "wavfile.hh"
#include "rtlsource.hh"
#include "baseband.hh"
#include "demod.hh"
#include "subsample.hh"
#include "gui/spectrum.hh"
#include "gui/waterfallview.hh"
#include "freqshift.hh"

#include <iostream>
#include <csignal>

#include <QApplication>
#include <QMainWindow>

using namespace sdr;

class WSPR2: public Sink<int16_t>, public Source
{
public:
  WSPR2() : Sink<int16_t>(), Source(), _fshift(0,0)
  {
    // pass...
  }

  virtual ~WSPR2() {
    // pass...
  }

  virtual void config(const Config &src_cfg) {
    // Requires type, sample-rate and buffer size
    if (!src_cfg.hasType() || ! src_cfg.hasSampleRate() || !src_cfg.hasBufferSize()) { return; }
    // check buffer type
    if (Config::typeId<int16_t>() != src_cfg.type()) {
      ConfigError err;
      err << "Can not configure WSPR2 node: Invalid buffer type " << src_cfg.type()
          << ", expected " << Config::typeId<int16_t>();
      throw err;
    }
    // Check sample rate:
    /*if (12000 != src_cfg.sampleRate()) {
      ConfigError err;
      err << "Can not configure WSPR2 node: Invalid sample rate " << src_cfg.sampleRate()
          << ", expected 12kHz.";
      throw err;
    }*/

    // Config frequency shift
    _fshift.setFrequencyShift(-1500);
    _fshift.setSampleRate(src_cfg.sampleRate());

    // Allocate working buffer
    _work = Buffer< std::complex<double> >(src_cfg.bufferSize());
    _curr_avg = 0; _avg_count = 0;

    // Propergate config
    this->setConfig(Config(Config::typeId< std::complex<double> >(),
                           src_cfg.sampleRate()/32, src_cfg.bufferSize(), 1));
  }

  virtual void process(const Buffer<int16_t> &buffer, bool allow_overwrite) {
    size_t i=0, o=0;
    for (; i<buffer.size(); i++) {
      _curr_avg += _fshift.applyFrequencyShift(buffer[i]); _avg_count++;
      if (32 == _avg_count) {
        _work[o] = _curr_avg/( 32.0 * (2<<15) );
        _curr_avg=0; _avg_count=0; o++;
      }
    }
    this->send(_work.head(o), false);
  }

protected:
  FreqShiftBase<int16_t> _fshift;
  /** Current average value. */
  std::complex<double> _curr_avg;
  /** Curretn average count. */
  size_t _avg_count;
  /** The working buffer. */
  Buffer< std::complex<double> > _work;
};



int main(int argc, char *argv[])
{
  sdr::Logger::get().addHandler(
        new sdr::StreamLogHandler(std::cerr, sdr::LOG_DEBUG));

  // Create nodes
  //WavSource src(argv[1]);
  RTLSource src(114.0956e6, 240000);
  AutoCast< std::complex<int16_t> > cast;
  IQBaseBand<int16_t> baseband(0, 1500, 500, 16, 1, 12e3);
  USBDemod<int16_t>   demod;
  WSPR2 wspr;
  gui::Spectrum spec(374./512, 512, 1);

  src.connect(&cast, true);
  cast.connect(&baseband, true);
  baseband.connect(&demod, true);
  demod.connect(&wspr);
  wspr.connect(&spec);

  QApplication app(argc, argv);

  QMainWindow *win = new QMainWindow();
  win->setCentralWidget(new gui::WaterFallView(&spec, 100));
  win->setMinimumSize(640,480);
  win->show();

  Queue::get().addStart(&src, &RTLSource::start);
  Queue::get().addStop(&src, &RTLSource::stop);

  Queue::get().start();
  app.exec();
  Queue::get().stop();
  Queue::get().wait();

  return 0;
}
