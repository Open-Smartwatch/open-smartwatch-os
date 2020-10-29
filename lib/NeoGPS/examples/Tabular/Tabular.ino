#include <NMEAGPS.h>

NMEAGPS     gps;

//======================================================================
//  Program: SyncTime.ino
//
//  Description:  This program displays all GPS fields in the default configuration
//     in a tabular display.  To be comparable to other libraries' tabular displays,
//     you must also enable HDOP in GPSfix_cfg.h.
//
//     Most NeoGPS examples display *all* configured GPS fields in a CSV format
//     (e.g., NMEA.ino).
//
//  Prerequisites:
//     1) NMEA.ino works with your device (correct TX/RX pins and baud rate)
//     2) GPS_FIX_HDOP is defined in GPSfix_cfg.h
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

#include <GPSport.h>

//-----------------
// Check configuration

#ifndef GPS_FIX_HDOP
  #error You must uncomment GPS_FIX_HDOP in GPSfix_cfg.h!
#endif

//-----------------

static const NeoGPS::Location_t London( 51.508131, -0.128002 );

void setup()
{
  DEBUG_PORT.begin(9600);
  
  DEBUG_PORT.println
    (
      F( "Testing NeoGPS library\n\n"
         "Sats HDOP Latitude  Longitude  Date       Time     Alt    Speed  Heading    -- To London --    Chars Sentences Errors\n"
         "          (deg)     (deg)                          (m)                      Dist    Dir\n" )
    );

  repeat( '-', 133 );

  gpsPort.begin(9600);
}

//-----------------

void loop()
{
  if (gps.available( gpsPort )) {
    gps_fix fix = gps.read();

    float bearingToLondon = fix.location.BearingToDegrees( London );
    bool  validDT         = fix.valid.date & fix.valid.time;

    print(             fix.satellites       , fix.valid.satellites, 3             );
    print(             fix.hdop/1000.0      , fix.valid.hdop      , 6, 2          );
    print(             fix.latitude ()      , fix.valid.location  , 10, 6         );
    print(             fix.longitude()      , fix.valid.location  , 11, 6         );
    print(             fix.dateTime         , validDT             , 20            );
    print(             fix.altitude ()      , fix.valid.altitude  , 7, 2          );
    print(             fix.speed_kph()      , fix.valid.speed     , 7, 2          );
    print(             fix.heading  ()      , fix.valid.heading   , 7, 2          );
    print( compassDir( fix.heading  () )    , fix.valid.heading   , 4             );
    print( fix.location.DistanceKm( London ), fix.valid.location  , 5             );
    print(             bearingToLondon      , fix.valid.location  , 7, 2          );
    print( compassDir( bearingToLondon )    , fix.valid.location  , 4             );

    print( gps.statistics.chars , true, 10 );
    print( gps.statistics.ok    , true,  6 );
    print( gps.statistics.errors, true,  6 );

    DEBUG_PORT.println();
  }
}

//-----------------
//  Print utilities

static void repeat( char c, int8_t len )
{
  for (int8_t i=0; i<len; i++)
    DEBUG_PORT.write( c );
}

static void printInvalid( int8_t len )
{
  DEBUG_PORT.write( ' ' );
  repeat( '*', abs(len)-1 );
}

static void print( float val, bool valid, int8_t len, int8_t prec )
{
  if (!valid) {
    printInvalid( len );
  } else {
    char s[16];
    dtostrf( val, len, prec, s );
    DEBUG_PORT.print( s );
  }
}

static void print( int32_t val, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    char s[16];
    ltoa( val, s, 10 );
    repeat( ' ', len - strlen(s) );
    DEBUG_PORT.print( s );
  }
}

static void print( const __FlashStringHelper *str, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    int slen = strlen_P( (const char *) str );
    repeat( ' ', len-slen );
    DEBUG_PORT.print( str );
  }
}

static void print( const NeoGPS::time_t & dt, bool valid, int8_t len )
{
  if (!valid) {
    printInvalid( len );
  } else {
    DEBUG_PORT.write( ' ' );
    Serial << dt; // this "streaming" operator outputs date and time
  }
}

//------------------------------------------------------------
//  This snippet is from NMEAaverage.  It keeps all the
//    compass direction strings in FLASH memory, saving RAM.

const char nCD  [] PROGMEM = "N";
const char nneCD[] PROGMEM = "NNE";
const char neCD [] PROGMEM = "NE";
const char eneCD[] PROGMEM = "ENE";
const char eCD  [] PROGMEM = "E";
const char eseCD[] PROGMEM = "ESE";
const char seCD [] PROGMEM = "SE";
const char sseCD[] PROGMEM = "SSE";
const char sCD  [] PROGMEM = "S";
const char sswCD[] PROGMEM = "SSW";
const char swCD [] PROGMEM = "SW";
const char wswCD[] PROGMEM = "WSW";
const char wCD  [] PROGMEM = "W";
const char wnwCD[] PROGMEM = "WNW";
const char nwCD [] PROGMEM = "NW";
const char nnwCD[] PROGMEM = "NNW";

const char * const dirStrings[] PROGMEM =
  { nCD, nneCD, neCD, eneCD, eCD, eseCD, seCD, sseCD, 
    sCD, sswCD, swCD, wswCD, wCD, wnwCD, nwCD, nnwCD };

const __FlashStringHelper *compassDir( uint16_t bearing ) // degrees CW from N
{
  const int16_t directions    = sizeof(dirStrings)/sizeof(dirStrings[0]);
  const int16_t degreesPerDir = 360 / directions;
        int8_t  dir           = (bearing + degreesPerDir/2) / degreesPerDir;

  while (dir < 0)
    dir += directions;
  while (dir >= directions)
    dir -= directions;

  return (const __FlashStringHelper *) pgm_read_ptr( &dirStrings[ dir ] );

} // compassDir
