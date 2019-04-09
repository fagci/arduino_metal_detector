#ifndef GRAPH_DRAWER
#define GRAPH_DRAWER

#include <U8g2lib.h>
#include "MeasurementThread.h"

class Point {
  public:
    int x;
    int y;
    Point(int px, int py);
};

class GraphDrawer {
  public:
    GraphDrawer(U8G2* gfx, byte x, byte y, byte w, byte h, const MeasurementThread* source);
    void draw();
    void update();
  private:
    const MeasurementThread* _source;
    byte _x, _y, _w, _h;
    byte _gh;
    byte _gb;
    int window;
    int mint;
    int maxt;
    U8G2* display;
    Point* points;
};

#endif

