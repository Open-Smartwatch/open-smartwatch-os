#include <NeoGPS_cfg.h>
#include <ublox/ubxGPS.h>
#include <NeoTeeStream.h>

//======================================================================
//  Program: ubloxRate.ino
//
//  Description:  This program sends ublox commands to enable and disable
//    NMEA sentences, set the update rate, and set the 
//    baud rate to 9600 or 115200.
//
//    Enter the following commands through the Serial Monitor window:
//
const char help[] PROGMEM = R"(
  '?'  - dislay this help message
  '1'  - send NMEA PUBX text command to enable all sentences
  '0'  - send NMEA PUBX text command to disable all sentences except GLL
  'd'  - send UBX binary command to disable all sentences except GLL
  'r1' - send UBX binary command to set update rate to  1Hz
  'r5' - send UBX binary command to set update rate to  5Hz
  'r0' - send UBX binary command to set update rate to 10Hz
  'r6' - send UBX binary command to set update rate to 16Hz
  're' - send UBX binary command to reset the GPS device (cold start)
  '5'  - send NMEA PUBX text command to set baud rate to 115200
  '7'  - send NMEA PUBX text command to set baud rate to 57600
  '3'  - send NMEA PUBX text command to set baud rate to 38400
  '9'  - send NMEA PUBX text command to set baud rate to 9600
  'e'  - toggle echo of all characters received from GPS device.
  't'  - toggle tracing of parsed GPS fields.
)";
//    CAUTION:   If your Serial Monitor window baud rate is less 
//       than the GPS baud rate, turning echo ON will cause the
//        sketch to lose some or all GPS data and/or fixes.
//
//    NOTE:  All NMEA PUBX text commands are also echoed to the debug port.
//
//  Prerequisites:
//     1) Your GPS device has been correctly powered.
//          Be careful when connecting 3.3V devices.
//     2) Your GPS device is correctly connected to an Arduino serial port.
//          See GPSport.h for the default connections.
//     3) You know the default baud rate of your GPS device.
//          If 9600 does not work, use NMEAdiagnostic.ino to
//          scan for the correct baud rate.
//     4) LAST_SENTENCE_IN_INTERVAL is defined to be
//          the following in NMEAGPS_cfg.h:
//
//          #include <stdint.h>
//          extern uint8_t LastSentenceInInterval; // a variable!
//          #define LAST_SENTENCE_IN_INTERVAL \
//                      ((NMEAGPS::nmea_msg_t) LastSentenceInInterval)
//
//        This is a replacement for the typical
//
//          #define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL
//
//        This allows the sketch to choose the last sentence *at run time*, not
//        compile time.  This is necessary because this sketch can send 
//        configuration commands that change which sentences are enabled.
//        The storage for the "externed" variable is below.
//     5) ublox.ino builds correctly (see its prequisites).
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2014-2018, SlashDevin
//
//    This file is part of NeoGPS
//
//    NeoGPS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NeoGPS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.
//
//======================================================================

#include <GPSport.h>
#include <Streamers.h>

//------------------------------------------------------------
// Check that the config files are set up properly

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *not* enable NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

#ifndef NMEAGPS_DERIVED_TYPES
  #error You must enable NMEAGPS_DERIVED_TYPES in NMEAGPS_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_GLL)
  #error You must enable NMEAGPS_PARSE_GLL in NMEAGPS_cfg.h!
#endif

#if !defined(UBLOX_PARSE_STATUS)  & !defined(UBLOX_PARSE_TIMEGPS) & \
    !defined(UBLOX_PARSE_TIMEUTC) & !defined(UBLOX_PARSE_POSLLH)  & \
    !defined(UBLOX_PARSE_DOP)     & !defined(UBLOX_PARSE_PVT)     & \
    !defined(UBLOX_PARSE_VELNED)  & !defined(UBLOX_PARSE_SVINFO)  & \
    !defined(UBLOX_PARSE_HNR_PVT)

  #error No UBX binary messages enabled: no fix data available.

#endif

//-----------------------------------------------------------------

static ubloxGPS  gps( &gpsPort );
static gps_fix  fix_data;
uint8_t LastSentenceInInterval = 0xFF; // storage for the run-time selection

