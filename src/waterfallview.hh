#ifndef __SDR_WSPR_WATERFALLVIEW_HH__
#define __SDR_WSPR_WATERFALLVIEW_HH__

#include "gui/waterfallview.hh"
#include "receiver.hh"

class WaterfallView
    : public sdr::gui::WaterFallView
{
  Q_OBJECT

public:
  WaterfallView(Receiver *rx, QWidget *parent=0);
  virtual ~WaterfallView();

protected slots:
  void onClick(double dF);

protected:
  virtual void paintEvent(QPaintEvent *evt);

protected:
  Receiver *_rx;
};

#endif // __SDR_WSPR_WATERFALLVIEW_HH__
