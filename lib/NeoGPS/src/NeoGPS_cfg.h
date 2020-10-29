#ifndef NEOGPS_CFG
#define NEOGPS_CFG

//  Copyright (C) 2014-2017, SlashDevin
//
//  This file is part of NeoGPS
//
//  NeoGPS is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  NeoGPS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.

//------------------------------------------------------------------------
// Enable/disable packed data structures.
//
// Enabling packed data structures will use two less-portable language
// features of GCC to reduce RAM requirements.  Although it was expected to slightly increase execution time and code size, the reverse is true on 8-bit AVRs: the code is smaller and faster with packing enabled.
//
// Disabling packed data structures will be very portable to other
// platforms.  NeoGPS configurations will use slightly more RAM, and on
// 8-bit AVRs, the speed is slightly slower, and the code is slightly
// larger.  There may be no choice but to disable packing on processors 
// that do not support packed structures.
//
// There may also be compiler-specific switches that affect packing and the
// code which accesses packed members.  YMMV.

#include <Arduino.h>

#ifdef __AVR__
  #define NEOGPS_PACKED_DATA
#endif

//------------------------------------------------------------------------
// Based on the above define, choose which set of packing macros should
// be used in the rest of the NeoGPS package.  Do not change these defines.

#ifdef NEOGPS_PACKED_DATA

  // This is for specifying the number of bits to be used for a 
  // member of a struct.  Booleans are typically one bit.
  #define NEOGPS_BF(b) :b

  // This is for requesting the compiler to pack the struct or class members
  // "as closely as possible".  This is a compiler-dependent interpretation.
  #define NEOGPS_PACKED __attribute__((packed))

#else

  // Let the compiler do whatever it wants.

  #define NEOGPS_PACKED
  #define NEOGPS_BF(b)

#endif

//------------------------------------------------------------------------
//  Accommodate C++ compiler and IDE changes.
//
//  Declaring constants as class data instead of instance data helps avoid
//  collisions with #define names, and allows the compiler to perform more
//  checks on their usage.
//
//  Until C++ 10 and IDE 1.6.8, initialized class data constants 
//  were declared like this:
//
//      static const <valued types> = <constant-value>;
//
//  Now, non-simple types (e.g., float) must be declared as
//
//      static constexpr <nonsimple-types> = <expression-treated-as-const>;
//
//  The good news is that this allows the compiler to optimize out an
//  expression that is "promised" to be "evaluatable" as a constant.
//  The bad news is that it introduces a new language keyword, and the old
//  code raises an error.
//
//  TODO: Evaluate the requirement for the "static" keyword.
//  TODO: Evaluate using a C++ version preprocessor symbol for the #if.
//          #if __cplusplus >= 201103L  (from XBee.h)
//
//  The CONST_CLASS_DATA define will expand to the appropriate keywords.
//


#if (                                              \
      (ARDUINO < 10606)                          | \
     ((10700  <= ARDUINO) & (ARDUINO <= 10799 )) | \
     ((107000 <= ARDUINO) & (ARDUINO <= 107999))   \
    )                                              \
        &                                          \
    !defined(ESP8266)  // PlatformIO Pull Request #82

  #define CONST_CLASS_DATA static const
  
#else

  #define CONST_CLASS_DATA static constexpr
  
#endif

//------------------------------------------------------------------------
// The PROGMEM definitions are not correct for Zero, MKR1000 and
//    earlier versions of Teensy boards

#if defined(ARDUINO_SAMD_MKRZERO) | \
    defined(ARDUINO_SAMD_ZERO)    | \
    defined(ARDUINO_SAM_DUE)      | \
    defined(ARDUINO_ARCH_ARC32)   | \
    defined(__TC27XX__)           | \
    (defined(TEENSYDUINO) && (TEENSYDUINO < 139))
  #undef pgm_read_ptr
  #define pgm_read_ptr(addr) (*(const void **)(addr))
#endif


#endif
