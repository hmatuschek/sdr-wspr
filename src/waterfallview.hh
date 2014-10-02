#ifndef __SDR_WSPR_WATERFALLVIEW_HH__
#define __SDR_WSPR_WATERFALLVIEW_HH__

#include "gui/waterfallview.hh"

class WaterfallView
    : public sdr::gui::WaterFallView
{
  Q_OBJECT

public:
  WaterfallView(sdr::gui::SpectrumProvider *spectrum, QWidget *parent=0);
  virtual ~WaterfallView();

protected:
  virtual void paintEvent(QPaintEvent *evt);
};

#endif // __SDR_WSPR_WATERFALLVIEW_HH__
