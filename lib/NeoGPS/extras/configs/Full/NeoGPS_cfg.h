#ifndef NEOGPS_CFG
#define NEOGPS_CFG

/**
 * Enable/disable packed data structures.
 *
 * Enabling packed data structures will use two less-portable language
 * features of GCC to reduce RAM requirements.  Although it was expected to slightly increase execution time and code size, the reverse is true on 8-bit AVRs: the code is smaller and faster with packing enabled.
 *
 * Disabling packed data structures will be very portable to other
 * platforms.  NeoGPS configurations will use slightly more RAM, and on
 * 8-bit AVRs, the speed is slightly slower, and the code is slightly
 * larger.  There may be no choice but to disable packing on processors 
 * that do not support packed structures.
 *
 * There may also be compiler-specific switches that affect packing and the
 * code which accesses packed members.  YMMV.
 **/

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

/*
 *  Accommodate C++ compiler and IDE changes.
 *
 *  Declaring constants as class data instead of instance data helps avoid
 *  collisions with #define names, and allows the compiler to perform more
 *  checks on their usage.
 *
 *  Until C++ 10 and IDE 1.6.8, initialized class data constants 
 *  were declared like this:
 *
 *      static const <valued types> = <constant-value>;
 *
 *  Now, non-simple types (e.g., float) must be declared as
 *
 *      static constexpr <nonsimple-types> = <expression-treated-as-const>;
 *
 *  The good news is that this allows the compiler to optimize out an
 *  expression that is "promised" to be "evaluatable" as a constant.
 *  The bad news is that it introduces a new language keyword, and the old
 *  code raises an error.
 *
 *  TODO: Evaluate the requirement for the "static" keyword.
 *  TODO: Evaluate using a C++ version preprocessor symbol for the #if.
 *
 *  The CONST_CLASS_DATA define will expand to the appropriate keywords.
 *
 */

#if ARDUINO < 10606

  #define CONST_CLASS_DATA static const
  
#else

  #define CONST_CLASS_DATA static constexpr
  
#endif

#endif