static char lastChar; // last command char
static bool echoing = false;
static bool tracing = true;

//  Use NeoTee to echo the NMEA text commands to the Serial Monitor window
Stream *both[2] = { &DEBUG_PORT, &gpsPort };
NeoTeeStream tee( both, sizeof(both)/sizeof(both[0]) );

//-------------------------------------------
// U-blox UBX binary commands

const unsigned char ubxRate1Hz[] PROGMEM = 
  { 0x06,0x08,0x06,0x00,0xE8,0x03,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate5Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,200,0x00,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate10Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,100,0x00,0x01,0x00,0x01,0x00 };
const unsigned char ubxRate16Hz[] PROGMEM =
  { 0x06,0x08,0x06,0x00,50,0x00,0x01,0x00,0x01,0x00 };

// Disable specific NMEA sentences
const unsigned char ubxDisableGGA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableGLL[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableGSA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x02,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableGSV[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x03,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableRMC[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x04,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableVTG[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x05,0x00,0x00,0x00,0x00,0x00,0x01 };
const unsigned char ubxDisableZDA[] PROGMEM =
  { 0x06,0x01,0x08,0x00,0xF0,0x08,0x00,0x00,0x00,0x00,0x00,0x01 };

static const uint8_t ubxReset[] __PROGMEM =
  { ublox::UBX_CFG, ublox::UBX_CFG_RST,
    UBX_MSG_LEN(ublox::cfg_reset_t), 0,               // word length MSB is 0
    0,0,                                              // clear bbr section
    ublox::cfg_reset_t::CONTROLLED_SW_RESET_GPS_ONLY, // reset mode
    0x00                                              // reserved
  };

void resetGPS()
{
  enum sendUBXmethod_t
    {
      FROM_RAM_STRUCT, 
      FROM_PROGMEM_BYTES, 
      FROM_PROGMEM_STRUCT
    };

  const sendUBXmethod_t WAY = FROM_PROGMEM_STRUCT; // pick one

  switch (WAY) {
    case FROM_RAM_STRUCT:
      {
        ublox::cfg_reset_t cfg_cold; // RAM
        cfg_cold.clear_bbr_section =
          // Hotstart
          { false };

          // Warmstart
          // { true, false };

          // Coldstart
          // { true, true, true, true, true, true, true, true, true,
          //   true, // reserved1 is 2 bits
          //   true, true, true, 
          //   true, // reserved2 is 1 bit
          //   true };

        cfg_cold.reset_mode = ublox::cfg_reset_t::CONTROLLED_SW_RESET_GPS_ONLY;
        cfg_cold.reserved   = 0;
        gps.send_request( cfg_cold );
      }
      break;

    case FROM_PROGMEM_BYTES:
      sendUBX( ubxReset, sizeof(ubxReset) ); // just send the PROGMEM bytes
      break;

    case FROM_PROGMEM_STRUCT:
      {
        const ublox::cfg_reset_t *cfg_cold_ptr = (const ublox::cfg_reset_t *) ubxReset;
        gps.send_request_P( *cfg_cold_ptr );
      }
      break;
  }

} // resetGPS
  
//--------------------------

void sendUBX( const unsigned char *progmemBytes, size_t len )
{
  gpsPort.write( 0xB5 ); // SYNC1
  gpsPort.write( 0x62 ); // SYNC2

  uint8_t a = 0, b = 0;
  while (len-- > 0) {
    uint8_t c = pgm_read_byte( progmemBytes++ );
    a += c;
    b += a;
    gpsPort.write( c );
  }

  gpsPort.write( a ); // CHECKSUM A
  gpsPort.write( b ); // CHECKSUM B

} // sendUBX

//-------------------------------------------
// U-blox NMEA text commands

const char disableRMC[] PROGMEM = "PUBX,40,RMC,0,0,0,0,0,0";
const char disableGLL[] PROGMEM = "PUBX,40,GLL,0,0,0,0,0,0";
const char disableGSV[] PROGMEM = "PUBX,40,GSV,0,0,0,0,0,0";
const char disableGSA[] PROGMEM = "PUBX,40,GSA,0,0,0,0,0,0";
const char disableGGA[] PROGMEM = "PUBX,40,GGA,0,0,0,0,0,0";
const char disableVTG[] PROGMEM = "PUBX,40,VTG,0,0,0,0,0,0";
const char disableZDA[] PROGMEM = "PUBX,40,ZDA,0,0,0,0,0,0";

const char enableRMC[] PROGMEM = "PUBX,40,RMC,0,1,0,0,0,0";
const char enableGLL[] PROGMEM = "PUBX,40,GLL,0,1,0,0,0,0";
const char enableGSV[] PROGMEM = "PUBX,40,GSV,0,1,0,0,0,0";
const char enableGSA[] PROGMEM = "PUBX,40,GSA,0,1,0,0,0,0";
const char enableGGA[] PROGMEM = "PUBX,40,GGA,0,1,0,0,0,0";
const char enableVTG[] PROGMEM = "PUBX,40,VTG,0,1,0,0,0,0";
const char enableZDA[] PROGMEM = "PUBX,40,ZDA,0,1,0,0,0,0";

const char baud9600  [] PROGMEM = "PUBX,41,1,3,3,9600,0";
const char baud38400 [] PROGMEM = "PUBX,41,1,3,3,38400,0";
const char baud57600 [] PROGMEM = "PUBX,41,1,3,3,57600,0";
const char baud115200[] PROGMEM = "PUBX,41,1,3,3,115200,0";

//--------------------------

const uint32_t COMMAND_DELAY = 250;

void changeBaud( const char *textCommand, unsigned long baud )
{
  gps.send_P( &tee, (const __FlashStringHelper *) disableRMC );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableGLL );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableGSV );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableGSA );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableGGA );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableVTG );
  delay( COMMAND_DELAY );
  gps.send_P( &tee, (const __FlashStringHelper *) disableZDA );
  delay( 500 );
  gps.send_P( &tee, (const __FlashStringHelper *) textCommand );
  gpsPort.flush();
  gpsPort.end();

  DEBUG_PORT.print( F("All sentences disabled for baud rate ") );
  DEBUG_PORT.print( baud );
  DEBUG_PORT.println( F(" change.  Enter '1' to reenable sentences.") );
  delay( 500 );
  gpsPort.begin( baud );

} // changeBaud

