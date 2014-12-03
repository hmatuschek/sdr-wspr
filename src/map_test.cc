#include <QMainWindow>
#include <QApplication>

#include "mapview.hh"
#include <QWebSettings>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QWebSettings::globalSettings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);

  QMainWindow *win = new QMainWindow();
  MapView *view = new MapView("JO62PK");

  win->setCentralWidget(view);
  win->show();

  app.exec();
  return 0;
}

