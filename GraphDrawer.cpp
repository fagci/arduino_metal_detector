#include "GraphDrawer.h"

Point::Point(int px = 0, int py = 0) {
  x = px;
  y = py;
}

GraphDrawer::GraphDrawer(U8G2* gfx, byte x, byte y, byte w, byte h, const MeasurementThread* source) {
  _x = x;
  _y = y;
  _w = w;
  _h = h;
  _gh = _h;
  _gb = _y + _h;
  display = gfx;
  _source = source;
  points = new Point[_source->count];
}

void GraphDrawer::update() {
  mint = min(_source->trigger, _source->min);
  maxt = max(_source->trigger, _source->max);
  window = _source->max - _source->min;

  for (byte i = 0; i < _source->count; i++) {
    points[i].x = _x + i;
    points[i].y = _gb - _gh * _source->values[i] / 1024;
  }
}

void GraphDrawer::draw() {
  byte right_side = _x + _w;
  byte bottom_side = _y + _h;
  char buf[4];

  Point p_prv;
  Point p_cur;

  byte sw, sh;

  //  display->drawFrame(_x, _y, _w, _h);

  sprintf(buf, "%d", _source->max);
  sw = display->getStrWidth(buf);
  display->drawStr(right_side - sw, _y, buf);

  sprintf(buf, "%d", _source->min);
  sw = display->getStrWidth(buf);
  sh = display->getMaxCharHeight();
  display->drawStr(right_side - sw, bottom_side - sh, buf);

  for (byte i = 1; i < _source->count; i++) {
    p_prv = points[i - 1];
    p_cur = points[i];
    display->drawLine(p_prv.x, p_prv.y, p_cur.x, p_cur.y);
  }

  //  for (byte i = 0; i < _source->count; ++i) {
  //    p_cur = points[i];
  //    display->drawPixel(p_cur.x, p_cur.y);
  //  }

  display->drawHLine(_x, _gb - _gh * _source->trigger / 1024, _w);
}

