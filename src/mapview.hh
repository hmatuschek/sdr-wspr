#ifndef MAPVIEW_HH
#define MAPVIEW_HH

#include <QWebView>

/** A trivial bridge to the JS environment, exposing two signals to update the map view. */
class MapViewJSBridge: public QObject
{
  Q_OBJECT

public:
  /** Constructor. */
  explicit MapViewJSBridge(QObject *parent=0);

  /** Emmits the setQTH signal. */
  void emitSetQTH(double lon, double lat);
  /** Emmits the addConnection signal. */
  void emitAddConnection(const QString &callsign, double lon, double lat, double snr);

signals:
  void setQTH(double lon, double lat);
  void addConnection(QString callsign, double lon, double lat, double snr);
};


/** A simple MapView using Google Maps. */
class MapView : public QWebView
{
  Q_OBJECT

public:
  /** Constructs a map view. */
  explicit MapView(QWidget *parent = 0);

signals:
  /** Gets emitted once the map was loaded completely. */
  void mapReady();

public slots:
  /** Centers the map at the given location. */
  void setQTH(double lon, double lat);
  /** Adds a connection (transmitter) to the map. */
  void addConnection(const QString &callsign, double lon, double lat, double snr);

protected slots:
  /** Gets called, once the map is loaded. */
  void onLoadFinished(bool success);

protected:
  /** The bridge to the JS environment of the MapView. */
  MapViewJSBridge _bridge;
};

#endif // MAPVIEW_HH
