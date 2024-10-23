#include "./apps/_experiments/gif_player.h"

#ifndef OSW_EMULATOR
#include <AnimatedGIF.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

#include "assets/mwdu.h"
#define GIF_NAME mwdu_gif

static AnimatedGIF gif;

// handle which is used by GIFDraw callback below
static Graphics2D* gfx;
static int gfxOffsetX = 0;
static int gfxOffsetY = 0;

// callback to draw the gif to the gfx above
static void GIFDraw(GIFDRAW* pDraw) {
    uint8_t* s;
    uint16_t* d, *usPalette, usTemp[320];
    int x, y;

    usPalette = pDraw->pPalette;
    y = pDraw->iY + pDraw->y;  // current line

    s = pDraw->pPixels;
    if (pDraw->ucDisposalMethod == 2) { // restore to background color
        for (x = 0; x < pDraw->iWidth; x++) {
            if (s[x] == pDraw->ucTransparent) s[x] = pDraw->ucBackground;
        }
        pDraw->ucHasTransparency = 0;
    }
    // Apply the new pixels to the main image
    if (pDraw->ucHasTransparency) { // if transparency used
        uint8_t* pEnd, c, ucTransparent = pDraw->ucTransparent;
        int x, iCount;
        pEnd = s + pDraw->iWidth;
        x = 0;
        iCount = 0; // count non-transparent pixels
        while (x < pDraw->iWidth) {
            c = ucTransparent - 1;
            d = usTemp;
            while (c != ucTransparent && s < pEnd) {
                c = *s++;
                if (c == ucTransparent) { // done, stop
                    s--; // back up to treat it like transparent
                } else { // opaque
                    *d++ = usPalette[c];
                    iCount++;
                }
            }            // while looking for opaque pixels
            if (iCount) { // any opaque pixels?
                for (int xOffset = 0; xOffset < iCount; xOffset++) {
                    gfx->drawPixel(x + xOffset + gfxOffsetX, y + gfxOffsetY, usTemp[xOffset]);
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
                x += iCount; // skip these
                iCount = 0;
            }
        }
    } else {
        s = pDraw->pPixels;
        // Translate the 8-bit pixels through the RGB565 palette (already byte reversed)
        for (x = 0; x < pDraw->iWidth; x++) {
            gfx->drawPixel(x + gfxOffsetX, y + gfxOffsetY, usPalette[*s++]);
        }
    }
}

static bool gifOpen = false;

void OswAppGifPlayer::setup() {
    gfx = OswHal::getInstance()->gfx(); // update static gfx handle for the GIFDraw callback
    gif.begin(LITTLE_ENDIAN_PIXELS);
    if (gif.open((uint8_t*)GIF_NAME, sizeof(GIF_NAME), GIFDraw)) {
        OSW_LOG_D("Successfully opened GIF; Canvas size = ", gif.getCanvasWidth(), " x ", gif.getCanvasHeight());
        gifOpen = true;
    }
    OswHal::getInstance()->gfx()->fill(rgb565(0, 0, 0));
}

void OswAppGifPlayer::loop() {
    gfx = OswHal::getInstance()->gfx(); // update static gfx handle for the GIFDraw callback
    if (gifOpen) {
        if (!gif.playFrame(true, NULL)) {
            gif.reset();
        }
    } else {
        OswHal::getInstance()->gfx()->setTextCursor(40, 100);
        OswHal::getInstance()->gfx()->print("GIF error!");
    }
}

void OswAppGifPlayer::stop() {
    if (gifOpen) {
        gif.close();
    }
}

#else

void OswAppGifPlayer::setup() {

}

void OswAppGifPlayer::loop() {
    OswHal::getInstance()->gfx()->setTextCursor(DISP_W / 2, DISP_H / 2);
    OswHal::getInstance()->gfx()->setTextCenterAligned();
    OswHal::getInstance()->gfx()->print("GIF_BG is not supported\non the emulator!"); // You don't like that? Then go ahead and implement it! ;)
}

void OswAppGifPlayer::stop() {

}

#endif
