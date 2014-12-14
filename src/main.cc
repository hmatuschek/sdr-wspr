
#include <QApplication>
#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>

#include <csignal>

#include "receiver.hh"
#include "waterfallview.hh"
#include "tableview.hh"
#include "mapview.hh"
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
  QTabWidget *tabs = new QTabWidget();
  tabs->addTab(new WaterfallView(&rx), "Spectrum");
  tabs->addTab(new TableView(rx), "Spots");
  tabs->addTab(new MapView(rx), "Map");
  split->addWidget(tabs);
  split->addWidget(new RXCtrlView(&rx));

  win->setCentralWidget(split);
  win->setMinimumSize(640,512);
  win->setWindowTitle("SDR-WSPR");
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
