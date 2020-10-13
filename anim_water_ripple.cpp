#include "anim_water_ripple.h"

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif


#include "gfx_2d.h"
#include "gfx_util.h"

// helper to calculate position in the buffer
#define a(x, y, width) (x) + ((y)*width)

// implementation according to:
// https://web.archive.org/web/20160418004149/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm

void calcWater(int8_t* buf1, int8_t* buf2, uint16_t width, uint16_t height, float damping /* [0,1]*/) {
  // for every non-edge element

  // this could be faster, but this is easier to understand:
  for (uint16_t x = 1; x < width - 1; x++) {
    for (uint16_t y = 1; y < height - 1; y++) {
      int16_t velocity = -buf2[a(x, y, width)];
      int16_t smoothed = (buf1[a(x - 1, y, width)]    //
                          + buf1[a(x + 1, y, width)]  //
                          + buf1[a(x, y + 1, width)]  //
                          + buf1[a(x, y - 1, width)]) /
                         2;
      buf2[a(x, y, width)] = (smoothed + velocity) * damping;
    }
  }
}

void mapWater(int8_t* buf, uint16_t width, uint16_t height, Graphics2D* background, Graphics2D* target,
              uint16_t offsetX, uint16_t offsetY) {
  for (uint16_t x = 1; x < width - 1; x++) {
    for (uint16_t y = 1; y < height - 1; y++) {
      int16_t xShift = x + (buf[a(x - 1, y, width)] - buf[a(x + 1, y, width)]) / 2;
      int16_t yShift = y + (buf[a(x, y - 1, width)] - buf[a(x, y + 1, width)]) / 2;

      xShift = xShift >= width ? width - 1: xShift;
      xShift = xShift < 0 ? 0 : xShift;
      yShift = yShift >= height ? height - 1: yShift;
      yShift = yShift < 0 ? 0 : yShift;
      int16_t shiftedColor = background->getPixel(xShift, yShift);
      target->drawPixel(x, y, shiftedColor);
    }
  }
}
