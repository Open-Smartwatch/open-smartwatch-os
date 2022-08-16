#include "osm_render.h"

#include "gfx_2d.h"

// #include <stdio.h>

// Memory estimation in bytes:
// 256*256*2=131.072
// 2x2 = 524.288
// 3x3 = 1.179.648
// 4x4 = 2.097.152
// 5x5 = 3.276.800

Graphics2D *getTile(BufferedTile **buffer, uint8_t bufferLength, loadTile loadTileFn, uint32_t tileX, uint32_t tileY,
                    uint8_t tileZ) {
  // return buffered tile
  for (uint16_t i = 0; i < bufferLength; i++) {
    if (buffer[i] != NULL && buffer[i]->hasTile(tileX, tileY, tileZ)) {
      return buffer[i]->getGraphics();
    }
  }

  // find oldest tile
  unsigned long oldestTimeStamp = 4294967295;
  uint16_t oldestIndex = 0;
  for (uint16_t i = 0; i < bufferLength; i++) {
    if (buffer[i] != NULL && buffer[i]->getLastUsed() < oldestTimeStamp) {
      oldestTimeStamp = buffer[i]->getLastUsed();
      oldestIndex = i;
    }
  }

  // overwrite withe new tile
  buffer[oldestIndex]->loadTile(loadTileFn, tileX, tileY, tileZ);

  // return fresh tile
  return buffer[oldestIndex]->getGraphics();
}

void drawTilesBuffered(BufferedTile **buffer, uint8_t n, Graphics2D *target,  //
                       loadTile loadTileFn, float lat, float lon, uint8_t z) {
  // TODO: proxy loadTileFn and reuse buffered tile if present
  float tileX = lon2tilex(lon, z);
  float tileY = lat2tiley(lat, z);
  uint16_t zoom = z;

  int32_t tposX = target->getWidth() / 2 - tileOffset(tileX);
  int32_t tposY = target->getHeight() / 2 - tileOffset(tileY);

  // getTile(buffer, n, loadTileFn, tileX, tileY, zoom)->fillFrame(0, 0, 255, 255, rgb565(255, 0, 0));

  target->drawGraphics2D(tposX, tposY, getTile(buffer, n, loadTileFn, tileX, tileY, zoom));

  // TODO below is not optimal, we have cases where nothing needs to be drawn
  if (tileOffset(tileX) < 128 && tileOffset(tileY) < 128) {
    // top left (first tile is bot right)
    target->drawGraphics2D(tposX - TILE_W, tposY /*    */, getTile(buffer, n, loadTileFn, tileX - 1, tileY, zoom));
    target->drawGraphics2D(tposX - TILE_W, tposY - TILE_H, getTile(buffer, n, loadTileFn, tileX - 1, tileY - 1, zoom));
    target->drawGraphics2D(tposX /*    */, tposY - TILE_H, getTile(buffer, n, loadTileFn, tileX, tileY - 1, zoom));
  } else if (tileOffset(tileX) < 128 && tileOffset(tileY) >= 128) {
    // bot left (first tile is top right)
    target->drawGraphics2D(tposX - TILE_W, tposY /*    */, getTile(buffer, n, loadTileFn, tileX - 1, tileY, zoom));
    target->drawGraphics2D(tposX - TILE_W, tposY + TILE_H, getTile(buffer, n, loadTileFn, tileX - 1, tileY + 1, zoom));
    target->drawGraphics2D(tposX /*    */, tposY + TILE_H, getTile(buffer, n, loadTileFn, tileX, tileY + 1, zoom));
  } else if (tileOffset(tileX) >= 128 && tileOffset(tileY) >= 128) {
    // bot right (first tile is top left)
    target->drawGraphics2D(tposX /*    */, tposY + TILE_H, getTile(buffer, n, loadTileFn, tileX, tileY + 1, zoom));
    target->drawGraphics2D(tposX + TILE_W, tposY + TILE_H, getTile(buffer, n, loadTileFn, tileX + 1, tileY + 1, zoom));
    target->drawGraphics2D(tposX + TILE_W, tposY /*    */, getTile(buffer, n, loadTileFn, tileX + 1, tileY, zoom));
  } else {
    // top right (first tile is bot left)
    target->drawGraphics2D(tposX + TILE_W, tposY /*    */, getTile(buffer, n, loadTileFn, tileX + 1, tileY, zoom));
    target->drawGraphics2D(tposX + TILE_W, tposY - TILE_H, getTile(buffer, n, loadTileFn, tileX + 1, tileY - 1, zoom));
    target->drawGraphics2D(tposX /*    */, tposY - TILE_H, getTile(buffer, n, loadTileFn, tileX, tileY - 1, zoom));
  }
}

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
}
