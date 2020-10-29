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

#include "Streamers.h"
#include "NMEAGPS.h"

//#define USE_FLOAT

Print& operator <<( Print &outs, const bool b )
  { outs.print( b ? 't' : 'f' ); return outs; }

Print& operator <<( Print &outs, const char c ) { outs.print(c); return outs; }

Print& operator <<( Print &outs, const uint16_t v ) { outs.print(v); return outs; }

Print& operator <<( Print &outs, const uint32_t v ) { outs.print(v); return outs; }

Print& operator <<( Print &outs, const int32_t v ) { outs.print(v); return outs; }

Print& operator <<( Print &outs, const uint8_t v ) { outs.print(v); return outs; }

Print& operator <<( Print &outs, const __FlashStringHelper *s )
{ outs.print(s); return outs; }

//------------------------------------------

const char gps_fix_header[] __PROGMEM =
  "Status,"

  #if defined(GPS_FIX_DATE) | defined(GPS_FIX_TIME)

    "UTC "

  #if defined(GPS_FIX_DATE)
    "Date"
  #endif
  #if defined(GPS_FIX_DATE) & defined(GPS_FIX_TIME)
    "/"
  #endif
  #if defined(GPS_FIX_TIME)
    "Time"
  #endif

  #else
    "s"
  #endif

  ","

  #ifdef GPS_FIX_LOCATION
    "Lat,Lon,"
  #endif

  #ifdef GPS_FIX_LOCATION_DMS
    "DMS,"
  #endif

  #if defined(GPS_FIX_HEADING)
    "Hdg,"
  #endif

  #if defined(GPS_FIX_SPEED)
    "Spd,"
  #endif

  #ifdef GPS_FIX_VELNED
    "Vel N,E,D,"
  #endif

  #if defined(GPS_FIX_ALTITUDE)
    "Alt,"
  #endif

  #if defined(GPS_FIX_HDOP)
    "HDOP,"
  #endif

  #if defined(GPS_FIX_VDOP)
    "VDOP,"
  #endif

  #if defined(GPS_FIX_PDOP)
    "PDOP,"
  #endif

  #if defined(GPS_FIX_LAT_ERR)
    "Lat err,"
  #endif

  #if defined(GPS_FIX_LON_ERR)
    "Lon err,"
  #endif

  #if defined(GPS_FIX_ALT_ERR)
    "Alt err,"
  #endif

  #if defined(GPS_FIX_SPD_ERR)
    "Spd err,"
  #endif

  #if defined(GPS_FIX_HDG_ERR)
    "Hdg err,"
  #endif

  #if defined(GPS_FIX_TIME_ERR)
    "Time err,"
  #endif

  #if defined(GPS_FIX_GEOID_HEIGHT)
    "Geoid Ht,"
  #endif

  #if defined(GPS_FIX_SATELLITES)
    "Sats,"
  #endif

  ;

//...............

#ifdef GPS_FIX_LOCATION_DMS

  static void printDMS( Print & outs, const DMS_t & dms )
  {
    if (dms.degrees < 10)
      outs.write( '0' );
    outs.print( dms.degrees );
    outs.write( ' ' );

    if (dms.minutes < 10)
      outs.write( '0' );
    outs.print( dms.minutes );
    outs.print( F("\' ") );

    if (dms.seconds_whole < 10)
      outs.write( '0' );
    outs.print( dms.seconds_whole );
    outs.write( '.' );

    if (dms.seconds_frac < 100)
      outs.write( '0' );
    if (dms.seconds_frac < 10)
      outs.write( '0' );
    outs.print( dms.seconds_frac );
    outs.print( F("\" ") );

  } // printDMS

#endif
//...............

