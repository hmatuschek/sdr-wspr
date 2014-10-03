#include "waterfallview.hh"
#include <QPainter>
#include <QPaintEvent>


using namespace sdr;


WaterfallView::WaterfallView(Receiver *rx, QWidget *parent)
  : gui::WaterFallView(rx->spectrum(), 256, parent), _rx(rx)
{
  QObject::connect(this, SIGNAL(click(double)), this, SLOT(onClick(double)));
}

WaterfallView::~WaterfallView() {
  // pass...
}

void
WaterfallView::paintEvent(QPaintEvent *evt) {
  gui::WaterFallView::paintEvent(evt);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.save();

  // Clip region to update
  painter.setClipRect(evt->rect());

  // Draw filter area:
  QRect filter; QColor color(0,0,255, 64);
  double dfdx = _spectrum->sampleRate()/this->width();
  double x1 = (-100 + _spectrum->sampleRate()/2)/dfdx;
  double x2 = ( 100 + _spectrum->sampleRate()/2)/dfdx;
  filter = QRect(x1, 0, x2-x1, this->height());
  painter.fillRect(filter, color);

  painter.restore();
}

void
WaterfallView::onClick(double dF) {
  double Fc = _rx->freqCorrection();
  Fc += dF;
  _rx->setFreqCorrection(Fc);
}
