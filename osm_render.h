#ifndef OSM_RENDER_H
#define OSM_RENDER_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_2d.h"
#include "math_osm.h"

#define TILE_W 256
#define TILE_H 256
#define TILE_CHUNK_H 16

typedef void (*loadTile)(Graphics2D *target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety);

class BufferedTile {
 public:
  BufferedTile(bool inPsram) {
    gfx = new Graphics2D(TILE_W, TILE_H, TILE_CHUNK_H, false /* not round */, inPsram /* but in psram*/);
    lastUsed = 0;
  };
  ~BufferedTile() { delete gfx; }

  void loadTile(loadTile loadTileFn, uint32_t tileX, uint32_t tileY, uint8_t tileZ) {
    loadTileFn(gfx, tileZ, tileX, tileY, 0, 0);
    _tileX = tileX;
    _tileY = tileY;
    _tileZ = tileZ;
    lastUsed = millis();
  }

  bool hasTile(uint32_t tileX, uint32_t tileY, uint8_t tileZ) {
    return tileZ == _tileZ && tileX == _tileX && tileY == _tileY && lastUsed != 0;
  }

  Graphics2D *getGraphics() {
    lastUsed = millis();
    return gfx;
  }

  unsigned long getLastUsed() { return lastUsed; }

 private:
  Graphics2D *gfx;
  uint8_t _tileZ;
  uint32_t _tileX;
  uint32_t _tileY;
  unsigned long lastUsed;
};

void drawTiles(Graphics2D *target, loadTile loadTileFn, float lat, float lon, uint8_t z);
void drawTilesBuffered(BufferedTile **buffer, uint8_t bufferLength, Graphics2D *target,  //
                       loadTile loadTileFn, float lat, float lon, uint8_t z);
#endif