Print & operator <<( Print &outs, const gps_fix &fix )
{
  if (fix.valid.status)
    outs << (uint8_t) fix.status;
  outs << ',';

  #if defined(GPS_FIX_DATE) | defined(GPS_FIX_TIME)
    bool someTime = false;

    #if defined(GPS_FIX_DATE)
      someTime |= fix.valid.date;
    #endif

    #if defined(GPS_FIX_TIME)
      someTime |= fix.valid.time;
    #endif

    if (someTime) {
      outs << fix.dateTime << '.';
      uint16_t ms = fix.dateTime_ms();
      if (ms < 100)
        outs << '0';
      if (ms < 10)
        outs << '0';
      outs << ms;
    }
    outs << ',';

  #else

    //  Date/Time not enabled, just output the interval number
    static uint32_t sequence = 0L;
    outs << sequence++ << ',';

  #endif

  #ifdef USE_FLOAT
    #ifdef GPS_FIX_LOCATION
      if (fix.valid.location) {
        outs.print( fix.latitude(), 6 );
        outs << ',';
        outs.print( fix.longitude(), 6 );
      } else
        outs << ',';
      outs << ',';
    #endif
    #ifdef GPS_FIX_LOCATION_DMS
      if (fix.valid.location) {
        printDMS( outs, fix.latitudeDMS );
        outs.print( fix.latitudeDMS.NS() );
        outs.write( ' ' );
        if (fix.longitudeDMS.degrees < 100)
          outs.write( '0' );
        printDMS( outs, fix.longitudeDMS );
        outs.print( fix.longitudeDMS.EW() );
      }
      outs << ',';
    #endif
    #ifdef GPS_FIX_HEADING
      if (fix.valid.heading)
        outs.print( fix.heading(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_SPEED
      if (fix.valid.speed)
        outs.print( fix.speed(), 3 ); // knots
      outs << ',';
    #endif
    #ifdef GPS_FIX_VELNED
      if (fix.valid.velned)
        outs.print( fix.velocity_north ); // cm/s
      outs << ',';
      if (fix.valid.velned)
        outs.print( fix.velocity_east  ); // cm/s
      outs << ',';
      if (fix.valid.velned)
        outs.print( fix.velocity_down  ); // cm/s
      outs << ',';
    #endif
    #ifdef GPS_FIX_ALTITUDE
      if (fix.valid.altitude)
        outs.print( fix.altitude(), 2 );
      outs << ',';
    #endif

    #ifdef GPS_FIX_HDOP
      if (fix.valid.hdop)
        outs.print( (fix.hdop * 0.001), 3 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_VDOP
      if (fix.valid.vdop)
        outs.print( (fix.vdop * 0.001), 3 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_PDOP
      if (fix.valid.pdop)
        outs.print( (fix.pdop * 0.001), 3 );
      outs << ',';
    #endif

    #ifdef GPS_FIX_LAT_ERR
      if (fix.valid.lat_err)
        outs.print( fix.lat_err(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_LON_ERR
      if (fix.valid.lon_err)
        outs.print( fix.lon_err(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_ALT_ERR
      if (fix.valid.alt_err)
        outs.print( fix.alt_err(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_SPD_ERR
      if (fix.valid.spd_err)
        outs.print( fix.spd_err(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_HDG_ERR
      if (fix.valid.hdg_err)
        outs.print( fix.hdg_err(), 2 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_TIME_ERR
      if (fix.valid.time_err)
        outs.print( fix.time_err(), 2 );
      outs << ',';
    #endif

    #ifdef GPS_FIX_GEOID_HEIGHT
      if (fix.valid.geoidHeight)
        outs.print( fix.geoidHeight(), 2 );
      outs << ',';
    #endif

  #else

    // not USE_FLOAT ----------------------

    #ifdef GPS_FIX_LOCATION
      if (fix.valid.location)
        outs << fix.latitudeL() << ',' << fix.longitudeL();
      else
        outs << ',';
      outs << ',';
    #endif
    #ifdef GPS_FIX_LOCATION_DMS
      if (fix.valid.location) {
        printDMS( outs, fix.latitudeDMS );
        outs.print( fix.latitudeDMS.NS() );
        outs.write( ' ' );
        if (fix.longitudeDMS.degrees < 100)
          outs.write( '0' );
        printDMS( outs, fix.longitudeDMS );
        outs.print( fix.longitudeDMS.EW() );
      }
      outs << ',';
    #endif
    #ifdef GPS_FIX_HEADING
      if (fix.valid.heading)
        outs << fix.heading_cd();
      outs << ',';
    #endif
    #ifdef GPS_FIX_SPEED
      if (fix.valid.speed)
        outs << fix.speed_mkn();
      outs << ',';
    #endif
    #ifdef GPS_FIX_VELNED
      if (fix.valid.velned)
        outs.print( fix.velocity_north ); // cm/s
      outs << ',';
      if (fix.valid.velned)
        outs.print( fix.velocity_east  ); // cm/s
      outs << ',';
      if (fix.valid.velned)
        outs.print( fix.velocity_down  ); // cm/s
      outs << ',';
    #endif
    #ifdef GPS_FIX_ALTITUDE
      if (fix.valid.altitude)
        outs << fix.altitude_cm();
      outs << ',';
    #endif

    #ifdef GPS_FIX_HDOP
      if (fix.valid.hdop)
        outs << fix.hdop;
      outs << ',';
    #endif
    #ifdef GPS_FIX_VDOP
      if (fix.valid.vdop)
        outs << fix.vdop;
      outs << ',';
    #endif
    #ifdef GPS_FIX_PDOP
      if (fix.valid.pdop)
        outs << fix.pdop;
      outs << ',';
    #endif

    #ifdef GPS_FIX_LAT_ERR
      if (fix.valid.lat_err)
        outs << fix.lat_err_cm;
      outs << ',';
    #endif
    #ifdef GPS_FIX_LON_ERR
      if (fix.valid.lon_err)
        outs << fix.lon_err_cm;
      outs << ',';
    #endif
    #ifdef GPS_FIX_ALT_ERR
      if (fix.valid.alt_err)
        outs << fix.alt_err_cm;
      outs << ',';
    #endif
    #ifdef GPS_FIX_SPD_ERR
      if (fix.valid.spd_err)
        outs.print( fix.spd_err_mmps );
      outs << ',';
    #endif
    #ifdef GPS_FIX_HDG_ERR
      if (fix.valid.hdg_err)
        outs.print( fix.hdg_errE5 );
      outs << ',';
    #endif
    #ifdef GPS_FIX_TIME_ERR
      if (fix.valid.time_err)
        outs.print( fix.time_err_ns );
      outs << ',';
    #endif

    #ifdef GPS_FIX_GEOID_HEIGHT
      if (fix.valid.geoidHeight)
        outs << fix.geoidHeight_cm();
      outs << ',';
    #endif
    
  #endif

  #ifdef GPS_FIX_SATELLITES
    if (fix.valid.satellites)
      outs << fix.satellites;
    outs << ',';
  #endif

  return outs;
}

//-----------------------------

static const char NMEAGPS_header[] __PROGMEM =
  #if defined(NMEAGPS_TIMESTAMP_FROM_INTERVAL) | defined(NMEAGPS_TIMESTAMP_FROM_PPS)
    "micros(),"
  #endif

  #if defined(NMEAGPS_PARSE_SATELLITES)
    "[sat"
    #if defined(NMEAGPS_PARSE_SATELLITE_INFO)
      " elev/az @ SNR"
    #endif
    "],"
  #endif

  #ifdef NMEAGPS_STATS
    "Rx ok,Rx err,Rx chars,"
  #endif

  "";

void trace_header( Print & outs )
{
  outs.print( (const __FlashStringHelper *) &gps_fix_header[0] );
  outs.print( (const __FlashStringHelper *) &NMEAGPS_header[0] );

  outs << '\n';
}

//--------------------------

void trace_all( Print & outs, const NMEAGPS &gps, const gps_fix &fix )
{
  outs << fix;

  #if defined(NMEAGPS_TIMESTAMP_FROM_INTERVAL) | defined(NMEAGPS_TIMESTAMP_FROM_PPS)
    outs << gps.UTCsecondStart();
    outs << ',';
  #endif

  #if defined(NMEAGPS_PARSE_SATELLITES)
    outs << '[';

    for (uint8_t i=0; i < gps.sat_count; i++) {
      outs << gps.satellites[i].id;

      #if defined(NMEAGPS_PARSE_SATELLITE_INFO)
        outs << ' ' << 
          gps.satellites[i].elevation << '/' << gps.satellites[i].azimuth;
        outs << '@';
        if (gps.satellites[i].tracked)
          outs << gps.satellites[i].snr;
        else
          outs << '-';
      #endif

      outs << ',';
    }

    outs << F("],");
  #endif

  #ifdef NMEAGPS_STATS
    outs << gps.statistics.ok     << ','
         << gps.statistics.errors << ','
         << gps.statistics.chars  << ',';
  #endif

  outs << '\n';

} // trace_all
