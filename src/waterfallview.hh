#ifndef __SDR_WSPR_WATERFALLVIEW_HH__
#define __SDR_WSPR_WATERFALLVIEW_HH__

#include "gui/waterfallview.hh"
#include "receiver.hh"

/** Extends the @c sdr::gui::WaterFallView to display the +/- 100Hz reception window around the
 * BFO frequency. */
class WaterfallView
    : public sdr::gui::WaterFallView
{
  Q_OBJECT

public:
  /** Constructor. */
  WaterfallView(Receiver *rx, QWidget *parent=0);
  /** Destructor. */
  virtual ~WaterfallView();

protected slots:
  void onClick(double dF);

protected:
  virtual void paintEvent(QPaintEvent *evt);

protected:
  Receiver *_rx;
};

#endif // __SDR_WSPR_WATERFALLVIEW_HH__
