#include <QMainWindow>
#include <QApplication>

#include "mapview.hh"


int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QMainWindow *win = new QMainWindow();
  MapView *view = new MapView();

  win->setCentralWidget(view);
  win->show();

  app.exec();
  return 0;
}

