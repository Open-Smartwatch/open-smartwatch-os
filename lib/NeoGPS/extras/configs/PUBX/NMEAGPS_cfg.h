#ifndef NMEAGPS_CFG_H
#define NMEAGPS_CFG_H

//------------------------------------------------------
// Enable/disable the parsing of specific sentences.
//
// Configuring out a sentence prevents it from being recognized; it
// will be completely ignored.  (See also NMEAGPS_RECOGNIZE_ALL, below)
//
// FYI: Only RMC and ZDA contain date information.  Other
// sentences contain time information.  Both date and time are 
// required if you will be doing time_t-to-clock_t operations.

//#define NMEAGPS_PARSE_GGA
//#define NMEAGPS_PARSE_GLL
//#define NMEAGPS_PARSE_GSA
//#define NMEAGPS_PARSE_GSV
//#define NMEAGPS_PARSE_GST
//#define NMEAGPS_PARSE_RMC
//#define NMEAGPS_PARSE_VTG
//#define NMEAGPS_PARSE_ZDA

//------------------------------------------------------
// Select which sentence is sent *last* by your GPS device
// in each update interval.  This can be used by your sketch
// to determine when the GPS quiet time begins, and thus
// when you can perform "some" time-consuming operations.

#define LAST_SENTENCE_IN_INTERVAL (nmea_msg_t) NMEA_LAST_MSG+5 /* ubloxNMEA::PUBX_04 */

// If the NMEA_LAST_SENTENCE_IN_INTERVAL is not chosen 
// correctly, GPS data may be lost because the sketch
// takes too long elsewhere when this sentence is received.
// Also, fix members may contain information from different 
// time intervals (i.e., they are not coherent).
//
// If you don't know which sentence is the last one,
// use NMEAorder.ino to list them.  You do not have to select
// the last sentence the device sends if you have disabled
// it.  Just select the last sentence that you have *enabled*.

//------------------------------------------------------
// Enable/Disable coherency:
//
// If you need each fix to contain information that is only
// from the current update interval, you should uncomment
// this define.  At the beginning of the next interval,
// the accumulating fix will start out empty.  When
// the LAST_SENTENCE_IN_INTERVAL arrives, the valid
// fields will be coherent.

//#define NMEAGPS_COHERENT

// With IMPLICIT merging, fix() will be emptied when the
// next sentence begins.
//
// With EXPLICIT or NO merging, the fix() was already
// being initialized.
//
// If you use the fix-oriented methods available() and read(),
// they will empty the current fix for you automatically.
//
// If you use the character-oriented method decode(), you should
// empty the accumulating fix by testing and clearing the
// 'intervalComplete' flag in the same way that available() does.

//------------------------------------------------------
// Choose how multiple sentences are merged:
//   1) No merging
//        Each sentence fills out its own fix; there could be 
//        multiple sentences per interval.
//   2) EXPLICIT_MERGING
//        All sentences in an interval are *safely* merged into one fix.
//        NMEAGPS_FIX_MAX must be >= 1.
//        An interval is defined by NMEA_LAST_SENTENCE_IN_INTERVAL.
//   3) IMPLICIT_MERGING
//        All sentences in an interval are merged into one fix, with 
//        possible data loss.  If a received sentence is rejected for 
//        any reason (e.g., a checksum error), all the values are suspect.
//        The fix will be cleared; no members will be valid until new 
//        sentences are received and accepted.  This uses less RAM.
//        An interval is defined by NMEA_LAST_SENTENCE_IN_INTERVAL.
// Uncomment zero or one:

#define NMEAGPS_EXPLICIT_MERGING
//#define NMEAGPS_IMPLICIT_MERGING

