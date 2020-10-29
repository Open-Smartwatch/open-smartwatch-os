#include <NMEAGPS.h>
using namespace NeoGPS;

//======================================================================
//  Program: NMEAtest.ino
//
//  Prerequisites:
//     1) All NMEA standard messages and Satellite Information
//             are enabled.
//     2) All 'gps_fix' members are enabled.
//     3) All validation options are enabled.
//
//  Description:  This test program uses one GPGGA sentence 
//  to test the parser's:
//     1) robustness WRT dropped, inserted, and mangled characters
//     2) correctness WRT values extracted from the input stream
//     
//  Some care in testing must be taken because
//     1) The XOR-style checksum is not very good at catching errors.  
//     2) The '*' is a special character for delimiting the CRC.  If
//        it is changed, a CR/LF will allow the sentence to pass.
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2014-2017, SlashDevin
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

#include <Streamers.h>

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined(NMEAGPS_PARSE_GGA) | \
    !defined(NMEAGPS_PARSE_GLL) | \
    !defined(NMEAGPS_PARSE_GSA) | \
    !defined(NMEAGPS_PARSE_GST) | \
    !defined(NMEAGPS_PARSE_GSV) | \
    !defined(NMEAGPS_PARSE_RMC) | \
    !defined(NMEAGPS_PARSE_VTG) | \
    !defined(NMEAGPS_PARSE_ZDA)

  #error NMEAGPS_PARSE_GGA, GLL, GSA, GSV, RMC, VTG and ZDA must be defined in NMEAGPS_cfg.h!
#endif

#ifndef GPS_FIX_DATE
  #error GPS_FIX_DATE must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_TIME
  #error GPS_FIX_TIME must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_LOCATION
  #error GPS_FIX_LOCATION must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_LOCATION_DMS
  #error GPS_FIX_LOCATION_DMS must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_ALTITUDE
  #error GPS_FIX_ALTITUDE must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_SPEED
  #error GPS_FIX_SPEED must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_HEADING
  #error GPS_FIX_HEADING must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_SATELLITES
  #error GPS_FIX_SATELLITES must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_HDOP
  #error GPS_FIX_HDOP must be defined in GPSfix_cfg.h!
#endif

#ifndef GPS_FIX_GEOID_HEIGHT
  #error GPS_FIX_GEOID_HEIGHT must be defined in GPSfix_cfg.h!
#endif

static NMEAGPS gps;

//--------------------------
// Example sentences

struct LocVector_t
  {
    float range;
    float bearing;
  };

static Location_t AyersRock( -253448688L, 1310324914L );
//  -25.3448688,131.0324914
//  2520.692128,S,13101.949484,E
//  25 20' 41.528" S 131 1' 56.969" E
static const LocVector_t NiihauToAyersRock = { 9078.681, 238.33972 };

const char validRMC[] __PROGMEM =
  "$GPRMC,092725.00,A,2520.69213,S,13101.94948,E,"
    "0.004,77.52,091202,,,A*43\r\n";

//...........................

static Location_t ubloxHQ( 472852369L, 85630763L ); // near Zurich, Switzerland
// 47.2852369, 8.5630763
// 47 17' 6.840" N 008 33' 54.954" E
static const LocVector_t NiihauToUblox = { 12248.67, 8.0625 };

const char validGGA[] __PROGMEM =
  "$GPGGA,092725.00,4717.113993,N,00833.915904,E,"
    "1,8,1.01,499.6,M,48.0,M,,0*5C\r\n";

//...........................

static Location_t MacchuPicchu( -131628050L, -725455080L );
//  -13.162805, -72.545508
//  13.162805,S,72.545508,W
//  13 09' 46.098" S 72 32' 43.830" W
static const LocVector_t NiihauToMacchu = { 10315.93, 103.07306 };

const char validGGA2[] __PROGMEM =
  "$GPGGA,162254.00,1309.7683,S,7232.7305,W,1,03,2.36,2430.2,M,-25.6,M,,*7E\r\n";

//...........................

static Location_t DexterMO( 367944050L, -899586550L );
//  36.794405, -89.958655
//  36.794405,N,89.958655,W
//  36 47' 39.858" N 89 57' 31.158" W
static const LocVector_t NiihauToDexter = { 6865.319, 58.85472 };

const char validRMC2[] __PROGMEM =
  "$GPRMC,162254.00,A,3647.6643,N,8957.5193,W,0.820,188.36,110706,,,A*49\r\n";

//...........................

