
#include "osw_app_map.h"

#include <anim_water_ripple.h>
#include <gfx_2d.h>
#include <gfx_util.h>
#include <osm_render.h>
#include <osw_app.h>
#include <osw_hal.h>

#define BUF_W 240
#define BUF_H 240

// Graphics2D* tileBuffer;
OswHal* h;

void OswAppMap::setup(OswHal* hal) {
  h = hal;
  // tileBuffer = new Graphics2D(240, 240, 4, true);
}

uint16_t z = 10;
#define MIN_Z 6
#define MAX_Z 17

void loadTileFn(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
  h->loadOsmTile(target, z, tilex, tiley, offsetx, offsety);
}

void OswAppMap::loop(OswHal* hal) {
  static uint16_t counter = 0;
  counter++;

  if (hal->btn2Down() && z > MIN_Z) {
    z--;
  }

  if (hal->btn3Down() && z < MAX_Z) {
    z++;
  }

  float lat = hal->gpsLat();
  float lon = hal->gpsLon();
  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();
  drawTiles(gfx, (loadTile)loadTileFn, lat, lon, z);

  if (!hal->hasGPS()) {
    gfx->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  } else if (hal->hasGPSFix()) {
    gfx->fillCircle(120, 120, 3, rgb565(0, 255, 0));
  } else {
    gfx->fillCircle(120, 120, 3, rgb565(0, 0, 255));
  }
}
