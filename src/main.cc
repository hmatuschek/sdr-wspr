#include "receiver.hh"
#include "waterfallview.hh"

#include <iostream>
#include <csignal>

#include <QApplication>
#include <QMainWindow>

using namespace sdr;



int main(int argc, char *argv[])
{
  if (argc < 2) {
    std::cerr << "USAGE: sdr_wspr FILENAME" << std::endl;
    return -1;
  }

  sdr::Logger::get().addHandler(
        new sdr::StreamLogHandler(std::cerr, sdr::LOG_DEBUG));

  PortAudio::init();

  // Create nodes
  //WavSource src(argv[1]);
  double F = 100e6;
  F +=  3.592600e6+200;  // 80m;
  //F +=  7.038600e6+200;  // 40m;
  //F += 10.138700e6;  // 30m;
  //F += 14.095600e6;  // 20m;
  //F += 18.104600e6;  // 17m;
  //F += 21.094600e5;  // 15m;

  Receiver rx(F);

  //src.connect(&inpol);
  //inpol.connect(&wspr);

  QApplication app(argc, argv);

  QMainWindow *win = new QMainWindow();
  win->setCentralWidget(new WaterfallView(rx.spectrum()));
  win->setMinimumSize(640,480);
  win->show();

  Queue::get().start();
  app.exec();
  Queue::get().stop();
  Queue::get().wait();

  // Wait for all WSPR worker threads to finish:
  rx.join();

  PortAudio::terminate();

  return 0;
}
