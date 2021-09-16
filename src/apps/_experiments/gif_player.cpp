
#include "./apps/_experiments/gif_player.h"

#include <AnimatedGIF.h>
#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>

// #include "assets/mwdu.h"
// #define GIF_NAME mwdu_gif

#include "assets/ltt2.h"
#define GIF_NAME ltt2_mov_gif

AnimatedGIF gif;

// handle which is used by GIFDraw callback below
Graphics2D* gfx;
int gfxOffsetX = 0;
int gfxOffsetY = 0;

// callback to draw the gif to the gfx above
void GIFDraw(GIFDRAW* pDraw) {
  uint8_t* s;
  uint16_t *d, *usPalette, usTemp[320];
  int x, y;

  usPalette = pDraw->pPalette;
  y = pDraw->iY + pDraw->y;  // current line

  s = pDraw->pPixels;
  if (pDraw->ucDisposalMethod == 2)  // restore to background color
  {
    for (x = 0; x < pDraw->iWidth; x++) {
      if (s[x] == pDraw->ucTransparent) s[x] = pDraw->ucBackground;
    }
    pDraw->ucHasTransparency = 0;
  }
  // Apply the new pixels to the main image
  if (pDraw->ucHasTransparency)  // if transparency used
  {
    uint8_t *pEnd, c, ucTransparent = pDraw->ucTransparent;
    int x, iCount;
    pEnd = s + pDraw->iWidth;
    x = 0;
    iCount = 0;  // count non-transparent pixels
    while (x < pDraw->iWidth) {
      c = ucTransparent - 1;
      d = usTemp;
      while (c != ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent)  // done, stop
        {
          s--;  // back up to treat it like transparent
        } else  // opaque
        {
          *d++ = usPalette[c];
          iCount++;
        }
      }            // while looking for opaque pixels
      if (iCount)  // any opaque pixels?
      {
        for (int xOffset = 0; xOffset < iCount; xOffset++) {
          gfx->drawPixel(x + xOffset + gfxOffsetX, y + gfxOffsetY, usTemp[xOffset]);
          // dma_display.drawPixelRGB565(x + xOffset, y, usTemp[xOffset]);
        }
        x += iCount;
        iCount = 0;
      }
      // no, look for a run of transparent pixels
      c = ucTransparent;
      while (c == ucTransparent && s < pEnd) {
        c = *s++;
        if (c == ucTransparent)
          iCount++;
        else
          s--;
      }
      if (iCount) {
        x += iCount;  // skip these
        iCount = 0;
      }
    }
  } else {
    s = pDraw->pPixels;
    // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
    for (x = 0; x < pDraw->iWidth; x++) {
      gfx->drawPixel(x + gfxOffsetX, y + gfxOffsetY, usPalette[*s++]);
      // dma_display.drawPixelRGB565(x, y, usPalette[*s++]);
    }
  }
}

bool gifOpen = false;

void OswAppGifPlayer::setup() {
  gfx = OswHal::getInstance()->gfx();
  gif.begin(LITTLE_ENDIAN_PIXELS);
  if (gif.open((uint8_t*)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    gifOpen = true;
  }
  gfx->fill(rgb565(0, 0, 0));
}

void OswAppGifPlayer::loop() {
  if (gifOpen) {
    if (!gif.playFrame(true, NULL)) {
      gif.reset();
    }
  } else {
    OswHal::getInstance()->gfx()->setTextCursor(40, 100);
    OswHal::getInstance()->gfx()->print("GIF error!");
  }

  // this app has something to display, request a flush to the screen
  OswHal::getInstance()->requestFlush();
}

void OswAppGifPlayer::stop() {
  if (gifOpen) {
    gif.close();
  }
}
