#include "./faces/osw_basicface.h"

#define COLOR_RED rgb565(210, 50, 66)
#define COLOR_GREEN rgb565(117, 235, 10)
#define COLOR_BLUE rgb565(25, 193, 202)
#define COLOR_WOOD rgb565(179, 107, 0)


void OswBasicFace::setup(OswLauncher* launcher) {
    _launcher = launcher;
}

void OswBasicFace::stop() {
    
}

void OswBasicFace::loop() {
    getTime();
    Graphics2D* gfx2d = _launcher->getGfx2g();
    draw(gfx2d);
    _launcher->requestFlush();
}

void OswBasicFace::getTime() {
    _launcher->getHal()->getLocalTime(&hour, &minute, &second);
}

void OswBasicFace::draw(Graphics2D* gfx2d) {
    // fill background
    gfx2d->fill(rgb565(0, 0, 0));

    // hours
    gfx2d->drawThickTick(120, 120, 0, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, rgb565(255, 255, 255));
    
    // minutes
    gfx2d->drawThickTick(120, 120, 0, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, rgb565(255, 255, 255));
    
    // seconds
    gfx2d->drawThickTick(120, 120, 110, 16, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
}



void OswBasicFace::action(){
    
} 
 
void OswBasicFace::action2(){

} 
