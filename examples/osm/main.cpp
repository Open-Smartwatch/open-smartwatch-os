#include <stdint.h>
#include <stdio.h>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../osm_render.h"

// full screen bufffer
#define BUF_W 240
#define BUF_H 240

// we buffer a quarter tile + 50% and upscale it by a factor of 2x
// this way we greatly reduce the number of reads from the uSD to load new tiles
// when moving around, and reduce memory consumption
#define MAP_BUF_W 255
#define MAP_BUF_H 255

Graphics2D gfx2d(BUF_W, BUF_H, 16);

Graphics2D tileBuffer(MAP_BUF_W, MAP_BUF_H, 5);

void loadTileFn(Graphics2D* target, int8_t z, float tilex, float tiley, int32_t offsetx, int32_t offsety) {
  int16_t xStart = max(0, offsetx);
  int16_t xEnd = min(target->getWidth(), target->getWidth() + offsetx);
  int16_t yStart = max(0, offsety);
  int16_t yEnd = min(target->getHeight(), target->getHeight() + offsety);
  // printf("xStart %d, xEnd %d, yStart %d, yEnd %d", xStart, xEnd, yStart, yEnd);
  uint8_t r = 0;
  uint8_t g = 0;
  uint8_t b = 0;

  Graphics2D* tile = new Graphics2D(256, 256, 16);
  const char* path = ("/Volumes/TILEDISK/map/" + std::to_string(z) + "/" + std::to_string((int)tilex) + "/" +
                      std::to_string((int)tiley) + ".png")
                         .c_str();
  loadPNG(tile, path);

  for (int16_t x = xStart; x < xEnd; x++) {
    for (int16_t y = yStart; y < yEnd; y++) {
      r++;
      g++;
      uint16_t color = rgb565(r, g, b);
      // TODO: optimize for offset
      target->drawPixel(x, y, tile->getPixel(x - offsetx, y - offsety));
    }
    b++;
  }
}

float lat = 50.76;
float lon = 4.21;
uint16_t z = 10;

class RotationExampleWindow : public SDLWindowRGB565 {
 public:
  RotationExampleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {}

  void loop() {
    static uint16_t counter = 1;
    counter++;

    drawTiles(&tileBuffer, (loadTile)loadTileFn, lat, lon, z);
    gfx2d.drawGraphics2D(0, 0, &tileBuffer);

    delay(1000 / 30);
    lat += 0.001;
    lon += 0.001;

    // if (counter % 100 == 0) {
    //   z++;
    //   if (z == 17) {
    //     z = 6;
    //   }
    // }
  }
};

int main(int argc, char* argv[]) {
  RotationExampleWindow exampleWindow(&gfx2d, BUF_W, BUF_H);
  exampleWindow.run();
  return 0;
}