#ifdef NMEAGPS_IMPLICIT_MERGING
  #define NMEAGPS_MERGING NMEAGPS::IMPLICIT_MERGING

  // When accumulating, nothing is done to the fix at the 
  // beginning of every sentence...
  #ifdef NMEAGPS_COHERENT
    // ...unless COHERENT is enabled and a new interval is starting
    #define NMEAGPS_INIT_FIX(m) \
      if (intervalComplete()) { intervalComplete( false ); m.valid.init(); }
  #else
    #define NMEAGPS_INIT_FIX(m)
  #endif

  // ...but we invalidate one part when it starts to get parsed.  It *may* get
  // validated when the parsing is finished.
  #define NMEAGPS_INVALIDATE(m) m_fix.valid.m = false

#else

  #ifdef NMEAGPS_EXPLICIT_MERGING
    #define NMEAGPS_MERGING NMEAGPS::EXPLICIT_MERGING
  #else
    #define NMEAGPS_MERGING NMEAGPS::NO_MERGING
    #define NMEAGPS_NO_MERGING
  #endif

  // When NOT accumulating, invalidate the entire fix at the 
  // beginning of every sentence
  #define NMEAGPS_INIT_FIX(m) m.valid.init()

  // ...so the individual parts do not need to be invalidated as they are parsed
  #define NMEAGPS_INVALIDATE(m)

#endif

#if ( defined(NMEAGPS_NO_MERGING) + \
    defined(NMEAGPS_IMPLICIT_MERGING) + \
    defined(NMEAGPS_EXPLICIT_MERGING) )  > 1
  #error Only one MERGING technique should be enabled in NMEAGPS_cfg.h!
#endif

//------------------------------------------------------
// Define the fix buffer size.  The NMEAGPS object will hold on to
// this many fixes before an overrun occurs.  This can be zero,
// but you have to be more careful about using gps.fix() structure,
// because it will be modified as characters are received.

#define NMEAGPS_FIX_MAX 1

#if defined(NMEAGPS_EXPLICIT_MERGING) && (NMEAGPS_FIX_MAX == 0)
  #error You must define FIX_MAX >= 1 to allow EXPLICIT merging in NMEAGPS_cfg.h
#endif

//------------------------------------------------------
// Enable/Disable interrupt-style processing of GPS characters
// If you are using one of the NeoXXSerial libraries,
//   to attachInterrupt, this must be defined.
// Otherwise, it must be commented out.

//#define NMEAGPS_INTERRUPT_PROCESSING

#ifdef  NMEAGPS_INTERRUPT_PROCESSING
  #define NMEAGPS_PROCESSING_STYLE NMEAGPS::PS_INTERRUPT
#else
  #define NMEAGPS_PROCESSING_STYLE NMEAGPS::PS_POLLING
#endif

//------------------------------------------------------
// Enable/disable the talker ID, manufacturer ID and proprietary message processing.
//
// First, some background information.  There are two kinds of NMEA sentences:
//
// 1. Standard NMEA sentences begin with "$ttccc", where
//      "tt" is the talker ID, and
//      "ccc" is the variable-length sentence type (i.e., command).
//
//    For example, "$GPGLL,..." is a GLL sentence (Geographic Lat/Long) 
//    transmitted by talker "GP".  This is the most common talker ID.  Some
//    devices may report "$GNGLL,..." when a mix of GPS and non-GPS
//    satellites have been used to determine the GLL data.
//
// 2. Proprietary NMEA sentences (i.e., those unique to a particular
//    manufacturer) begin with "$Pmmmccc", where
//      "P" is the NMEA-defined prefix indicator for proprietary messages,
//      "mmm" is the 3-character manufacturer ID, and
//      "ccc" is the variable-length sentence type (it can be empty).
//
// No validation of manufacturer ID and talker ID is performed in this
// base class.  For example, although "GP" is a common talker ID, it is not
// guaranteed to be transmitted by your particular device, and it IS NOT
// REQUIRED.  If you need validation of these IDs, or you need to use the
// extra information provided by some devices, you have two independent
// options:
//
// 1. Enable SAVING the ID: When /decode/ returns DECODE_COMPLETED, the
// /talker_id/ and/or /mfr_id/ members will contain ID bytes.  The entire
// sentence will be parsed, perhaps modifying members of /fix/.  You should
// enable one or both IDs if you want the information in all sentences *and*
// you also want to know the ID bytes.  This adds two bytes of RAM for the
// talker ID, and 3 bytes of RAM for the manufacturer ID.
//
// 2. Enable PARSING the ID:  The virtual /parse_talker_id/ and
// /parse_mfr_id/ will receive each ID character as it is parsed.  If it
// is not a valid ID, return /false/ to abort processing the rest of the
// sentence.  No CPU time will be wasted on the invalid sentence, and no
// /fix/ members will be modified.  You should enable this if you want to
// ignore some IDs.  You must override /parse_talker_id/ and/or
// /parse_mfr_id/ in a derived class.
//

