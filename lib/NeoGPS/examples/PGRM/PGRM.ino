#include <NeoGPS_cfg.h>
#include <Garmin/GrmNMEA.h>
#include <GPSTime.h>

//======================================================================
//  Program: PGRM.ino
//
//  Description:  This program parses NMEA proprietary messages from
//     Garmin devices.  It is an extension of NMEA.ino.
//
//  Prerequisites:
//     1) You have a Garmin GPS device
//     2) NMEA.ino works with your device
//     3) At least one NMEA standard or proprietary sentence has been enabled.
//     4) Implicit Merging is disabled.
//
//  Serial is for debug output to the Serial Monitor window.
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
#include <Streamers.h>

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
    !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
    !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
    !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST ) & \
    !defined( GARMINGPS_PARSE_F )

  #error No NMEA sentences enabled: no fix data available.

#endif

#if !defined( GARMINGPS_PARSE_F )
  #error No PGRM messages enabled!  You must enable one or more in PGRM.h!
#endif

#ifndef NMEAGPS_DERIVED_TYPES
  #error You must "#define NMEAGPS_DERIVED_TYPES" in NMEAGPS_cfg.h!
#endif

#ifndef NMEAGPS_EXPLICIT_MERGING
  #error You must define NMEAGPS_EXPLICIT_MERGING in NMEAGPS_cfg.h
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

//------------------------------------------------------------

static GarminNMEA gps; // This parses received characters
static gps_fix    fix;

//----------------------------------------------------------------

static void doSomeWork()
{
  // Print leap seconds, when it finally arrives.
  static bool leapSecondsPrinted = false;
  if (not leapSecondsPrinted and (GPSTime::leap_seconds != 0)) {
    leapSecondsPrinted = true;
    DEBUG_PORT.print( F("GPS leap seconds = ") );
    DEBUG_PORT.println( GPSTime::leap_seconds );
  }

  // Print all the things!
  trace_all( DEBUG_PORT, gps, fix );

} // doSomeWork

//------------------------------------

static void GPSloop()
{  
  while (gps.available( gpsPort )) {
    fix = gps.read();

    doSomeWork();
  }
} // GPSloop
  
//--------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("PGRM: started\n") );
  DEBUG_PORT.print( F("fix object size = ") );
  DEBUG_PORT.println( sizeof(gps.fix()) );
  DEBUG_PORT.print( F("GarminNMEA object size = ") );
  DEBUG_PORT.println( sizeof(gps) );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );

  #ifdef GARMINGPS_PARSE_F
    if (LAST_SENTENCE_IN_INTERVAL != (NMEAGPS::nmea_msg_t) GarminNMEA::PGRMF) {
      DEBUG_PORT.println( F("ERROR! LAST_SENTENCE_IN_INTERVAL should be PGRMF") );
      for(;;);
    }
  #endif

  trace_header( DEBUG_PORT );
  DEBUG_PORT.flush();
  
  gpsPort.begin(9600);

}

//--------------------------

void loop()
{
  GPSloop();
}
