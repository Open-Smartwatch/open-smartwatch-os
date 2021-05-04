#include "./faces/osw_analogface.h"

#define COLOR_RED rgb565(210, 50, 66)
#define COLOR_GREEN rgb565(117, 235, 10)
#define COLOR_BLUE rgb565(25, 193, 202)
#define COLOR_WOOD rgb565(179, 107, 0)


#ifdef GIF_BG
#include "./apps/_experiments/gif_player.h"
#endif


#ifdef GIF_BG
OswAppGifPlayer* bgGif = new OswAppGifPlayer();
#endif

void OswAnalogFace::setup(OswLauncher* launcher) {
    #ifdef GIF_BG
        bgGif->setup(_launcher->getHal());
    #endif
    OswBasicFace::setup(launcher);
}


void OswAnalogFace::action(){
    _launcher->getHal()->increaseBrightness(25);
}
void OswAnalogFace::action2(){
    _launcher->getHal()->decreaseBrightness(25);
}


void OswAnalogFace::stop() {
#ifdef GIF_BG
        bgGif->stop(hal);
#endif
    OswBasicFace::stop();
}
void OswAnalogFace::loop() {
#ifdef GIF_BG
    bgGif->loop(_launcher->getHal());
#endif
    OswBasicFace::loop();
}


void OswAnalogFace::draw(Graphics2D* gfx2d) {


#ifndef GIF_BG
  gfx2d->drawArc(120, 120, 0, 360, 90, 113, 5, rgb565(32, 32, 32));
#endif
    gfx2d->fill(rgb565(0, 0, 0));

  // gfx2d.drawMinuteTicks(120, 120, 116, 50, rgb565(255, 0, 0));
  gfx2d->drawHourTicks(120, 120, 117, 107, rgb565(128, 128, 128));

  uint32_t steps = _launcher->getHal()->getStepCount();

#ifndef GIF_BG
  gfx2d->drawArc(120, 120, 0, 360, 180, 93, 7, changeColor(COLOR_WOOD, 0.25));
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 7, dimColor(COLOR_WOOD, 25));
#endif
  gfx2d->drawArc(120, 120, 0, steps % 360, 180, 93, 6, COLOR_WOOD);

  // hours
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 4, rgb565(255, 255, 255));

  // minutes
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, rgb565(255, 255, 255));
  gfx2d->drawThickTick(120, 120, 16, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 4, rgb565(255, 255, 255));

#ifndef GIF_BG
  // seconds
  gfx2d->fillCircle(120, 120, 3, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 16, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
  gfx2d->drawThickTick(120, 120, 0, 110, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
#endif
  
}

