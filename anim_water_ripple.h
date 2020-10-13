#ifndef WATER_RIPPLE_H
#define WATER_RIPPLE_H

#ifdef FAKE_ARDUINO
#include "FakeArduino.h"
#else
#include <Arduino.h>
#endif

#include "gfx_2d.h"
#include "gfx_util.h"

// implementation according to:
// https://web.archive.org/web/20160418004149/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm

void calcWater(int8_t* buf1, int8_t* buf2, uint16_t width, uint16_t height, float damping /* [0,1]*/);

void mapWater(int8_t* buf, uint16_t width, uint16_t height, Graphics2D* background, Graphics2D* target,
              uint16_t offsetX, uint16_t offsetY);

#endif