#include "mapview.hh"
#include <QFile>
#include <QWebFrame>
#include "../libwspr/wspr.hh"
#include <iostream>
#include <time.h>


/* ********************************************************************************************* *
 * Implementation of MapViewJSBridge
 * ********************************************************************************************* */
MapViewJSBridge::MapViewJSBridge(QObject *parent)
  : QObject(parent)
{
  // pass...
}

void
MapViewJSBridge::emitSetQTH(double lon, double lat) {
  emit setQTH(lon, lat);
}

void
MapViewJSBridge::emitAddConnection(const QString &callsign, double lon, double lat, double snr) {
  emit addConnection(callsign, lon, lat, snr);
}


/* ********************************************************************************************* *
 * Implementation of MapView
 * ********************************************************************************************* */
MapView::MapView(Receiver &rx, QWidget *parent)
  : QWebView(parent), _rx(rx), _bridge(this)
{
  // Add bridge to JS environment of QWebFrame
  page()->mainFrame()->addToJavaScriptWindowObject("bridge", &_bridge);
  // Gets called once the map has been loaded
  QObject::connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

  // Load HTML content
  QString html_content;
  QFile html_doc("://resources/map.html");
  if (! html_doc.open(QFile::ReadOnly)) {
    html_content = "Ooops: " + html_doc.errorString();
  } else {
    html_content = html_doc.readAll();
  }
  setHtml(html_content);

  QObject::connect(_rx.messages(), SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this, SLOT(onMessagesReceived(QModelIndex,int,int)));
}

void
MapView::onLoadFinished(bool success) {
  emit mapReady();
  float lon, lat;
  sdr::loc2deg(_rx.qth().toStdString(), lon, lat);
  _bridge.emitSetQTH(-lon, lat);
}

void
MapView::onMessagesReceived(QModelIndex parent, int from, int to) {

  for (int row=0; row<_rx.messages()->rowCount(); row++) {
    addConnection(_rx.messages()->item(row, 1)->text(),
                  _rx.messages()->item(row, 2)->text(),
                  _rx.messages()->item(row, 5)->text().toDouble());
  }
}

void
MapView::setQTH(double lon, double lat) {
  _bridge.emitSetQTH(lon, lat);
}

void
MapView::addConnection(const QString &callsign, const QString &loc, double snr) {
  float lon, lat;
  sdr::loc2deg(loc.toStdString(), lon, lat);
  _bridge.emitAddConnection(callsign, -lon, lat, snr);
}