static Location_t NiihauHI( 218276210L, -1602448760L );
//   21.827621, -160.244876
//   21.827621,N,160.244876,W
//   21 49' 39.4356" N 160 14' 41.5536 W
static const LocVector_t NiihauToNiihau = { 0.0, 90.0 };

const char validRMC3[] __PROGMEM =
"$GPRMC,235959.99,A,2149.65726,N,16014.69256,W,8.690,359.99,051015,9.47,E,A*26\r\n";

// 218276212L, -1602448757L
// 21 49.65727' N  160 14.69254' W
// 21 49' 39.4362" N  160 14' 41.5524" W
const char validRMC4[] __PROGMEM =
"$GPRMC,235959.99,A,2149.65727,N,16014.69254,W,8.690,359.99,051015,9.47,E,A*25\r\n";
static const LocVector_t NiihauToNiihau2 = { 0.00003812513, 54.31585 };

//...........................

static Location_t JujaKenya( -10934552L, 370261835L );

//    -1.0934552, 37.0261835
//    01 05' 36.458" S 037 01' 42.140" E
static const LocVector_t NiihauToJuja = { 17046.24, 318.6483 };

const char validGLL[] __PROGMEM =
"$GNGLL,0105.60764,S,03701.70233,E,225627.00,A,A*6B\r\n";

//--------------------------------

const char mtk1[] __PROGMEM =
"$GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*63\r\n";
const char mtk2[] __PROGMEM =
"$GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C\r\n";
const char mtk3[] __PROGMEM =
"$GPVTG,165.48,T,,M,0.03,N,0.06,K,A*36\r\n";
const char mtk4[] __PROGMEM =
"$GPGSA,A,3,29,21,26,15,18,09,06,10,,,,,2.32,0.95,2.11*00\r\n";
const char mtk5[] __PROGMEM =
"$GPGSV,3,1,09,29,36,029,42,21,46,314,43,26,44,020,43,15,21,321,39*7D\r\n";
const char mtk6[] __PROGMEM =
"$GPGSV,3,2,09,18,26,314,40,09,57,170,44,06,20,229,37,10,26,084,37*77\r\n";
const char mtk7[] __PROGMEM =
"$GPGSV,3,3,09,07,,,26*73\r\n";
const char mtk7a[] __PROGMEM =
"$GLGSV,1,1,4,29,36,029,42,21,46,314,43,26,44,020,43,15,21,321,39*5E\r\n";
const char mtk8[] __PROGMEM =
"$GNGST,082356.00,1.8,,,,1.7,1.3,2.2*60\r\n";
const char mtk9[] __PROGMEM =
"$GNRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A,V*33\r\n";
const char mtk10[] __PROGMEM =
"$GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,*45\r\n";
const char mtk11[] __PROGMEM =
  "$GLZDA,225627.00,21,09,2015,00,00*70\r\n";

