
#include "./apps/_experiments/autumn.h"

#include <animations/anim_water_ripple.h>
#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_hal.h>

#if defined(GPS_EDITION) || defined(GPS_EDITION_ROTATED)

#define BUF_W 240
#define BUF_H 240

#define WATER_W 120
#define WATER_H 120

int8_t wbuf1[WATER_W * WATER_H];
int8_t wbuf2[WATER_W * WATER_H];

uint16_t maskColor = rgb565(255, 255, 255);
int16_t lx1 = 30 + random(20), ly1 = 30 + (-30 + random(60));
int16_t lx2 = 80 + random(20), ly2 = 30 + (-30 + random(60));
int16_t lx3 = 30 + random(20), ly3 = 80 + (-30 + random(60));
int16_t lx4 = 80 + random(10), ly4 = 80 + (-30 + random(60));

int8_t mx1 = -3 + random(5), my1 = -3 + random(5);
int8_t mx2 = -3 + random(5), my2 = -3 + random(5);
int8_t mx3 = -3 + random(5), my3 = -3 + random(5);
int8_t mx4 = -3 + random(5), my4 = -3 + random(5);

// this is dirty:

Graphics2D* gfx2d;
Graphics2D* waterBackground;
Graphics2D* waterScreenBuffer;
Graphics2D* leaf0;
Graphics2D* leaf1;
Graphics2D* leaf2;
Graphics2D* leaf3;

void OswAppAutumn::setup() {
    gfx2d = OswHal::getInstance()->gfx();

    waterBackground = new Graphics2D(WATER_W, WATER_H, 8);
    waterScreenBuffer = new Graphics2D(WATER_W, WATER_H, 8);
    leaf0 = new Graphics2D(32, 32, 16);
    leaf1 = new Graphics2D(32, 32, 16);
    leaf2 = new Graphics2D(32, 32, 16);
    leaf3 = new Graphics2D(32, 32, 16);

    // leaf0->enableMask(maskColor);
    // leaf1->enableMask(maskColor);
    // leaf2->enableMask(maskColor);
    // leaf3->enableMask(maskColor);

    OswHal* hal = OswHal::getInstance();
    hal->setPNGAlphaPlaceHolder(maskColor);  // cheat because we have no transparency
    hal->loadPNGfromSD(leaf0, "/leaf1.png");
    hal->loadPNGfromSD(leaf1, "/leaf2.png");
    hal->loadPNGfromSD(leaf2, "/leaf3.png");
    hal->loadPNGfromSD(leaf3, "/leaf4.png");
    waterBackground->enableMask(maskColor);
}

void OswAppAutumn::loop() {
    static uint16_t counter = 0;
    counter++;

    if (counter % 10 == 0) {
        // this is ugly and not object oriented...
        lx1 += mx1;
        ly1 += my1;
        lx2 += mx2;
        ly2 += my2;
        lx3 += mx3;
        ly3 += my3;
        lx4 += mx4;
        ly4 += my4;

        if (lx1 < -16) {
            lx1 = 120 + 16;
        }
        if (ly1 < -16) {
            ly1 = 120 + 16;
        }
        if (lx2 < -16) {
            lx2 = 120 + 16;
        }
        if (ly2 < -16) {
            ly2 = 120 + 16;
        }
        if (lx3 < -16) {
            lx3 = 120 + 16;
        }
        if (ly3 < -16) {
            ly3 = 120 + 16;
        }
        if (lx4 < -16) {
            lx4 = 120 + 16;
        }
        if (ly4 < -16) {
            ly4 = 120 + 16;
        }
    }

    if (counter % 5 == 0) {
        uint16_t r1 = random(WATER_W - 4);
        uint16_t r2 = random(WATER_H - 4);

        // randomize water
        for (uint16_t x = r1; x < r1 + 3; x++) {
            for (uint16_t y = r2; y < r2 + 3; y++) {
                wbuf1[x + WATER_W * y] = 127;
            }
        }
    }

    waterBackground->fillFrame(0, 0, 240, 240, rgb565(1 << 4, 1 << 4, 1 << 4));

    // gfx2d.enableAlpha(.5);
    waterBackground->drawGraphics2D_rotated(lx1, ly1, leaf0, 16, 16, counter / 50.0);
    waterBackground->drawGraphics2D_rotated(lx2, ly2, leaf1, 16, 16, -counter / 50.0);
    waterBackground->drawGraphics2D_rotated(lx3, ly3, leaf2, 16, 16, counter / 50.0);
    waterBackground->drawGraphics2D_rotated(lx4, ly4, leaf3, 16, 16, -counter / 50.0);
    // gfx2d.disableAlpha();

    calcWater(wbuf1, wbuf2, WATER_W, WATER_H, .9);
    mapWater(wbuf1, WATER_W, WATER_H, waterBackground, waterScreenBuffer, 0, 0);
    std::swap(wbuf1, wbuf2);

    // gfx2d->fillFrame(0, 0, 240, 240, rgb565(0, 0, 0));
    gfx2d->drawGraphics2D_2x(0, 0, waterScreenBuffer);

    // fix glitches:
    gfx2d->fillFrame(0, 0, 240, 4, rgb565(0, 0, 0));
    gfx2d->fillFrame(0, 236, 240, 4, rgb565(0, 0, 0));
    gfx2d->fillFrame(0, 0, 4, 240, rgb565(0, 0, 0));
    gfx2d->fillFrame(236, 0, 4, 240, rgb565(0, 0, 0));
}
#endif