//------------------------------------

static void doSomeWork()
{
  // Print all the things!

  if (tracing)
    trace_all( DEBUG_PORT, gps, fix_data );

} // doSomeWork

//--------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.println( F("ubloxRate.INO: started\r\n"
                        "Looking for GPS device on " GPS_PORT_NAME "\r\n"
                        "Enter '?' for help.") );

  #ifdef NMEAGPS_INTERRUPT_PROCESSING
    #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
  #endif

  #if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )

    DEBUG_PORT.println( F("\nWARNING: No NMEA sentences are enabled: no fix data will be displayed.") );

  #else
    if (gps.merging == NMEAGPS::NO_MERGING) {
      DEBUG_PORT.print  ( F("\nWARNING: displaying data from ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.print  ( F(" sentences ONLY, and only if ") );
      DEBUG_PORT.print  ( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
      DEBUG_PORT.println( F(" is enabled.\n"
                            "  Other sentences may be parsed, but their data will not be displayed.") );
    }
  #endif

  // Make sure the run-time selectable LAST_SENTENCE is
  //   configured correctly in NMEAGPS_cfg.h
  for (uint8_t i=0; i < 1; i++) {
    if (LastSentenceInInterval != LAST_SENTENCE_IN_INTERVAL) {
      DEBUG_PORT.println(
        F("LAST_SENTENCE_IN_INTERVAL is not properly defined in NMEAGPS_cfg.h!\n"
          "   See Prerequisite 4 above") );
      for (;;); // hang here!
    }
    LastSentenceInInterval ++;
  }
  LastSentenceInInterval = NMEAGPS::NMEA_GLL;

  trace_header( DEBUG_PORT );
  DEBUG_PORT.flush();

  gpsPort.begin( 9600 );
}

void loop()
{
  // Check for commands
  
  if (DEBUG_PORT.available()) {
    char c = DEBUG_PORT.read();

    switch (c) {
      case '?':
        DEBUG_PORT.print( (const __FlashStringHelper *) help );
        echoing = false;
        tracing = false;
        DEBUG_PORT.print( F("Enter command> ") );
        break;

      case '0':
        if (lastChar == 'r') {
          sendUBX( ubxRate10Hz, sizeof(ubxRate10Hz) );
        } else {
          gps.send_P( &tee, (const __FlashStringHelper *) disableRMC );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableGLL );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) disableGSV );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) disableGSA );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) disableGGA );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) disableVTG );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) disableZDA );
          LastSentenceInInterval = NMEAGPS::NMEA_GLL;
        }
        break;
      case '1':
        if (lastChar == 'r') {
          sendUBX( ubxRate1Hz, sizeof(ubxRate1Hz) );
        } else {
          gps.send_P( &tee, (const __FlashStringHelper *) enableRMC );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableGLL );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableGSV );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableGSA );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableGGA );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableVTG );
          delay( COMMAND_DELAY );
          gps.send_P( &tee, (const __FlashStringHelper *) enableZDA );
          LastSentenceInInterval = NMEAGPS::NMEA_ZDA;
        }
        break;
      case '3':
        changeBaud( baud38400, 38400UL );
        break;
      case '5':
        if (lastChar == 'r') {
          sendUBX( ubxRate5Hz, sizeof(ubxRate5Hz) );
        } else {
          changeBaud( baud115200, 115200UL );
        }
        break;
      case '6':
        if (lastChar == 'r') {
          sendUBX( ubxRate16Hz, sizeof(ubxRate16Hz) );
        }
        break;
      case '7':
        changeBaud( baud57600, 57600UL );
        break;
      case '9':
        changeBaud( baud9600, 9600UL );
        break;

      case 'd':
        sendUBX( ubxDisableRMC, sizeof(ubxDisableRMC) );
          delay( COMMAND_DELAY );
        //sendUBX( ubxDisableGLL, sizeof(ubxDisableGLL) );
        sendUBX( ubxDisableGSV, sizeof(ubxDisableGSV) );
          delay( COMMAND_DELAY );
        sendUBX( ubxDisableGSA, sizeof(ubxDisableGSA) );
          delay( COMMAND_DELAY );
        sendUBX( ubxDisableGGA, sizeof(ubxDisableGGA) );
          delay( COMMAND_DELAY );
        sendUBX( ubxDisableVTG, sizeof(ubxDisableVTG) );
          delay( COMMAND_DELAY );
        sendUBX( ubxDisableZDA, sizeof(ubxDisableZDA) );
        LastSentenceInInterval = NMEAGPS::NMEA_GLL;
        break;

      case 'e':
        if (lastChar == 'r') {
          resetGPS();
        } else {
          echoing = !echoing;
        }
        break;

      case 't':
        tracing = !tracing;
        break;

      default: break;
    }
    lastChar = c;
  }

  //  Check for GPS data

  static bool displayingHex = false;

  if (echoing) {
    // Use advanced character-oriented methods to echo received characters to
    //    the Serial Monitor window.
    if (gpsPort.available()) {

      char c = gpsPort.read();

      if (((' ' <= c) && (c <= '~')) || (c == '\r') || (c == '\n')) {
        DEBUG_PORT.write( c );
        displayingHex = false;
      } else {
        if (!displayingHex) {
          displayingHex = true;
          DEBUG_PORT.print( F("0x") );
        }
        if (c < 0x10)
          DEBUG_PORT.write( '0' );
        DEBUG_PORT.print( (uint8_t) c, HEX );
        DEBUG_PORT.write( ' ' );
      }

      gps.handle( c );
      if (gps.available()) {
        fix_data = gps.read();

        if (displayingHex)
          displayingHex = false;
        DEBUG_PORT.println();
        doSomeWork();
      }
    }

  } else {

    // Use the normal fix-oriented methods to display fixes
    if (gps.available( gpsPort )) {
      fix_data = gps.read();
      doSomeWork();
      displayingHex = false;
    }
  }
}
