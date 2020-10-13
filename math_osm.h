#ifndef OSM_H
#define OSM_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

// source: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B

// we return float here, because we need the fraction

float lon2tilex(float lon, uint8_t z);

float lat2tiley(float lat, uint8_t z);

// helper function to get the offset within the tile
int32_t tileOffset(float tilex);

float tilex2lon(float x, uint8_t z);

float tiley2lat(float y, uint8_t z);

float getTileResolution(float lat, uint8_t z);

#endif