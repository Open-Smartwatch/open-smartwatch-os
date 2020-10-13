#include "anim_doom_fire.h"

#include "gfx_2d.h"
#include "gfx_util.h"
// see: https://p3dt.net/post/2019/01/05/playing-with-doom.html

const uint16_t doomColorMap[36] = {
    //
    rgb565(0x00, 0x00, 0x00),  // #000000
    rgb565(0x1f, 0x07, 0x07),  // #1f0707
    rgb565(0x2f, 0x0f, 0x07),  // #2f0f07
    rgb565(0x47, 0x0f, 0x07),  // #470f07
    rgb565(0x57, 0x17, 0x07),  // #571707
    rgb565(0x67, 0x1f, 0x07),  // #671f07
    rgb565(0x77, 0x1f, 0x07),  // #771f07
    rgb565(0x8f, 0x27, 0x07),  // #8f2707
    rgb565(0x9f, 0x2f, 0x07),  // #9f2f07
    rgb565(0xaf, 0x3f, 0x07),  // #af3f07
    rgb565(0xbf, 0x47, 0x07),  // #bf4707
    rgb565(0xc7, 0x47, 0x07),  // #c74707
    rgb565(0xDF, 0x4F, 0x07),  // #DF4F07
    rgb565(0xDF, 0x57, 0x07),  // #DF5707
    rgb565(0xDF, 0x57, 0x07),  // #DF5707
    rgb565(0xD7, 0x5F, 0x07),  // #D75F07
    rgb565(0xD7, 0x67, 0x0F),  // #D7670F
    rgb565(0xcf, 0x6f, 0x0f),  // #cf6f0f
    rgb565(0xcf, 0x77, 0x0f),  // #cf770f
    rgb565(0xcf, 0x7f, 0x0f),  // #cf7f0f
    rgb565(0xCF, 0x87, 0x17),  // #CF8717
    rgb565(0xC7, 0x87, 0x17),  // #C78717
    rgb565(0xC7, 0x8F, 0x17),  // #C78F17
    rgb565(0xC7, 0x97, 0x1F),  // #C7971F
    rgb565(0xBF, 0x9F, 0x1F),  // #BF9F1F
    rgb565(0xBF, 0x9F, 0x1F),  // #BF9F1F
    rgb565(0xBF, 0xA7, 0x27),  // #BFA727
    rgb565(0xBF, 0xA7, 0x27),  // #BFA727
    rgb565(0xBF, 0xAF, 0x2F),  // #BFAF2F
    rgb565(0xB7, 0xAF, 0x2F),  // #B7AF2F
    rgb565(0xB7, 0xB7, 0x2F),  // #B7B72F
    rgb565(0xB7, 0xB7, 0x37),  // #B7B737
    rgb565(0xCF, 0xCF, 0x6F),  // #CFCF6F
    rgb565(0xDF, 0xDF, 0x9F),  // #DFDF9F
    rgb565(0xEF, 0xEF, 0xC7),  // #EFEFC7
    rgb565(0xFF, 0xFF, 0xFF)   // #FFFFFF
};

void setupFire(uint8_t **firePixels, uint16_t w, uint16_t h) {
  for (uint8_t y = 0; y < h; y++) {
    for (uint8_t x = 0; x < w; x++) {
      // last row is hot
      firePixels[y][x] = y == h - 1 ? 35 : 0;
    }
  }
}

void calcFire(uint8_t **firePixels, uint16_t w, uint16_t h) {
  for (uint8_t y = 0; y < h - 1; y++) {
    for (uint8_t x = 0; x < w; x++) {
      uint8_t wind = x + random(2);
      wind = wind >= w ? wind - w : wind;
      uint8_t speed = y + random(2);
      speed = speed >= h ? h - 1 : speed;
      firePixels[y][x] = firePixels[speed][wind] - random(2);
      firePixels[y][x] = firePixels[y][x] > 35 ? 0 : firePixels[y][x];  // fix overflow
    }
  }
}

void mapFire(uint8_t **firePixels, uint16_t fireW, uint16_t fireH,  //
             Graphics2D *graphics2d, uint16_t offsetX, uint16_t offsetY) {
  for (uint8_t x = 0; x < fireW; x++) {
    for (uint8_t y = 0; y < fireH; y++) {
      graphics2d->drawPixel(x + offsetX, y + offsetY, doomColorMap[firePixels[y][x]]);
    }
  }
}
