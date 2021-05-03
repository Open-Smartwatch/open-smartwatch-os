
#include "./apps/_experiments/menu.h"

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_hal.h>
#include <string.h>


void OswAppMenu::setup(OswHal* hal) {
  index = 3;
  selectionInProgress = false;
}
void OswAppMenu::loop(OswHal* hal) {
    static long loopCount = 0;

    char * entries[] = { "Orange", "Hello Paul", "Choice1", "Do me", "Do me not", "Click me"};
    uint8_t EntryCount = 5;

    if(hal->btn1Down() && hal->btn3Down()){
        selectionInProgress = true;
    }
    else
    {
        selectionInProgress = false;
        if (hal->btn1Down()) {
            index++;
            hal->clearBtn1();
        }

        if (hal->btn3Down()) {
            index--; 
            hal->clearBtn3();
        }
    }

    if(index >= EntryCount){
        index = EntryCount -1;
    }
    if(index < 0){
        index = 0;
    }

    
    
    hal->getCanvas()->fillRect(0, 0, 240, 240, rgb565(25, 25, 25));


    printEntry(hal, entries , index, EntryCount);
    
    hal->requestFlush();
    loopCount++;

}

void OswAppMenu::printEntry(OswHal* hal, char** entries, uint8_t index, int8_t EntryCount) {
    
    uint8_t sWidth = hal->getCanvas()->width();
    uint8_t sHeight = hal->getCanvas()->height();
    
    uint8_t cx = sWidth / 2;
    uint8_t cy = sHeight / 2;
    uint8_t  hy,ly;
    uint8_t  hx,lx;

    int16_t  x1, y1;
    uint16_t  w, h;

    uint8_t barPadding = 5;
    uint16_t backgroundColor = 0x5B0D;
    uint16_t textColor = 0xCE9B;
    uint16_t barColor = 0xFC68;
    uint16_t barSelectionColor = 0x7DEC;
    uint16_t barTextcolor = 0xFFBF;


    hal->getCanvas()->fillRect(0, 0, 240, 240, backgroundColor);
    hal->getCanvas()->setTextColor(rgb565(200, 255, 200));

    //Center
    hal->getCanvas()->setTextSize(3);
    hal->getCanvas()->getTextBounds(entries[index], 0, 0, &x1, &y1, &w, &h);  
    hal->getCanvas()->setCursor(cx - (w / 2) , cy - (h / 2));
    hal->getCanvas()->fillRect(0, sHeight / 2 - h / 2 - barPadding, sWidth , barPadding * 2 + h , selectionInProgress ? barSelectionColor : barColor);

    hal->getCanvas()->setTextColor(barTextcolor);
    hal->getCanvas()->print(entries[index]);

    hal->getCanvas()->setTextColor(textColor);
    
    //First Level
    hx = cx + sWidth / 8;
    hy = cy - h * 1.5f;
    lx = cx - sWidth / 8;
    ly = cy + h * 1.5f;

    if(index - 1 >= 0){
        hal->getCanvas()->setTextSize(2);
        hal->getCanvas()->getTextBounds(entries[index - 1], 0, 0, &x1, &y1, &w, &h);  

        hal->getCanvas()->setCursor(hx - (w / 2) , hy - (h / 2));
        hal->getCanvas()->print(entries[index - 1]);
    }

    if(index + 1 < EntryCount){
        hal->getCanvas()->setTextSize(2);
        hal->getCanvas()->getTextBounds(entries[index + 1], 0, 0, &x1, &y1, &w, &h);  

        hal->getCanvas()->setCursor(lx - (w / 2) , ly - (h / 2));
        hal->getCanvas()->print(entries[index + 1]);
    }

    //Second Level
    hx = hx + sWidth / 7;
    hy = hy - h * 1.5f;
    lx = lx - sWidth / 7;
    if(lx < 0 ){
        lx = 0;
    }
    ly = ly + h * 1.5f;

    if(index - 2 >= 0){
        hal->getCanvas()->setTextSize(1);
        hal->getCanvas()->getTextBounds(entries[index - 2], 0, 0, &x1, &y1, &w, &h);  
        hal->getCanvas()->setCursor(hx - (w / 2) , hy - (h / 2));
        hal->getCanvas()->print(entries[index - 2]);
    }
    if(index + 2 < EntryCount){
        hal->getCanvas()->setTextSize(1);
        hal->getCanvas()->getTextBounds(entries[index + 2], 0, 0, &x1, &y1, &w, &h);  
        hal->getCanvas()->setCursor(lx - (w / 2) , ly - (h / 2));
        hal->getCanvas()->print(entries[index + 2]);
    }

}

void OswAppMenu::stop(OswHal* hal) {

}