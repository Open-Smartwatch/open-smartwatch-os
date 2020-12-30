#include "osm_render.h"

// #include <stdio.h>

#define TILE_W 256
#define TILE_H 256

void drawTiles(Graphics2D *target, loadTile loadTileFn, float lat, float lon, uint8_t z) {
  float tileX = lon2tilex(lon, z);
  float tileY = lat2tiley(lat, z);
  uint16_t zoom = z;

  int32_t tposX = target->getWidth() / 2 - tileOffset(tileX);
  int32_t tposY = target->getHeight() / 2 - tileOffset(tileY);
  // target->fill(rgb565(0, 0, 0));

  loadTileFn(target, zoom, tileX, tileY, tposX, tposY);
  // target->drawFrame(tposX, tposY, 256, 256, rgb565(255, 0, 0));
  // TODO below is not optimal, we have cases where nothing needs to be drawn
  if (tileOffset(tileX) < 128 && tileOffset(tileY) < 128) {
    // top left (first tile is bot right)
    loadTileFn(target, zoom, tileX - 1, tileY, tposX - TILE_W, tposY);
    loadTileFn(target, zoom, tileX - 1, tileY - 1, tposX - TILE_W, tposY - TILE_H);
    loadTileFn(target, zoom, tileX, tileY - 1, tposX, tposY - TILE_H);
    // target->drawFrame(200, 200, 10, 10, rgb565(255, 0, 0));
  } else if (tileOffset(tileX) < 128 && tileOffset(tileY) >= 128) {
    // bot left (first tile is top right)
    loadTileFn(target, zoom, tileX - 1, tileY, tposX - TILE_W, tposY);
    loadTileFn(target, zoom, tileX - 1, tileY + 1, tposX - TILE_W, tposY + TILE_H);
    loadTileFn(target, zoom, tileX, tileY + 1, tposX, tposY + TILE_H);
    // target->drawFrame(200, 40, 10, 10, rgb565(255, 0, 0));
  } else if (tileOffset(tileX) >= 128 && tileOffset(tileY) >= 128) {
    // bot right (first tile is top left)
    loadTileFn(target, zoom, tileX, tileY + 1, tposX, tposY + TILE_H);
    loadTileFn(target, zoom, tileX + 1, tileY + 1, tposX + TILE_W, tposY + TILE_H);
    loadTileFn(target, zoom, tileX + 1, tileY, tposX + TILE_W, tposY);
    // target->drawFrame(40, 40, 10, 10, rgb565(255, 0, 0));
  } else {
    // top right (first tile is bot left)
    loadTileFn(target, zoom, tileX + 1, tileY, tposX + TILE_W, tposY);
    loadTileFn(target, zoom, tileX + 1, tileY - 1, tposX + TILE_W, tposY - TILE_H);
    loadTileFn(target, zoom, tileX, tileY - 1, tposX, tposY - TILE_H);
    // target->drawFrame(40, 200, 10, 10, rgb565(255, 0, 0));
  }
  // printf("%f, %f, %d, %d, %d\n", lat, lon, zoom, tileOffset(tilex), tileOffset(tiley));
}