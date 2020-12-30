#include "math_osm.h"

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

// source: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B

// we return float here, because we need the fraction

float lon2tilex(float lon, uint8_t z) { return (lon + 180.0) / 360.0 * (float)(1 << z); }

float lat2tiley(float lat, uint8_t z) {
  float latrad = lat * PI / 180.0;
  return (1.0 - asinh(tan(latrad)) / PI) / 2.0 * (float)(1 << z);
}

// helper function to get the offset within the tile
int32_t tileOffset(float tilex) {
  int32_t decimalPlaces = (int32_t)tilex;
  return (int32_t)(255 * (tilex - decimalPlaces));
}

float tilex2lon(float x, uint8_t z) { return x / (float)(1 << z) * 360.0 - 180; }

float tiley2lat(float y, uint8_t z) {
  float n = PI - TWO_PI * y / (float)(1 << z);
  return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
}

float osmResolution[] = {156543.03, 78271.52, 39135.76, 19567.88, 9783.94, 4891.97, 2445.98, 1222.99, 611.50, 305.75,
                         152.87,    76.43,    38.21,    19.10,    9.55,    4.77,    2.38,    1.19,    0.59};

float getTileResolution(float lat, uint8_t z) { return 156543.03 /*meters/pixel*/ * cos(lat) / (2 ^ z); }
