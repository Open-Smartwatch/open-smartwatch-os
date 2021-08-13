#ifndef PROGMEM_TILES_H
#define PROGMEM_TILES_H
#include "osm_0_0_0_png.h"
#include "osm_1_0_0_png.h"
#include "osm_1_0_1_png.h"
#include "osm_1_1_0_png.h"
#include "osm_1_1_1_png.h"
#include "osm_2_0_0_png.h"
#include "osm_2_0_1_png.h"
#include "osm_2_0_2_png.h"
#include "osm_2_0_3_png.h"
#include "osm_2_1_0_png.h"
#include "osm_2_1_1_png.h"
#include "osm_2_1_2_png.h"
#include "osm_2_1_3_png.h"
#include "osm_2_2_0_png.h"
#include "osm_2_2_1_png.h"
#include "osm_2_2_2_png.h"
#include "osm_2_2_3_png.h"
#include "osm_2_3_0_png.h"
#include "osm_2_3_1_png.h"
#include "osm_2_3_2_png.h"
#include "osm_2_3_3_png.h"
const unsigned char* getProgmemTilePng(unsigned int z, unsigned int x, unsigned int y, unsigned int* len) {
    if (z == 0 && x == 0 && y == 0) { *len = osm_0_0_0_png_len; return osm_0_0_0_png;}
    if (z == 1 && x == 0 && y == 0) { *len = osm_1_0_0_png_len; return osm_1_0_0_png;}
    if (z == 1 && x == 0 && y == 1) { *len = osm_1_0_1_png_len; return osm_1_0_1_png;}
    if (z == 1 && x == 1 && y == 0) { *len = osm_1_1_0_png_len; return osm_1_1_0_png;}
    if (z == 1 && x == 1 && y == 1) { *len = osm_1_1_1_png_len; return osm_1_1_1_png;}
    if (z == 2 && x == 0 && y == 0) { *len = osm_2_0_0_png_len; return osm_2_0_0_png;}
    if (z == 2 && x == 0 && y == 1) { *len = osm_2_0_1_png_len; return osm_2_0_1_png;}
    if (z == 2 && x == 0 && y == 2) { *len = osm_2_0_2_png_len; return osm_2_0_2_png;}
    if (z == 2 && x == 0 && y == 3) { *len = osm_2_0_3_png_len; return osm_2_0_3_png;}
    if (z == 2 && x == 1 && y == 0) { *len = osm_2_1_0_png_len; return osm_2_1_0_png;}
    if (z == 2 && x == 1 && y == 1) { *len = osm_2_1_1_png_len; return osm_2_1_1_png;}
    if (z == 2 && x == 1 && y == 2) { *len = osm_2_1_2_png_len; return osm_2_1_2_png;}
    if (z == 2 && x == 1 && y == 3) { *len = osm_2_1_3_png_len; return osm_2_1_3_png;}
    if (z == 2 && x == 2 && y == 0) { *len = osm_2_2_0_png_len; return osm_2_2_0_png;}
    if (z == 2 && x == 2 && y == 1) { *len = osm_2_2_1_png_len; return osm_2_2_1_png;}
    if (z == 2 && x == 2 && y == 2) { *len = osm_2_2_2_png_len; return osm_2_2_2_png;}
    if (z == 2 && x == 2 && y == 3) { *len = osm_2_2_3_png_len; return osm_2_2_3_png;}
    if (z == 2 && x == 3 && y == 0) { *len = osm_2_3_0_png_len; return osm_2_3_0_png;}
    if (z == 2 && x == 3 && y == 1) { *len = osm_2_3_1_png_len; return osm_2_3_1_png;}
    if (z == 2 && x == 3 && y == 2) { *len = osm_2_3_2_png_len; return osm_2_3_2_png;}
    if (z == 2 && x == 3 && y == 3) { *len = osm_2_3_3_png_len; return osm_2_3_3_png;}
}
#endif
