%module osw

%rename(drawBitmap_const) drawBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t,uint16_t);
%rename(drawBitmap_const) drawBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t,uint16_t,uint16_t);
%rename(drawGrayscaleBitmap_const) drawGrayscaleBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t);
%rename(drawGrayscaleBitmap_const) drawGrayscaleBitmap(int16_t,int16_t,uint8_t const [],uint8_t const [],int16_t,int16_t);
%rename(draw16bitRGBBitmap_const) draw16bitRGBBitmap(int16_t,int16_t,uint16_t const [],int16_t,int16_t);
%rename(draw16bitRGBBitmap_const) draw16bitRGBBitmap(int16_t,int16_t,uint16_t const [],uint8_t const [],int16_t,int16_t);
%rename(draw24bitRGBBitmap_const) draw24bitRGBBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t);
%rename(draw24bitRGBBitmap_const) draw24bitRGBBitmap(int16_t,int16_t,uint8_t const [],uint8_t const [],int16_t,int16_t);

%{
#include <osw_hal.h>
#include "Arduino_G.h"
#include "Arduino_GFX.h"
#include "ArduinoGraphics2DCanvas.h"

void halToLua(lua_State *L, OswHal *hal) {
    //Pass HAL to Lua
    SWIG_NewPointerObj(L, hal, SWIGTYPE_p_OswHal, 0);
    lua_setglobal(L, "hal");
}
%}

%include "osw_hal.h"
%include "Arduino_G.h"
%include "Arduino_GFX.h"
%include "ArduinoGraphics2DCanvas.h"