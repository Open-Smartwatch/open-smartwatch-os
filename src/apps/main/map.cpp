
#include "./apps/main/map.h"

#include <anim_water_ripple.h>
#include <gfx_2d.h>
#include <gfx_util.h>
#include <osm_render.h>
#include <osw_app.h>
#include <osw_hal.h>
#if defined(GPS_EDITION)

#define BUF_W 240
#define BUF_H 240

#define BUF_LEN 12

// Graphics2D* tileBuffer;
OswHal* h;
BufferedTile** tileBuffer;

void OswAppMap::setup(OswHal* hal) {
  // required for loadTileFn
  h = hal;
  // tileBuffer = new Graphics2D(240, 240, 4, true);
  tileBuffer = new BufferedTile*[BUF_LEN];
  for (uint8_t i = 0; i < BUF_LEN; i++) {
    tileBuffer[i] = new BufferedTile(true /* inPsram */);
  }
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

  if (hal->btnIsDown(BUTTON_2) && z > MIN_Z) {
    z--;
    hal->clearButtonState(BUTTON_2);
  }

  if (hal->btnIsDown(BUTTON_3) && z < MAX_Z) {
    z++;
    hal->clearButtonState(BUTTON_3);
  }

  float lat = hal->gpsLat();
  float lon = hal->gpsLon();

  // demo if no location
  lat = lat < 1 ? 50.839 : lat;
  lon = lon < 1 ? 4.384 : lon;

  Graphics2D* gfx = hal->getCanvas()->getGraphics2D();
  gfx->fill(rgb565(0, 0, 0));

  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(20,120);
  hal->getCanvas()->print(counter);

  drawTilesBuffered(tileBuffer, BUF_LEN, gfx, (loadTile)loadTileFn, lat, lon, z);

  if (!hal->hasGPS()) {
    gfx->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  } else if (hal->hasGPSFix()) {
    gfx->fillCircle(120, 120, 3, rgb565(0, 255, 0));
  } else {
    gfx->fillCircle(120, 120, 3, rgb565(0, 0, 255));
  }

  h->requestFlush();
}

void OswAppMap::stop(OswHal* hal) {
  for (uint8_t i = 0; i < BUF_LEN; i++) {
    delete tileBuffer[i];
  }
  delete[] tileBuffer;
}

#endif
