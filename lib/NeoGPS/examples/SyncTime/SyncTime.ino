#include <NMEAGPS.h>

//======================================================================
//  Program: SyncTime.ino
//
//  Description:  This program shows how to update the sub-second
//       part of a clock's seconds.  You can adjust the clock update
//       interval to be as small as 9ms.  It will calculate the
//       correct ms offset from each GPS second.
//
//  Prerequisites:
//     1) NMEA.ino works with your device
//     2) GPS_FIX_TIME is enabled in GPSfix_cfg.h
//     3) NMEAGPS_PARSE_RMC is enabled in NMEAGPS_cfg.h.  You could use 
//        any sentence that contains a time field.  Be sure to change the 
//        "if" statement in GPSloop from RMC to your selected sentence.
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

static NMEAGPS gps;
static gps_fix fix;

#if !defined(GPS_FIX_TIME)
  #error You must define GPS_FIX_TIME in GPSfix_cfg.h!
#endif

#if !defined(NMEAGPS_PARSE_RMC) & \
    !defined(NMEAGPS_PARSE_GLL) & \
    !defined(NMEAGPS_PARSE_GGA) & \
    !defined(NMEAGPS_PARSE_GST)
  #error You must define NMEAGPS_PARSE_RMC, GLL, GGA or GST in NMEAGPS_cfg.h!
#endif

#if !defined(NMEAGPS_TIMESTAMP_FROM_INTERVAL) & \
    !defined(NMEAGPS_TIMESTAMP_FROM_PPS)
  #error You must define NMEAGPS_TIMESTAMP_FROM_INTERVAL or PPS in NMEAGPS_cfg.h!
#endif

#if defined(NMEAGPS_TIMESTAMP_FROM_PPS)
  #warning You must modify this sketch to call gps.UTCsecondStart at the PPS rising edge (see NMEAGPS.h comments).
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

//----------------------------------------------------------------

static const uint32_t CLOCK_INTERVAL_MS = 100UL;
static       uint32_t lastShowTime = CLOCK_INTERVAL_MS+1; // First time never matches

//----------------------------------------------------------------

static void showTime( uint16_t subs, uint16_t factor = 100 /* hundredths */ )
{
  uint8_t showSeconds = fix.dateTime.seconds;
  
  // Step by seconds until we're in the current UTC second
  while (subs >= 1000UL) {
    subs -= 1000UL;
    if (showSeconds < 59)
      showSeconds++;
    else
      showSeconds = 0;
    //DEBUG_PORT.print( '+' );
  }

  DEBUG_PORT.print( showSeconds );
  DEBUG_PORT.print( '.' );
  
  // Leading zeroes
  for (;;) {
    factor /= 10;
    if ((factor < 10) || (subs >= factor))
      break;
    DEBUG_PORT.print( '0' );
  }

  DEBUG_PORT.println( subs );

} // showTime

//--------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("SyncTime.INO: started\n") );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );
  DEBUG_PORT.println( F("Local time seconds.milliseconds") );
  DEBUG_PORT.flush();

  gpsPort.begin( 9600 );
}

//--------------------------

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();
  }

  if (fix.valid.time) {
    uint32_t UTCms = gps.UTCms();

    if (((UTCms % CLOCK_INTERVAL_MS) == 0) && (UTCms != lastShowTime)) {
      showTime( UTCms, 1000 );
      lastShowTime = UTCms;
    }
  }
}
