#include "./apps/examples/fonts/fonts_example.h"

// main documentation:
// https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts

// option #1: "ready" fonts":
// "ready" fonts in GFXfont format, downloaded from here:
// https://github.com/adafruit/Adafruit-GFX-Library/tree/master/Fonts
#include "./fonts/FreeMonoBold24pt7b.h"
#include "./fonts/FreeSerifItalic18pt7b.h"
#include "./fonts/KRfont.h"

// option #2:  custom font:
// download as TTF and convert to GFXfont using one of many command line tools or online tools.
// eg, font url: (Free for personal use) https://www.dafont.com/ds-digital.font
// eg, online tool: https://rop.nl/truetype2gfx/
#include "./fonts/DS_DIGI30pt7b.h"

static void drawFontsExampleScreen(OswHal* hal) {
  hal->gfx()->fill(0);
  hal->gfx()->setTextColor(rgb565(255, 255, 255), rgb565(0, 0, 0));

  // default font
  hal->gfx()->setFont(nullptr);
  hal->gfx()->setTextSize(3);
  hal->gfx()->setTextCursor(40, 60);
  hal->gfx()->print("Fonts");
  hal->gfx()->setTextCursor(65, 80);
  hal->gfx()->print("example");

  // font #1 example
  hal->gfx()->setFont(&FreeMonoBold24pt7b);
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCursor(30, 120);
  hal->gfx()->print("Hello");

  // font #2 example
  hal->gfx()->setFont(&FreeSerifItalic18pt7b);
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCursor(110, 150);
  hal->gfx()->print("world");

  // font #3 example
  hal->gfx()->setFont(&DS_DIGI30pt7b);
  hal->gfx()->setTextSize(1);
  hal->gfx()->setTextCursor(50, 210);
  hal->gfx()->print("23:59");

  hal->requestFlush();
}

void OswAppFontsExample::setup(OswHal* hal) {}

void OswAppFontsExample::loop(OswHal* hal) { drawFontsExampleScreen(hal); }

void OswAppFontsExample::stop(OswHal* hal) {}
