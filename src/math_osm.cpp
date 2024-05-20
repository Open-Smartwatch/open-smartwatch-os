#include "math_osm.h"

#include <Arduino.h>

// source: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#C.2FC.2B.2B

// we return float here, because we need the fraction

float lon2tilex(float lon, uint8_t z) {
    return (lon + 180.0f) / 360.0f * (float)(1 << z);
}

float lat2tiley(float lat, uint8_t z) {
    float latrad = lat * PI / 180.0f;
    return (1.0f - asinh(tanf(latrad)) / PI) / 2.0f * (float)(1 << z);
}

// helper function to get the offset within the tile
int32_t tileOffset(float tilex) {
    int32_t decimalPlaces = (int32_t)tilex;
    return (int32_t)(255 * (tilex - decimalPlaces));
}

float tilex2lon(float x, uint8_t z) {
    return x / (float)(1 << z) * 360.0f - 180.0f;
}

float tiley2lat(float y, uint8_t z) {
    float n = PI - TWO_PI * y / (float)(1 << z);
    return 180.0f / PI * atanf(0.5f * (expf(n) - expf(-n)));
}

float osmResolution[] = {156543.03f, 78271.52f, 39135.76f, 19567.88f, 9783.94f, 4891.97f, 2445.98f, 1222.99f, 611.50f, 305.75f,
                         152.87f,    76.43f,    38.21f,    19.10f,    9.55f,    4.77f,    2.38f,    1.19f,    0.59f
                        };

float getTileResolution(float lat, uint8_t z) {
    return 156543.03f /*meters/pixel*/ * cosf(lat) / (2 ^ z);
}
