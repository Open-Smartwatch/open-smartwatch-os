
#include "./apps/main/map.h"

#include <FS.h>
#include <SD.h>
#include <anim_water_ripple.h>
#include <gfx_2d_print.h>
#include <gfx_util.h>
#include <osm_render.h>
#include <osw_app.h>
#include <osw_hal.h>

#ifdef PROGMEM_TILES
// see 00_create_headers.sh
#include "./assets/maps/progmem_tiles.h"
#endif

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

#define BUF_W 240
#define BUF_H 240

#define BUF_LEN 12
#define SAT_BOX_W BUF_W / NMEAGPS_MAX_SATELLITES

// Graphics2D* tileBuffer;
BufferedTile** tileBuffer;

void OswAppMap::setup() {
  Serial.print("TotalBytes:");
  Serial.println(SD.totalBytes());
  Serial.print("UsedBytes:");
  Serial.println(SD.usedBytes());

  // tileBuffer = new Graphics2D(240, 240, 4, true);
  tileBuffer = new BufferedTile*[BUF_LEN];
  for (uint8_t i = 0; i < BUF_LEN; i++) {
    tileBuffer[i] = new BufferedTile(true /* inPsram */);
  }

  for (uint8_t i = 0; i < NMEAGPS_MAX_SATELLITES; i++) {
    _satellites[i].azimuth = 0;
    _satellites[i].elevation = 0;
    _satellites[i].id = i;
    _satellites[i].snr = 0;
    _satellites[i].tracked = 0;
  }

  OswHal::getInstance()->gpsFullOnGpsGlonassBeidu();
}

uint16_t z = 0;
#define MIN_Z 0
#define MAX_Z 2

void loadTileFn(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
#ifdef PROGMEM_TILES
  if (z < 3) {
    Serial.print("loading from progmem ");
    Serial.print(z);
    Serial.print(" ");
    Serial.print(tilex);
    Serial.print(" ");
    Serial.println(tiley);

    unsigned int dataLen = 0;
    const unsigned char* data;
    data = getProgmemTilePng(z, tilex, tiley, &dataLen);

    Serial.print("tile size: ");
    Serial.println(dataLen);
    if (dataLen != 0) {
      Serial.println("found tile");
      OswHal::getInstance()->loadPNGfromProgmem(target, data, dataLen);
      return;
    }
  }
#endif
  OswHal::getInstance()->loadOsmTile(target, z, tilex, tiley, offsetx, offsety);
}

void OswAppMap::drawSatelliteOverlay() {
  OswHal* hal = OswHal::getInstance();
  Graphics2DPrint* gfx = hal->gfx();

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

void OswAppMap::drawDataOverlay() {
  Graphics2DPrint* gfx = OswHal::getInstance()->gfx();
  static double hdg = 0.0;
  static double speed_kph = 0.0;
  static double altitude_m = 0.0;
  static double latErr = 0.0;
  static double lonErr = 0.0;
  static double altErr = 0.0;

  gfx->setTextLeftAligned();
  gfx->setTextTopAligned();
  gfx->setTextSize(2);
  gfx->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));
  gfx->setTextCursor(30, BUF_H / 5);
  gfx->print(String("Speed(kph): ") + String(speed_kph, 2));
  gfx->setTextCursor(30, BUF_H / 5 + 16);
  gfx->print(String("Alt.(m): ") + String(altitude_m, 2));
  gfx->setTextCursor(30, BUF_H / 5 + 16 * 2);
  gfx->print(String("Hdg.: ") + String(hdg, 2));
  gfx->setTextCursor(30, BUF_H / 5 + 16 * 3);
  gfx->print(String("Lat.Err(m): ") + String(latErr, 2));
  gfx->setTextCursor(30, BUF_H / 5 + 16 * 4);
  gfx->print(String("Lon.Err(m): ") + String(lonErr, 2));
  gfx->setTextCursor(30, BUF_H / 5 + 16 * 5);
  gfx->print(String("Alt.Err(m): ") + String(altErr, 2));

  OswHal* hal = OswHal::getInstance();
  if (hal->hasGPSFix()) {
    speed_kph = hal->gpsFix()->speed_kph();
    hdg = hal->gpsFix()->heading();
    altitude_m = hal->gpsFix()->altitude_cm() / 100.0;
    latErr = hal->gpsFix()->lat_err();
    lonErr = hal->gpsFix()->lon_err();
    altErr = hal->gpsFix()->alt_err();
  }
}

void OswAppMap::loop() {
  static float lat = 0;
  static float lon = 0;
  static uint8_t overlay = 0;

  OswHal* hal = OswHal::getInstance();
  Graphics2DPrint* gfx = hal->gfx();

  if (hal->btnHasGoneDown(BUTTON_2) && z > MIN_Z) {
    z--;
  }

  if (hal->btnHasGoneDown(BUTTON_3) && z < MAX_Z) {
    z++;
  }

  if (hal->btnHasGoneDown(BUTTON_1)) {
    overlay++;
    overlay = overlay > 2 ? 0 : overlay;
  }

  // update location if there is data
  if (hal->gpsLat() != 0 && hal->gpsLon() != 0) {
    lat = hal->gpsLat();
    lon = hal->gpsLon();
  }

  gfx->fill(rgb565(0, 0, 0));

  hal->getCanvas()->setTextColor(rgb565(255, 255, 255));
  hal->getCanvas()->setCursor(20, 120);

  drawTilesBuffered(tileBuffer, BUF_LEN, gfx, (loadTile)loadTileFn, lat, lon, z);

  if (!hal->hasGPS()) {
    gfx->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  } else if (hal->hasGPSFix()) {
    gfx->fillCircle(120, 120, 3, rgb565(0, 255, 0));
  } else {
    gfx->fillCircle(120, 120, 3, rgb565(0, 0, 255));
  }

  hal->requestFlush();

  switch (overlay) {
    case 1:
      drawSatelliteOverlay();
      break;
    case 2:
      drawDataOverlay();
      break;
  }
}

void OswAppMap::stop() {
  for (uint8_t i = 0; i < BUF_LEN; i++) {
    delete tileBuffer[i];
  }
  delete[] tileBuffer;

  OswHal::getInstance()->gpsBackupMode();
}

#endif
