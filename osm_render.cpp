#include "osm_render.h"

#define TILE_W 256
#define TILE_H 256

void drawTiles(Graphics2D *target, loadTile loadTileFn, float lat, float lon, uint8_t z) {
  float tilex = lon2tilex(lat, z);
  float tiley = lat2tiley(lon, z);
  uint16_t zoom = z;

  uint16_t centerX = target->getWidth() / 2;
  uint16_t centerY = target->getHeight() / 2;

  loadTileFn(target, zoom, tilex, tiley, centerX, centerY);
  // TODO below is not optimal, we have cases where nothing needs to be drawn
  if (tileOffset(tilex) < 128 && tileOffset(tiley) < 128) {
    // top left
    loadTileFn(target, zoom, tilex - 1, tiley, centerX - TILE_W, centerY);
    loadTileFn(target, zoom, tilex - 1, tiley - 1, centerX - TILE_W, centerY - TILE_H);
    loadTileFn(target, zoom, tilex, tiley - 1, centerX, centerY - TILE_H);
  } else if (tileOffset(tilex) < 128 && tileOffset(tiley) >= 128) {
    // bot left
    loadTileFn(target, zoom, tilex - 1, tiley, centerX - TILE_W, centerY);
    loadTileFn(target, zoom, tilex - 1, tiley + 1, centerX - TILE_W, centerY + TILE_H);
    loadTileFn(target, zoom, tilex, tiley + 1, centerX, centerY + TILE_H);
  } else if (tileOffset(tilex) >= 128 && tileOffset(tiley) >= 128) {
    // bot right
    loadTileFn(target, zoom, tilex, tiley + 1, centerX, centerY + TILE_H);
    loadTileFn(target, zoom, tilex + 1, tiley + 1, centerX + TILE_W, centerY + TILE_H);
    loadTileFn(target, zoom, tilex + 1, tiley, centerX + TILE_W, centerY);
  } else {
    // top right
    loadTileFn(target, zoom, tilex + 1, tiley, centerX + TILE_W, centerY);
    loadTileFn(target, zoom, tilex + 1, tiley - 1, centerX + TILE_W, centerY - TILE_H);
    loadTileFn(target, zoom, tilex, tiley - 1, centerX, centerY - TILE_H);
  }
}