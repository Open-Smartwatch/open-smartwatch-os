#ifndef OSM_RENDER_H
#define OSM_RENDER_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_2d.h"
#include "math_osm.h"

typedef void (*loadTile)(Graphics2D *target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);
void drawTiles(Graphics2D *target, loadTile loadTileFn, float lat, float lon, uint8_t z);

#endif