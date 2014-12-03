#include "mapview.hh"
#include <QFile>
#include <QWebFrame>

#include <iostream>


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
MapView::MapView(QWidget *parent)
  : QWebView(parent), _bridge(this)
{
  // Gets called once the map has been loaded
  QObject::connect(this, SIGNAL(loadFinished(bool)), this, SLOT(onLoadFinished(bool)));

  QString html_content;
  QFile html_doc("://resources/map.html");
  if (! html_doc.open(QFile::ReadOnly)) {
    html_content = "Ooops: " + html_doc.errorString();
  } else {
    html_content = html_doc.readAll();
  }
  setHtml(html_content);

  // Add bridge to JS environment of QWebFrame
  page()->mainFrame()->addToJavaScriptWindowObject("bridge", &_bridge);
}

void
MapView::onLoadFinished(bool success) {
  emit mapReady();
  // Test
  _bridge.emitSetQTH(15, 52);
}

void
MapView::setQTH(double lon, double lat) {
  _bridge.emitSetQTH(lon, lat);
}

void
MapView::addConnection(const QString &callsign, double lon, double lat, double snr) {
  _bridge.emitAddConnection(callsign, lon, lat, snr);
}
