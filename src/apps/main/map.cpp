
#include "./apps/main/map.h"

#include <FS.h>
#include <SD.h>
#include <anim_water_ripple.h>
#include <gfx_2d_print.h>
#include <gfx_util.h>
#include <osm_render.h>
#include <osw_app.h>
#include <osw_hal.h>
#if defined(GPS_EDITION)

#define BUF_W 240
#define BUF_H 240

#define BUF_LEN 12
#define SAT_BOX_W BUF_W / NMEAGPS_MAX_SATELLITES

// Graphics2D* tileBuffer;
OswHal* h;
BufferedTile** tileBuffer;

void OswAppMap::setup(OswHal* hal) {
  // required for loadTileFn
  h = hal;

  Serial.print("TotalBytes:");
  Serial.println(SD.totalBytes());
  Serial.print("UsedBytes:");
  Serial.println(SD.usedBytes());

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

  if (hal->btnHasGoneDown(BUTTON_2) && z > MIN_Z) {
    z--;
  }

  if (hal->btnHasGoneDown(BUTTON_3) && z < MAX_Z) {
    z++;
  }

  if (hal->btnHasGoneDown(BUTTON_1)) {
    _drawSats = !_drawSats;
    Serial.println("toggle stats");
  }

  float lat = hal->gpsLat();
  float lon = hal->gpsLon();

  // demo if no location
  lat = lat < 1 ? 50.839 : lat;
  lon = lon < 1 ? 4.384 : lon;

  Graphics2DPrint* gfx = hal->getCanvas()->getGraphics2D();
  gfx->fill(rgb565(0, 0, 0));

  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(20, 120);
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

  if (_drawSats) {
    // copy over sat stats so the screan doesn't flicker as gps() clears stats
    if (hal->gps()->nmeaMessage != NMEAGPS::NMEA_GSV && hal->gps()->nmeaMessage != NMEAGPS::NMEA_GSA &&
        hal->gps()->sat_count) {
      _sat_count = hal->gps()->sat_count;
      for (uint8_t i = 0; i < hal->gps()->sat_count; i++) {
        _satellites[i].azimuth = hal->gps()->satellites[i].azimuth;
        _satellites[i].elevation = hal->gps()->satellites[i].elevation;
        _satellites[i].id = hal->gps()->satellites[i].id;
        _satellites[i].snr = hal->gps()->satellites[i].snr;
        _satellites[i].tracked = hal->gps()->satellites[i].tracked;
      }
    }

    gfx->setTextTopAligned();
    gfx->setTextCenterAligned();
    for (uint8_t i = 0; i < _sat_count; i++) {
      uint8_t h = _satellites[i].snr;
      gfx->fillFrame(SAT_BOX_W * i, (BUF_H / 2) - h, SAT_BOX_W, h,
                     h > 45   ? rgb565(0, 255, 0)    // high is green
                     : h < 25 ? rgb565(255, 0, 0)    // low is red
                              : rgb565(0, 0, 255));  // other is blue
      h = h * 2;                                     // scale to 100% because > 50 is not expected
      h = h > 99 ? 99 : h;                           // catch overflow
      gfx->setTextCursor(SAT_BOX_W * i, BUF_H / 2);
      // blue on green when tracked, white on black nontracked
      gfx->setTextColor(_satellites[i].tracked ? rgb565(0, 0, 255) : rgb565(255, 255, 255),
                        rgb565(0, _satellites[i].tracked ? 255 : 0, 0));
      gfx->println(_satellites[i].id);
      gfx->setTextCursor(SAT_BOX_W * i + SAT_BOX_W / 2, BUF_H / 2 + 10);
      gfx->println(_satellites[i].azimuth);
      gfx->setTextCursor(SAT_BOX_W * i + SAT_BOX_W / 2, BUF_H / 2 + 20);
      gfx->println(_satellites[i].elevation);
      gfx->setTextCursor(SAT_BOX_W * i + SAT_BOX_W / 2, BUF_H / 2 - (h / 2 + 10));
      gfx->println(_satellites[i].snr);
    }
    // for (uint8_t i = 0; i < NMEAGPS_MAX_SATELLITES; i++) {
    //   gfx->drawFrame(SAT_BOX_W * i, (BUF_H / 2) - 100, SAT_BOX_W, 100, rgb565(0, 255, 0));
    // }

    gfx->setTextTopAligned();
    gfx->setTextCenterAligned();
    gfx->setTextCursor(BUF_W / 2, 200);
    gfx->setTextSize(3);
    gfx->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
    gfx->println(_sat_count);
  }
}

void OswAppMap::stop(OswHal* hal) {
  for (uint8_t i = 0; i < BUF_LEN; i++) {
    delete tileBuffer[i];
  }
  delete[] tileBuffer;
}

#endif