//#define NMEAGPS_SAVE_TALKER_ID
//#define NMEAGPS_PARSE_TALKER_ID

#define NMEAGPS_PARSE_PROPRIETARY
#ifdef NMEAGPS_PARSE_PROPRIETARY
  //#define NMEAGPS_SAVE_MFR_ID
  #define NMEAGPS_PARSE_MFR_ID
#endif

//------------------------------------------------------
// Enable/disable tracking the current satellite array and,
// optionally, all the info for each satellite.
//

//#define NMEAGPS_PARSE_SATELLITES
//#define NMEAGPS_PARSE_SATELLITE_INFO

#ifdef NMEAGPS_PARSE_SATELLITES
  #define NMEAGPS_MAX_SATELLITES (20)

  #ifndef GPS_FIX_SATELLITES
    #error GPS_FIX_SATELLITES must be defined in GPSfix.h!
  #endif

#endif

#if defined(NMEAGPS_PARSE_SATELLITE_INFO) & \
    !defined(NMEAGPS_PARSE_SATELLITES)
  #error NMEAGPS_PARSE_SATELLITES must be defined!
#endif

//------------------------------------------------------
// Enable/disable gathering interface statistics:
// CRC errors and number of sentences received

//#define NMEAGPS_STATS

//------------------------------------------------------
// Configuration item for allowing derived types of NMEAGPS.
// If you derive classes from NMEAGPS, you *must* define NMEAGPS_DERIVED_TYPES.
// If not defined, virtuals are not used, with a slight size (2 bytes) and 
// execution time savings.

#define NMEAGPS_DERIVED_TYPES

#ifdef NMEAGPS_DERIVED_TYPES
  #define NMEAGPS_VIRTUAL virtual
#else
  #define NMEAGPS_VIRTUAL
#endif

//-----------------------------------
// See if DERIVED_TYPES is required
#if (defined(NMEAGPS_PARSE_TALKER_ID) | defined(NMEAGPS_PARSE_MFR_ID)) &  \
           !defined(NMEAGPS_DERIVED_TYPES)
  #error You must define NMEAGPS_DERIVED_TYPES in NMEAGPS.h in order to parse Talker and/or Mfr IDs!
#endif

//------------------------------------------------------
// Some devices may omit trailing commas at the end of some 
// sentences.  This may prevent the last field from being 
// parsed correctly, because the parser for some types keep 
// the value in an intermediate state until the complete 
// field is received (e.g., parseDDDMM, parseFloat and 
// parseZDA).
//
// Enabling this will inject a simulated comma when the end 
// of a sentence is received and the last field parser 
// indicated that it still needs one.

//#define NMEAGPS_COMMA_NEEDED

//------------------------------------------------------
//  Some applications may want to recognize a sentence type
//  without actually parsing any of the fields.  Uncommenting
//  this define will allow the nmeaMessage member to be set
//  when *any* standard message is seen, even though that 
//  message is not enabled by a NMEAGPS_PARSE_xxx define above.
//  No valid flags will be true for those sentences.

//#define NMEAGPS_RECOGNIZE_ALL

//------------------------------------------------------
// Sometimes, a little extra space is needed to parse an intermediate form.
// This config items enables extra space.

//#define NMEAGPS_PARSING_SCRATCHPAD

#endif