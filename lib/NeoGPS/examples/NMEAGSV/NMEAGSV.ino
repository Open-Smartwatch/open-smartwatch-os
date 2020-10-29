#include <NMEAGPS.h>

NMEAGPS  gps; // This parses the GPS characters
gps_fix  fix; // This holds on to the latest values

//======================================================================
//  Program: NMEAGSV.ino
//
//  Description:  Display satellites in view, as reported by the GSV sentences.
//
//  Prerequisites:
//     1) NMEA.ino works with your device (correct TX/RX pins and baud rate)
//     2) NMEAGPS_PARSE_SATELLITES and NMEAGPS_PARSE_SATELLITE_INFO are
//              enabled in NMEAGPS_cfg.h
//     3) The GSV sentence has been enabled in NMEAGPS_cfg.h.
//     4) Your device emits the GSV sentence (use NMEAorder.ino to confirm).
//     5) LAST_SENTENCE_IN_INTERVAL has been set to GSV (or any other enabled sentence)
//              in NMEAGPS_cfg.h (use NMEAorder.ino).
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2017, SlashDevin
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

#ifndef NMEAGPS_PARSE_GSV
  #error You must define NMEAGPS_PARSE_GSV in NMEAGPS_cfg.h!
#endif

#ifndef NMEAGPS_PARSE_SATELLITES
  #error You must define NMEAGPS_PARSE_SATELLITE in NMEAGPS_cfg.h!
#endif

#ifndef NMEAGPS_PARSE_SATELLITE_INFO
  #error You must define NMEAGPS_PARSE_SATELLITE_INFO in NMEAGPS_cfg.h!
#endif

//-----------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!Serial)
    ;
  DEBUG_PORT.print( F("NeoGPS GSV example started\n") );

  gpsPort.begin(9600);

} // setup

//-----------------

void loop()
{
  while (gps.available( gpsPort )) {
    fix = gps.read();

    displaySatellitesInView();
  }

} // loop

//-----------------

void displaySatellitesInView()
{
  DEBUG_PORT.print( gps.sat_count );
  DEBUG_PORT.print( ',' );

  for (uint8_t i=0; i < gps.sat_count; i++) {
    DEBUG_PORT.print( gps.satellites[i].id );
    DEBUG_PORT.print( ' ' );
    DEBUG_PORT.print( gps.satellites[i].elevation );
    DEBUG_PORT.print( '/' );
    DEBUG_PORT.print( gps.satellites[i].azimuth );
    DEBUG_PORT.print( '@' );
    if (gps.satellites[i].tracked)
      DEBUG_PORT.print( gps.satellites[i].snr );
    else
      DEBUG_PORT.print( '-' );
    DEBUG_PORT.print( F(", ") );
  }

  DEBUG_PORT.println();

} // displaySatellitesInView
