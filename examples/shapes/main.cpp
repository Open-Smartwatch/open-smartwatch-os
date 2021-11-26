#include <stdint.h>

#include <iostream>

#include "../../FakeArduino.h"
#include "../../FakeArduinoWindowSDL.h"
#include "../../gfx_2d.h"
#include "../../gfx_util.h"
#include "../../math_angles.h"
using namespace std;

#define BUF_W 240
#define BUF_H 240
uint16_t screenBuffer[BUF_W * BUF_H];

Graphics2D gfx2d(BUF_W, BUF_H, 16, false);

class WatchSimpleWindow : public SDLWindowRGB565 {
 public:
  WatchSimpleWindow(Graphics2D* gfx2d, int w, int h) : SDLWindowRGB565(gfx2d, w, h) {}
  void setup() {}

  void loop() {
    uint8_t cx = 119;
    uint8_t cy = 119;

    gfx2d.fill(rgb565(0, 0, 0));

    // gfx2d.drawLine(0, 0, x, x, rgb565(x, x, 255));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(x, x, 100));
    // gfx2d.drawCircle(120, 120, 32, rgb565(x, 255, 255));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(255, x, 100));
    // gfx2d.drawEllipse(20, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillEllipse(120, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillFrame(10, 10, 10, 10, rgb565(255, 255, 100));
    // gfx2d.drawEllipse(20, 20, 5, 20, rgb565(255, x, 0));
    // gfx2d.fillEllipse(120, 20, 5, 20, rgb565(255, 0, x));
    // gfx2d.fillRFrame(40, 40, 20, 30, 4, rgb565(255, x, 100));
    // gfx2d.drawRFrame(60, 60, 30, 20, 4, rgb565(255, x, x));

    // gfx2d.drawCircle(119, 119, 119, rgb565(255, 255, 255));
    // gfx2d.drawCircle(119, 120, 119, rgb565(255, 255, 255));
    // gfx2d.drawCircle(120, 119, 119, rgb565(255, 255, 255));
    // gfx2d.drawCircle(120, 120, 119, rgb565(255, 255, 255));

    // gfx2d.drawThickLine(0, 0, 120, 120, 4, rgb565(255, 0, 0));
    // gfx2d.drawThickLine(240, 0, 120, 120, 5, rgb565(0, 255, 0));
    // gfx2d.drawThickLine(0, 240, 120, 120, 6, rgb565(0, 0, 255));
    // gfx2d.drawThickLine(240, 240, 120, 120, 7, rgb565(255, 255, 0));

    // gfx2d.fill(rgb565(0, 255, 100));
    // gfx2d.fillCircle(120, 120, 20, rgb565(255, 255, 255));

    // drawArc(120, 120, -90, 90, 90, 110, 6, rgb565(128, 128, 128));
    // drawArc(120, 120, -90, 90, 90, 110, 5, rgb565(255, 255, 255));
    // drawArc(120, 120, -90, 90, 90, 110, 4, rgb565(0, 32, 0));
    // drawArc(120, 120, -90, 44, 90, 110, 4, rgb565(0, 255, 0));

    gfx2d.drawArc(120, 120, 0, 360, 90, 113, 5, rgb565(32, 32, 32));
    // gfx2d.drawMinuteTicks(120, 120, 116, 50, rgb565(255, 0, 0));
    gfx2d.drawHourTicks(120, 120, 117, 107, rgb565(128, 128, 128));

    gfx2d.drawArc(120, 120, 0, 360, 90, 93, 7, changeColor(rgb565(210, 50, 66), 0.25));
    gfx2d.drawArc(120, 120, 0, 280, 90, 93, 7, dimColor(rgb565(210, 50, 66), 25));
    gfx2d.drawArc(120, 120, 0, 280, 90, 93, 6, rgb565(210, 50, 66));

    gfx2d.drawArc(120, 120, 0, 360, 90, 75, 7, changeColor(rgb565(117, 235, 10), 0.25));
    gfx2d.drawArc(120, 120, 0, 128, 90, 75, 7, dimColor(rgb565(117, 235, 10), 25));
    gfx2d.drawArc(120, 120, 0, 128, 90, 75, 6, rgb565(117, 235, 10));

    gfx2d.drawArc(120, 120, 0, 360, 90, 57, 7, changeColor(rgb565(25, 193, 202), 0.25));
    gfx2d.drawArc(120, 120, 0, 32, 90, 57, 7, dimColor(rgb565(25, 193, 202), 25));
    gfx2d.drawArc(120, 120, 0, 32, 90, 57, 6, rgb565(25, 193, 202));

    static uint32_t ticks = 0;
    ticks++;
    float deltaAngle = ticks;
    // hours
    gfx2d.drawThickTick(120, 120, 0, 16 + deltaAngle / (3600), -66, 1, rgb565(255, 255, 255));
    gfx2d.drawThickTick(120, 120, 16, 60, -66 + deltaAngle / (3600), 4, rgb565(255, 255, 255));

    // minutes
    gfx2d.drawThickTick(120, 120, 0, 16, 45 + deltaAngle / (60), 1, rgb565(255, 255, 255));
    gfx2d.drawThickTick(120, 120, 16, 105, 45 + deltaAngle / (60), 4, rgb565(255, 255, 255));

    // seconds
    gfx2d.fillCircle(120, 120, 3, rgb565(255, 0, 0));
    gfx2d.drawThickTick(120, 120, 0, 16, 0 + deltaAngle, 1, rgb565(255, 0, 0));
    gfx2d.drawThickTick(120, 120, 0, 110, 180 + deltaAngle, 1, rgb565(255, 0, 0));

    // moon
    gfx2d.fillCircle(120, 230, 9, rgb565(128, 128, 128));
    gfx2d.fillCircle(120, 230, 8, rgb565(255, 255, 255));
    gfx2d.fillCircle(123, 230, 6, rgb565(0, 0, 0));

    // usb connector
    gfx2d.fillFrame(104, 10, 13, 2, rgb565(128, 128, 128));  // cable dot
    gfx2d.fillFrame(117, 8, 3, 6, rgb565(200, 200, 200));    // cable to casing
    gfx2d.fillFrame(124, 8, 11, 6, rgb565(128, 128, 128));   // connector
    gfx2d.fillFrame(120, 6, 8, 10, rgb565(200, 200, 200));   // casing
    delay(1000 / 30);
  }
};

int main(int argc, char* argv[]) {
  WatchSimpleWindow wsw(&gfx2d, BUF_W, BUF_H);
  wsw.run();
  return 0;
}