const char fpGGA00[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816900,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*49\r\n";
const char fpGGA01[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816901,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*48\r\n";
const char fpGGA02[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816902,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4B\r\n";
const char fpGGA03[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816903,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4A\r\n";
const char fpGGA04[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816904,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4D\r\n";
const char fpGGA05[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816905,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4C\r\n";
const char fpGGA06[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816906,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4F\r\n";
const char fpGGA07[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816907,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*4E\r\n";
const char fpGGA08[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816908,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*41\r\n";
const char fpGGA09[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816909,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*40\r\n";
const char fpGGA10[] __PROGMEM = "$GPGGA,092725.00,3242.9000,N,11705.816910,W,"
  "1,8,1.01,499.6,M,48.0,M,,0*48\r\n";

//--------------------------

static bool parse_P( const char *ptr )
{
    bool decoded = false;
    char c;

    gps.fix().init();
    while ( (c = pgm_read_byte( ptr++ )) != '\0' ) {
      if (NMEAGPS::DECODE_COMPLETED == gps.decode( c )) {
        decoded = true;
      }
    }

    return decoded;
}

//--------------------------

static void traceSample( const char *ptr, bool init = true )
{
    Serial << F("Input:  ") << (const __FlashStringHelper *) ptr;

    if (init)
      gps.data_init();
    bool decoded = parse_P( ptr );

    if (decoded)
      Serial << F("Results:  ");
    else
      Serial << F("Failed to decode!  ");

    trace_all( Serial, gps, gps.fix() );
    Serial << '\n';
}

//--------------------------

static uint8_t passed = 0;
static uint8_t failed = 0;

static void checkFix
  ( const char *msg, NMEAGPS::nmea_msg_t msg_type, gps_fix::status_t status,
    int32_t lat, int32_t lon,
    uint8_t latDeg, uint8_t latMin, uint8_t latSec, uint16_t latSecFrac, Hemisphere_t ns,
    uint8_t lonDeg, uint8_t lonMin, uint8_t lonSec, uint16_t lonSecFrac, Hemisphere_t ew,
    const LocVector_t & to    )
{
  const char *ptr = msg;
  for (;;) {
    char c = pgm_read_byte( ptr++ );
    if (!c) {
      Serial.print( F("FAILED to parse \"") );
      Serial.print( (const __FlashStringHelper *) msg );
      Serial.println( F("\"\n") );
      failed++;
      break;
    }
    if (NMEAGPS::DECODE_COMPLETED == gps.decode( c )) {
            bool      ok  = true;
      const gps_fix & fix = gps.fix();

      if (gps.nmeaMessage != msg_type) {
        Serial.print( F("FAILED wrong message type ") );
        Serial.println( gps.nmeaMessage );
        failed++;
        ok = false;
      }
      if (fix.status != status ) {
        Serial.print( F("FAILED wrong status ") );
        Serial.print( fix.status );
        Serial.print( F(" (expected ") );
        Serial.print( status );
        Serial.println( ')' );
        failed++;
        ok = false;
      }
      if (fix.latitudeL() != lat) {
        Serial.print( F("FAILED wrong latitude ") );
        Serial.print( fix.latitudeL() );
        Serial.print( F(" (expected ") );
        Serial.print( lat );
        Serial.println( ')' );
        failed++;
        ok = false;
      }
      if (fix.longitudeL() != lon) {
        Serial.print( F("FAILED wrong longitude ") );
        Serial.print( fix.longitudeL() );
        Serial.print( F(" (expected ") );
        Serial.print( lon );
        Serial.println( ')' );
        failed++;
        ok = false;
      }

      if (fix.latitudeDMS.degrees != latDeg) {
        Serial.print( F("FAILED wrong latitude degrees ") );
        Serial.print( fix.latitudeDMS.degrees );
        Serial.print( F(", expected ") );
        Serial.println( latDeg );
        failed++;
        ok = false;
      }
      if (fix.latitudeDMS.minutes != latMin) {
        Serial.print( F("FAILED wrong latitude minutes ") );
        Serial.print( fix.latitudeDMS.minutes );
        Serial.print( F(", expected ") );
        Serial.println( latMin );
        failed++;
        ok = false;
      }
      if (fix.latitudeDMS.seconds_whole != latSec) {
        Serial.print( F("FAILED wrong latitude seconds ") );
        Serial.print( fix.latitudeDMS.seconds_whole );
        Serial.print( F(", expected ") );
        Serial.println( latSec );
        failed++;
        ok = false;
      }
      int8_t fracDiff = (int8_t)(fix.latitudeDMS.seconds_frac - latSecFrac);
      const int8_t ieps     = 1;
      if (abs(fracDiff) > ieps) {
        Serial.print( F("FAILED wrong latitude seconds fraction ") );
        Serial.print( fix.latitudeDMS.seconds_frac );
        Serial.print( F(", expected ") );
        Serial.println( latSecFrac );
        failed++;
        ok = false;
      }
      if (fix.latitudeDMS.hemisphere    != ns) {
        Serial.print( F("FAILED wrong latitude NS ") );
        Serial.println( fix.latitudeDMS.NS() );
        failed++;
        ok = false;
      }

      if (fix.longitudeDMS.degrees != lonDeg) {
        Serial.print( F("FAILED wrong longitude degrees ") );
        Serial.print( fix.longitudeDMS.degrees );
        Serial.print( F(", expected ") );
        Serial.println( lonDeg );
        failed++;
        ok = false;
      }
      if (fix.longitudeDMS.minutes != lonMin) {
        Serial.print( F("FAILED wrong longitude minutes ") );
        Serial.print( fix.longitudeDMS.minutes );
        Serial.print( F(", expected ") );
        Serial.println( lonMin );
        failed++;
        ok = false;
      }
      if (fix.longitudeDMS.seconds_whole != lonSec) {
        Serial.print( F("FAILED wrong longitude seconds ") );
        Serial.print( fix.longitudeDMS.seconds_whole );
        Serial.print( F(", expected ") );
        Serial.println( lonSec );
        failed++;
        ok = false;
      }
      fracDiff = (int8_t)(fix.longitudeDMS.seconds_frac - lonSecFrac);
      if (abs(fracDiff) > ieps) {
        Serial.print( F("FAILED wrong longitude seconds fraction ") );
        Serial.print( fix.longitudeDMS.seconds_frac );
        Serial.print( F(", expected ") );
        Serial.println( lonSecFrac );
        failed++;
        ok = false;
      }
      if (fix.longitudeDMS.hemisphere    != ew) {
        Serial.print( F("FAILED wrong longitude EW ") );
        Serial.println( fix.longitudeDMS.EW() );
        failed++;
        ok = false;
      }

      char floatChars[16];
      float distance = NiihauHI.DistanceKm( fix.location );
      float diff     = abs( distance - to.range );
      if ( (diff/to.range) > 0.000001 ) {
        Serial.print( F("FAILED distance ") );
        dtostre( distance, floatChars, 6, DTOSTR_PLUS_SIGN );
        Serial.print( floatChars );
        Serial.print( F(" != ") );
        dtostre( to.range, floatChars, 6, DTOSTR_PLUS_SIGN );
        Serial.println( floatChars );
        failed++;
        ok = false;
      }

      float courseTo = NiihauHI.BearingToDegrees( fix.location );
      diff = abs( courseTo - to.bearing );
      if ( diff > 0.005 ) {
        Serial.print( F("FAILED bearing ") );
        dtostre( courseTo, floatChars, 6, DTOSTR_PLUS_SIGN );
        Serial.print( floatChars );
        Serial.print( F(" != ") );
        dtostre( to.bearing, floatChars, 6, DTOSTR_PLUS_SIGN );
        Serial.print( floatChars );
        failed++;
        ok = false;
      }

      if (ok)
        passed++;
      break;
    }
  }
}

//--------------------------

void setup()
{
  // Start the normal trace output
  Serial.begin(9600);

  Serial.print( F("NMEA test: started\n") );
  Serial.print( F("fix object size = ") );
  Serial.println( sizeof(gps_fix) );
  Serial.print( F("gps object size = ") );
  Serial.println( sizeof(NMEAGPS) );

  //  Some basic rejection tests
  Serial.println( F("Test rejection of all byte values") );
  for (uint16_t c=0; c < 256; c++) {
    if (c != '$') {
      if (NMEAGPS::DECODE_CHR_INVALID != gps.decode( (char)c )) {
        Serial.print( F("FAILED to reject single character ") );
        Serial.println( c );
        failed++;
        return;
      }
    }
  }
  passed++;

  Serial.println( F("Test rejection of multiple $") );
  for (uint16_t i=0; i < 256; i++) {
    if (NMEAGPS::DECODE_COMPLETED == gps.decode( '$' )) {
      Serial.print( F("FAILED to reject multiple '$' characters\n") );
      failed++;
      return;
    }
  }
  passed++;

  uint16_t validGGA_len = 0;

  // Insert a ' ' at each position of the test sentence
  Serial.println( F("Insert ' '") );
  uint16_t insert_at = 1;
  do {
    const char *ptr = validGGA;
    uint8_t j = 0;
    for (;;) {
      if (j++ == insert_at) {
        NMEAGPS::decode_t result = gps.decode( ' ' );
        if (gps.validateChars() || gps.validateFields()) {
          if (result == NMEAGPS::DECODE_COMPLETED) {
            Serial.print( F("FAILED incorrectly decoded ' ' @ pos ") );
            Serial.println( insert_at );
            failed++;
          } else if (gps.nmeaMessage != NMEAGPS::NMEA_UNKNOWN) {
            Serial.print( F("FAILED incorrectly accepted ' ' @ pos ") );
            Serial.println( insert_at );
            failed++;
          }
        }
      }
      char c = pgm_read_byte( ptr++ );
      if (!c) {
        if (validGGA_len == 0) {
          validGGA_len = j-1;
          Serial.print( F("Test string length = ") );
          Serial.println( validGGA_len );
        }
        break;
      }
      if (gps.decode( c ) == NMEAGPS::DECODE_COMPLETED) {
        Serial.print( F("FAILED incorrectly decoded @ pos ") );
        Serial.println( insert_at );
        failed++;
        //return;
      }
    }
  } while (++insert_at < validGGA_len-2);
  passed++;

  // Drop one character from each position in example sentence
  Serial.println( F("Drop character") );
  for (uint16_t i=0; i < validGGA_len-3; i++) {
    const char *ptr     = validGGA;
    uint8_t     firstInvalidPos = 0;
    char        dropped = 0;
    for (uint8_t j = 0;; j++) {
      char c = pgm_read_byte( ptr++ );
      if (!c || (c == '*')) break;

      if (j == i) {
        dropped = c;
      } else {
        NMEAGPS::decode_t result = gps.decode( c );
        if (result == NMEAGPS::DECODE_COMPLETED) {
          Serial.print( F("FAILED decoded after dropping '") );
          Serial << dropped;
          Serial.print( F("' at pos ") );
          Serial.println( i );
          failed++;
        } else if (gps.nmeaMessage == NMEAGPS::NMEA_UNKNOWN) {
          if (firstInvalidPos != 0)
            firstInvalidPos = j;
        }
      }
    }
    if (firstInvalidPos != 0) {
      Serial.print( F("  /*") );
      Serial.print( i );
      Serial.print( F("*/ ") );
      Serial.println( firstInvalidPos );
    }
  }
  passed++;

  // Mangle one character from each position in example sentence
  Serial.println( F("Mangle one character") );
  for (uint16_t i=0; i < validGGA_len-3; i++) {
    const char *ptr = validGGA;
    uint8_t j = 0;
    char replaced = 0;
    for (;;) {
      char c = pgm_read_byte( ptr++ );
      if (!c || (c == '*')) break;
      if (j++ == i)
        replaced = c++; // mangle means increment
      if (NMEAGPS::DECODE_COMPLETED == gps.decode( c )) {
        Serial.print( F("FAILED replacing '") );
        Serial << (uint8_t) replaced;
        Serial.print( F("' with '") );
        Serial << (uint8_t) (replaced+1);
        Serial.print( F("' at pos ") );
        Serial.println( i );
        failed++;
        break;
        //return;
      }
    }
  }
  passed++;

  //  Verify that exact values are extracted
  Serial.println( F("Verify parsed values") );
  {
    const char *ptr = validGGA;
    for (;;) {
      char c = pgm_read_byte( ptr++ );
      if (!c) {
        Serial.print( F("FAILED to parse \"") );
        Serial.print( (str_P) validGGA );
        Serial.println( F("\"\n") );
        failed++;
        break;
      }
      if (NMEAGPS::DECODE_COMPLETED == gps.decode( c )) {
        gps_fix expected;
        expected.dateTime.parse( PSTR("2002-12-09 09:27:25") );
        expected.dateTime_cs = 0;

        if (gps.nmeaMessage != NMEAGPS::NMEA_GGA) {
          Serial.print( F("FAILED wrong message type ") );
          Serial.println( gps.nmeaMessage );
          failed++;
          break;
        }
        if ((gps.fix().dateTime.hours   != expected.dateTime.hours  ) ||
            (gps.fix().dateTime.minutes != expected.dateTime.minutes) ||
            (gps.fix().dateTime.seconds != expected.dateTime.seconds) ||
            (gps.fix().dateTime_cs      != expected.dateTime_cs)) {
          Serial << F("FAILED wrong time ") << gps.fix().dateTime << '.' << gps.fix().dateTime_cs << F(" != ") << expected.dateTime << '.' << expected.dateTime_cs << '\n';
          failed++;
          break;
        }
        if (gps.fix().latitudeL() != 472852332L) {
          Serial.print( F("FAILED wrong latitude ") );
          Serial.println( gps.fix().latitudeL() );
          failed++;
          break;
        }
        if (gps.fix().longitudeL() != 85652651L) {
          Serial.print( F("FAILED wrong longitude ") );
          Serial.println( gps.fix().longitudeL() );
          failed++;
          break;
        }
        if (gps.fix().status != gps_fix::STATUS_STD) {
          Serial.print( F("FAILED wrong status ") );
          Serial.println( gps.fix().status );
          failed++;
          break;
        }
        if (gps.fix().satellites != 8) {
          Serial.print( F("FAILED wrong satellites ") );
          Serial.println( gps.fix().satellites );
          failed++;
          break;
        }
        if (gps.fix().hdop != 1010) {
          Serial.print( F("FAILED wrong HDOP ") );
          Serial.println( gps.fix().hdop );
          failed++;
          break;
        }
        if (gps.fix().altitude_cm() != 49960) {
          Serial.print( F("FAILED wrong altitude ") );
          Serial.println( gps.fix().longitudeL() );
          failed++;
          break;
        }
        break;
      }
    }
  }
  passed++;

  checkFix( validRMC , NMEAGPS::NMEA_RMC, gps_fix::STATUS_STD,
               -253448688L,  1310324913L,
               25, 20, 41, 528, SOUTH_H, 131, 1, 56, 969, EAST_H,
               NiihauToAyersRock );
  checkFix( validGGA , NMEAGPS::NMEA_GGA, gps_fix::STATUS_STD,
               472852332L,    85652651L,
               47, 17, 6, 840, NORTH_H, 8, 33, 54, 954, EAST_H,
               NiihauToUblox );
  if (gps.fix().geoidHeight_cm() != 4800) {
    Serial.print( F("FAILED wrong geoid height for 48.00") );
    Serial.println( gps.fix().geoidHeight_cm() );
    failed++;
  }
  checkFix( validGGA2, NMEAGPS::NMEA_GGA, gps_fix::STATUS_STD,
               -131628050L,  -725455083L,
               13, 9, 46, 98, SOUTH_H, 72, 32, 43, 830, WEST_H,
               NiihauToMacchu );
  if (gps.fix().geoidHeight_cm() != -2560) {
    Serial.print( F("FAILED wrong geoid height for -25.60") );
    Serial.println( gps.fix().geoidHeight_cm() );
    failed++;
  }
  checkFix( validRMC2, NMEAGPS::NMEA_RMC, gps_fix::STATUS_STD,
                367944050L,  -899586550L,
               36, 47, 39, 858, NORTH_H, 89, 57, 31, 158, WEST_H,
               NiihauToDexter );
  checkFix( validRMC3, NMEAGPS::NMEA_RMC, gps_fix::STATUS_STD,
                218276210L, -1602448760L,
               21, 49, 39, 436, NORTH_H, 160, 14, 41, 554, WEST_H,
               NiihauToNiihau );
  checkFix( validRMC4, NMEAGPS::NMEA_RMC, gps_fix::STATUS_STD,
                218276212L, -1602448757L,
               21, 49, 39, 436, NORTH_H, 160, 14, 41, 552, WEST_H,
               NiihauToNiihau2 );
  checkFix( validGLL , NMEAGPS::NMEA_GLL, gps_fix::STATUS_STD,
                -10934607L,   370283722L,
               1, 5, 36, 458, SOUTH_H, 37, 1, 42, 140, EAST_H,
               NiihauToJuja );
}

//--------------------------

void loop()
{
  Serial.print( F("PASSED ") );
  Serial << passed;
  Serial.println( F(" tests.") );

  if (failed) {
    Serial.print( F("FAILED ") );
    Serial << failed;
    Serial.println( F(" tests.") );
  } else {
    Serial << F("------ Samples ------\nResults format:\n  ");
    trace_header( Serial );
    Serial << '\n';

#ifdef NMEAGPS_STATS
    gps.statistics.init();
#endif

    traceSample( validGGA );
    traceSample( validGGA2 );
    traceSample( validRMC );
    traceSample( validRMC2 );
    traceSample( validRMC3 );
    traceSample( validGLL );
    traceSample( mtk1 );
    traceSample( mtk2 );
    traceSample( mtk3 );
    traceSample( mtk4 );
    traceSample( mtk5 );
    traceSample( mtk6, false );
    traceSample( mtk7, false );
    traceSample( mtk7a, false );
    traceSample( mtk8 );
    traceSample( mtk9 );
    traceSample( mtk10 );
    traceSample( mtk11 );
    if (!gps.fix().valid.date              ||
        (gps.fix().dateTime.date    != 21) ||
        (gps.fix().dateTime.month   != 9)  ||
        (gps.fix().dateTime.year    != 15) ||
        !gps.fix().valid.time              ||
        (gps.fix().dateTime.hours   != 22) ||
        (gps.fix().dateTime.minutes != 56) ||
        (gps.fix().dateTime.seconds != 27))
      Serial << F("********  ZDA not parsed correctly **********\n");

    /**
     * This next section displays incremental longitudes.
     * If you have defined USE_FLOAT in Streamers.cpp, this will show
     * how the conversion to /float/ causes loss of accuracy compared 
     * to the /uint32_t/ values.
     */
    Serial << F("--- floating point conversion tests ---\n\n");

    traceSample( fpGGA00 );
    traceSample( fpGGA01 );
    traceSample( fpGGA02 );
    traceSample( fpGGA03 );
    traceSample( fpGGA04 );
    traceSample( fpGGA05 );
    traceSample( fpGGA06 );
    traceSample( fpGGA07 );
    traceSample( fpGGA08 );
    traceSample( fpGGA09 );
    traceSample( fpGGA10 );
  }

  for (;;);
}
