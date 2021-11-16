%module osw

%rename(drawBitmap_const) drawBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t,uint16_t);
%rename(drawBitmap_const) drawBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t,uint16_t,uint16_t);
%rename(drawGrayscaleBitmap_const) drawGrayscaleBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t);
%rename(drawGrayscaleBitmap_const) drawGrayscaleBitmap(int16_t,int16_t,uint8_t const [],uint8_t const [],int16_t,int16_t);
%rename(draw16bitRGBBitmap_const) draw16bitRGBBitmap(int16_t,int16_t,uint16_t const [],int16_t,int16_t);
%rename(draw16bitRGBBitmap_const) draw16bitRGBBitmap(int16_t,int16_t,uint16_t const [],uint8_t const [],int16_t,int16_t);
%rename(draw24bitRGBBitmap_const) draw24bitRGBBitmap(int16_t,int16_t,uint8_t const [],int16_t,int16_t);
%rename(draw24bitRGBBitmap_const) draw24bitRGBBitmap(int16_t,int16_t,uint8_t const [],uint8_t const [],int16_t,int16_t);

//These are all shadowed by similar types
//Ignore for now, have to see if ignoring is good enough
%ignore print(int);
%ignore print(unsigned int);
%ignore print(long);
%ignore print(unsigned long);
%ignore print(long long);
%ignore print(unsigned long long);
%ignore print(double);
%ignore print(unsigned char);
%ignore print(int,int);
%ignore print(unsigned long long,int);
%ignore print(long,int);
%ignore print(double,int);
%ignore print(unsigned char,int);
%ignore print(unsigned long,int);
%ignore print(long long,int);
%ignore print(unsigned int,int);
%ignore println(int);
%ignore println(unsigned int);
%ignore println(long);
%ignore println(unsigned long);
%ignore println(long long);
%ignore println(unsigned long long);
%ignore println(double);
%ignore println(unsigned char);
%ignore println(unsigned long long,int);
%ignore println(long,int);
%ignore println(double,int);
%ignore println(unsigned char,int);
%ignore println(unsigned long,int);
%ignore println(int,int);
%ignore println(unsigned int,int);
%ignore println(long long,int);

%{
#include <osw_hal.h>

void halToLua(lua_State *L) {
    //Pass HAL to Lua
    SWIG_NewPointerObj(L, OswHal::getInstance(), SWIGTYPE_p_OswHal, 0);
    lua_setglobal(L, "hal");
}
%}

#define __attribute__(x)

%include <std_string.i>
%include "stdint.i"
%include "Print.h"
%include "osw_hal.h"
