#ifndef NMEAGPS_CFG_H
#define NMEAGPS_CFG_H

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

#include "GPSfix_cfg.h"

//------------------------------------------------------
// Enable/disable the parsing of specific sentences.
//
// Configuring out a sentence prevents it from being recognized; it
// will be completely ignored.  (See also NMEAGPS_RECOGNIZE_ALL, below)
//
// FYI: Only RMC and ZDA contain date information.  Other
// sentences contain time information.  Both date and time are 
// required if you will be doing time_t-to-clock_t operations.

#define NMEAGPS_PARSE_GGA
#define NMEAGPS_PARSE_GLL
#define NMEAGPS_PARSE_GSA
#define NMEAGPS_PARSE_GSV
//#define NMEAGPS_PARSE_GST
#define NMEAGPS_PARSE_RMC
#define NMEAGPS_PARSE_VTG
//#define NMEAGPS_PARSE_ZDA

//------------------------------------------------------
// Select which sentence is sent *last* by your GPS device
// in each update interval.  This can be used by your sketch
// to determine when the GPS quiet time begins, and thus
// when you can perform "some" time-consuming operations.

#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL

// NOTE: For PUBX-only, PGRM and UBX configs, use
//          (NMEAGPS::nmea_msg_t)(NMEAGPS::NMEA_LAST_MSG+1)
//       Otherwise, use one of the standard NMEA messages:
//          NMEAGPS::NMEA_RMC
//
//    ==>  CONFIRM THIS WITH NMEAorder.INO  <==
//
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
// Choose how multiple sentences are merged into a fix:
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

  // Nothing is done to the fix at the beginning of every sentence...
  #define NMEAGPS_INIT_FIX(m)

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

  // When NOT accumulating (not IMPLICIT), invalidate the entire fix 
  // at the beginning of every sentence...
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
// Define how fixes are dropped when the FIFO is full.
//   true  = the oldest fix will be dropped, and the new fix will be saved.
//   false = the new fix will be dropped, and all old fixes will be saved.

#define NMEAGPS_KEEP_NEWEST_FIXES true

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

//#define NMEAGPS_PARSE_PROPRIETARY
#ifdef NMEAGPS_PARSE_PROPRIETARY
  //#define NMEAGPS_SAVE_MFR_ID
  #define NMEAGPS_PARSE_MFR_ID
#endif

//------------------------------------------------------
// Enable/disable tracking the current satellite array and,
// optionally, all the info for each satellite.
//

#define NMEAGPS_PARSE_SATELLITES
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

#define NMEAGPS_STATS

//------------------------------------------------------
// Configuration item for allowing derived types of NMEAGPS.
// If you derive classes from NMEAGPS, you *must* define NMEAGPS_DERIVED_TYPES.
// If not defined, virtuals are not used, with a slight size (2 bytes) and 
// execution time savings.

//#define NMEAGPS_DERIVED_TYPES

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
//  Becase the NMEA checksum is not very good at error detection, you can 
//    choose to enable additional validity checks.  This trades a little more 
//    code and execution time for more reliability.
//
//  Validation at the character level is a syntactic check only.  For 
//    example, integer fields must contain characters in the range 0..9, 
//    latitude hemisphere letters can be 'N' or 'S'.  Characters that are not 
//    valid for a particular field will cause the entire sentence to be 
//    rejected as an error, *regardless* of whether the checksum would pass.
#define NMEAGPS_VALIDATE_CHARS false

//  Validation at the field level is a semantic check.  For 
//    example, latitude degrees must be in the range -90..+90.
//    Values that are not valid for a particular field will cause the 
//    entire sentence to be rejected as an error, *regardless* of whether the 
//    checksum would pass.
#define NMEAGPS_VALIDATE_FIELDS false

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

#define NMEAGPS_COMMA_NEEDED

//------------------------------------------------------
//  Some applications may want to recognize a sentence type
//  without actually parsing any of the fields.  Uncommenting
//  this define will allow the nmeaMessage member to be set
//  when *any* standard message is seen, even though that 
//  message is not enabled by a NMEAGPS_PARSE_xxx define above.
//  No valid flags will be true for those sentences.

#define NMEAGPS_RECOGNIZE_ALL

//------------------------------------------------------
// Sometimes, a little extra space is needed to parse an intermediate form.
// This config items enables extra space.

//#define NMEAGPS_PARSING_SCRATCHPAD

//------------------------------------------------------
// If you need to know the exact UTC time at *any* time,
//   not just after a fix arrives, you must calculate the
//   offset between the Arduino micros() clock and the UTC 
//   time in a received fix.  There are two ways to do this:
//
// 1) When the GPS quiet time ends and the new update interval begins.  
//    The timestamp will be set when the first character (the '$') of 
//    the new batch of sentences arrives from the GPS device.  This is fairly
//    accurate, but it will be delayed from the PPS edge by the GPS device's
//    fix calculation time (usually ~100us).  There is very little variance
//    in this calculation time (usually < 30us), so all timestamps are 
//    delayed by a nearly-constant amount.
//
//    NOTE:  At update rates higher than 1Hz, the updates may arrive with 
//    some increasing variance.

//#define NMEAGPS_TIMESTAMP_FROM_INTERVAL

// 2) From the PPS pin of the GPS module.  It is up to the application 
//    developer to decide how to capture that event.  For example, you could:
//
//    a) simply poll for it in loop and call UTCsecondStart(micros());
//    b) use attachInterrupt to call a Pin Change Interrupt ISR to save 
//       the micros() at the time of the interrupt (see NMEAGPS.h), or
//    c) connect the PPS to an Input Capture pin.  Set the 
//       associated TIMER frequency, calculate the elapsed time
//       since the PPS edge, and add that to the current micros().

//#define NMEAGPS_TIMESTAMP_FROM_PPS

#if defined( NMEAGPS_TIMESTAMP_FROM_INTERVAL ) &   \
    defined( NMEAGPS_TIMESTAMP_FROM_PPS )
  #error You cannot enable both TIMESTAMP_FROM_INTERVAL and PPS in NMEAGPS_cfg.h!
#endif

#endif