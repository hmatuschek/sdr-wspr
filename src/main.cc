#include "receiver.hh"
#include "waterfallview.hh"

#include <iostream>
#include <csignal>

#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include "rxctrlview.hh"


using namespace sdr;



int main(int argc, char *argv[])
{
  sdr::Logger::get().addHandler(
        new sdr::StreamLogHandler(std::cerr, sdr::LOG_DEBUG));

  PortAudio::init();

  // Create nodes
  Receiver rx(Receiver::AUDIO_SOURCE);

  QApplication app(argc, argv);

  QMainWindow *win = new QMainWindow();
  QSplitter *split = new QSplitter();
  split->addWidget(new WaterfallView(&rx));
  split->addWidget(new RXCtrlView(&rx));

  win->setCentralWidget(split);
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
