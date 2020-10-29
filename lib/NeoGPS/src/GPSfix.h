#ifndef GPSFIX_H
#define GPSFIX_H

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

#include "NeoGPS_cfg.h"
#include "GPSfix_cfg.h"

#if defined( GPS_FIX_DATE ) | defined( GPS_FIX_TIME )
  #include "NeoTime.h"
#endif

#ifdef GPS_FIX_LOCATION_DMS
  #include "DMS.h"
#endif

#ifdef GPS_FIX_LOCATION
  #include "Location.h"
#endif

/**
 * A structure for holding a GPS fix: time, position, velocity, etc.
 *
 * Because GPS devices report various subsets of a coherent fix,
 * this class tracks which members of the fix are being reported:
 * each part has its own validity flag. Also, operator |= implements
 * merging multiple reports into one consolidated report.
 *
 * @section Limitations
 * Reports are not really fused with an algorithm; if present in
 * the source, they are simply replaced in the destination.
 *
 */

class gps_fix
{
public:

  // Default Constructor
  gps_fix() { init(); };

  //------------------------------------------------------------------
  // 'whole_frac' is a utility structure that holds the two parts
  //    of a floating-point number.
  //
  // This is used for Altitude, Heading and Speed, which require more
  //   significant digits than a 16-bit number.
  //
  // When parsing floating-point fields, the decimal point is used as
  //   a separator for these two parts.  This is much more efficient
  //   than calling 'long' or 'floating-point' math subroutines.
  //
  // This requires knowing the exponent on the fraction when a simple type
  //   (e.g., float or int) is needed.  For example, 'altitude()' knows that
  //   the whole part was stored as integer meters, and the fractional part
  //   was stored as integer centimeters.
  //
  // Unless you want the speed and precision of the two integer parts, you
  //   shouldn't have to use 'whole_frac'.  Instead, use the
  //   accessor functions for each of the specific fields for
  //   Altitude, Heading and Speed.

  struct whole_frac {
    int16_t whole;
    int16_t frac;
    void init() { whole = 0; frac = 0; };
    int32_t int32_00() const { return ((int32_t)whole) * 100L + frac; };
    int16_t int16_00() const { return whole * 100 + frac; };
    int32_t int32_000() const { return whole * 1000L + frac; };
    float float_00() const { return ((float)whole) + ((float)frac)*0.01; };
    float float_000() const { return ((float)whole) + ((float)frac)*0.001; };
  } NEOGPS_PACKED;

  //--------------------------------------------------------------
  // Members of a GPS fix
  //
  // Each member is separately enabled or disabled by the CFG file
  //   by #ifdef/#endif wrappers.
  // Each member has a storage declaration that depends on the
  //   precision and type of data available from GPS devices.
  // Some members have a separate accessor function that converts the
  //   internal storage type to a more common or convenient type
  //   for use by an application.
  // For example, latitude data is internally stored as a 32-bit integer,
  //   where the reported degrees have been multiplied by 10^7.  Many
  //   applications expect a floating-point value, so a floating-point
  //   accessor is provided: 'latitude()'.  This function converts the
  //   internal 32-bit integer to a floating-point value, and then
  //   divides it by 10^7.  The returned value is now a floating-point
  //   degrees value.

  #ifdef GPS_FIX_LOCATION
    NeoGPS::Location_t location;

    int32_t latitudeL() const { return location.lat (); };
    float   latitude () const { return location.latF(); }; // accuracy loss

    int32_t longitudeL() const { return location.lon (); };
    float   longitude () const { return location.lonF(); }; // accuracy loss
  #endif

  #ifdef GPS_FIX_LOCATION_DMS
    DMS_t latitudeDMS;
    DMS_t longitudeDMS;
  #endif

  #ifdef GPS_FIX_ALTITUDE
    whole_frac    alt; // .01 meters

    int32_t altitude_cm() const { return alt.int32_00(); };
    float   altitude   () const { return alt.float_00(); };
    float   altitude_ft() const { return altitude() * 3.28084; };
  #endif

  #ifdef GPS_FIX_VELNED
    int32_t  velocity_north;    // cm/s
    int32_t  velocity_east;     // cm/s
    int32_t  velocity_down;     // cm/s

    void calculateNorthAndEastVelocityFromSpeedAndHeading()
    {
      #if defined( GPS_FIX_HEADING ) && defined( GPS_FIX_SPEED )
        if (valid.heading && valid.speed && valid.velned) {

          float course         = heading() * NeoGPS::Location_t::RAD_PER_DEG;
          float speed_cm_per_s = speed_metersph() * (100.0 / 3600.0);
          velocity_north = round( speed_cm_per_s * cos( course ) );
          velocity_east  = round( speed_cm_per_s * sin( course ) );
          // velocity_down has already been set.

        }
      #endif
    }
  #endif

  #ifdef GPS_FIX_SPEED
    whole_frac    spd; // .001 nautical miles per hour

    uint32_t speed_mkn () const { return spd.int32_000(); };
    float    speed     () const { return spd.float_000(); };

    // Utilities for speed in other units
    CONST_CLASS_DATA float KM_PER_NMI = 1.852;
    float    speed_kph () const { return speed() * KM_PER_NMI; };

    CONST_CLASS_DATA uint32_t M_PER_NMI = 1852;
    uint32_t speed_metersph() const { return (spd.whole * M_PER_NMI) + (spd.frac * M_PER_NMI)/1000; };

    CONST_CLASS_DATA float MI_PER_NMI = 1.150779;
    float  speed_mph() const { return speed() * MI_PER_NMI; };
  #endif

  #ifdef GPS_FIX_HEADING
    whole_frac    hdg; //  .01 degrees

    uint16_t heading_cd() const { return hdg.int16_00(); };
    float    heading   () const { return hdg.float_00(); };
  #endif

  //--------------------------------------------------------
  // Dilution of Precision is a measure of the current satellite
  // constellation geometry WRT how 'good' it is for determining a
  // position.  This is _independent_ of signal strength and many
  // other factors that may be internal to the receiver.
  // It _cannot_ be used to determine position accuracy in meters.
  // Instead, use the LAT/LON/ALT error in cm members, which are
  //   populated by GST sentences.

  #ifdef GPS_FIX_HDOP
    uint16_t           hdop; // Horizontal Dilution of Precision x 1000
  #endif
  #ifdef GPS_FIX_VDOP
    uint16_t           vdop; // Vertical Dilution of Precision x 1000
  #endif
  #ifdef GPS_FIX_PDOP
    uint16_t           pdop; // Position Dilution of Precision x 1000
  #endif

  //--------------------------------------------------------
  //  Error estimates for lat, lon, altitude, speed, heading and time

  #ifdef GPS_FIX_LAT_ERR
    uint16_t lat_err_cm;
    float lat_err() const { return lat_err_cm / 100.0; }    // m
  #endif

  #ifdef GPS_FIX_LON_ERR
    uint16_t lon_err_cm;
    float lon_err() const { return lon_err_cm / 100.0; }    // m
  #endif

  #ifdef GPS_FIX_ALT_ERR
    uint16_t alt_err_cm;
    float alt_err() const { return alt_err_cm / 100.0; }    // m
  #endif

  #ifdef GPS_FIX_SPD_ERR
    uint16_t spd_err_mmps;
    float spd_err() const { return spd_err_mmps / 1000.0; } // m/s
  #endif

  #ifdef GPS_FIX_HDG_ERR
    uint16_t hdg_errE5;    // 0.00001 deg
    float hdg_err() const { return hdg_errE5 / 1.0e5; } // deg
  #endif

  #ifdef GPS_FIX_TIME_ERR
    uint16_t time_err_ns;
    float time_err() const { return time_err_ns / 1.0e9; } // s
  #endif

  //--------------------------------------------------------
  // Height of the geoid above the WGS84 ellipsoid
  #ifdef GPS_FIX_GEOID_HEIGHT
    whole_frac    geoidHt; // .01 meters

    int32_t geoidHeight_cm() const { return geoidHt.int32_00(); };
    float   geoidHeight   () const { return geoidHt.float_00(); };
  #endif

  //--------------------------------------------------------
  // Number of satellites used to calculate a fix.
  #ifdef GPS_FIX_SATELLITES
    uint8_t   satellites;
  #endif

  //--------------------------------------------------------
  //  Date and Time for the fix
  #if defined(GPS_FIX_DATE) | defined(GPS_FIX_TIME)
    NeoGPS::time_t  dateTime   ; // Date and Time in one structure
  #endif
  #if defined(GPS_FIX_TIME)
    uint8_t         dateTime_cs;         // The fix's UTC hundredths of a second
    uint32_t        dateTime_us() const  // The fix's UTC microseconds
                      { return dateTime_cs * 10000UL; };
    uint16_t        dateTime_ms() const  // The fix's UTC millseconds
                      { return dateTime_cs * 10; };
  #endif

  //--------------------------------------------------------
  // The current fix status or mode of the GPS device.
  //
  // Unfortunately, the NMEA sentences are a little inconsistent
  //   in their use of "status" and "mode". Both fields are mapped
  //   onto this enumerated type.  Be aware that different
  //   manufacturers interpret them differently.  This can cause
  //   problems in sentences which include both types (e.g., GPGLL).
  //
  // Note: Sorted by increasing accuracy.  See also /operator |=/.

  enum status_t {
    STATUS_NONE,
    STATUS_EST,
    STATUS_TIME_ONLY,
    STATUS_STD,
    STATUS_DGPS,
    STATUS_RTK_FLOAT,
    STATUS_RTK_FIXED,
    STATUS_PPS // Precise Position System, *NOT* Pulse-per-second
  };

  status_t  status NEOGPS_BF(8);

  //--------------------------------------------------------
  //  Flags to indicate which members of this fix are valid.
  //
  //  Because different sentences contain different pieces of a fix,
  //    each piece can be valid or NOT valid.  If the GPS device does not
  //    have good reception, some fields may not contain any value.
  //    Those empty fields will be marked as NOT valid.

  struct valid_t {
    bool status NEOGPS_BF(1);

    #if defined(GPS_FIX_DATE)
      bool date NEOGPS_BF(1);
    #endif

    #if defined(GPS_FIX_TIME)
      bool time NEOGPS_BF(1);
    #endif

    #if defined( GPS_FIX_LOCATION ) | defined( GPS_FIX_LOCATION_DMS )
      bool location NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_ALTITUDE
      bool altitude NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_SPEED
      bool speed NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_VELNED
      bool velned NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_HEADING
      bool heading NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_SATELLITES
      bool satellites NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_HDOP
      bool hdop NEOGPS_BF(1);
    #endif
    #ifdef GPS_FIX_VDOP
      bool vdop NEOGPS_BF(1);
    #endif
    #ifdef GPS_FIX_PDOP
      bool pdop NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_LAT_ERR
      bool lat_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_LON_ERR
      bool lon_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_ALT_ERR
      bool alt_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_SPD_ERR
      bool spd_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_HDG_ERR
      bool hdg_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_TIME_ERR
      bool time_err NEOGPS_BF(1);
    #endif

    #ifdef GPS_FIX_GEOID_HEIGHT
      bool geoidHeight NEOGPS_BF(1);
    #endif

    // Initialize all flags to false
    void init()
      {
        uint8_t *all = (uint8_t *) this;
        for (uint8_t i=0; i<sizeof(*this); i++)
          *all++ = 0;
      }

    // Merge these valid flags with another set of valid flags
    void operator |=( const valid_t & r )
      {
        uint8_t *all = (uint8_t *) this;
        const uint8_t *r_all = (const uint8_t *) &r;
        for (uint8_t i=0; i<sizeof(*this); i++)
          *all++ |= *r_all++;
      }
  } NEOGPS_PACKED
      valid;        // This is the name of the collection of valid flags

  //--------------------------------------------------------
  //  Initialize a fix.  All configured members are set to zero.

  void init()
  {
    #ifdef GPS_FIX_LOCATION
      location.init();
    #endif

    #ifdef GPS_FIX_LOCATION_DMS
      latitudeDMS.init();
      longitudeDMS.init();
    #endif

    #ifdef GPS_FIX_ALTITUDE
      alt.init();
    #endif

    #ifdef GPS_FIX_SPEED
      spd.init();
    #endif

    #ifdef GPS_FIX_VELNED
      velocity_north =
      velocity_east  =
      velocity_down  = 0;
    #endif

    #ifdef GPS_FIX_HEADING
      hdg.init();
    #endif

    #ifdef GPS_FIX_HDOP
      hdop = 0;
    #endif
    #ifdef GPS_FIX_VDOP
      vdop = 0;
    #endif
    #ifdef GPS_FIX_PDOP
      pdop = 0;
    #endif

    #ifdef GPS_FIX_LAT_ERR
      lat_err_cm = 0;
    #endif
    #ifdef GPS_FIX_LON_ERR
      lon_err_cm = 0;
    #endif
    #ifdef GPS_FIX_ALT_ERR
      alt_err_cm = 0;
    #endif
    #ifdef GPS_FIX_SPD_ERR
      spd_err_mmps = 0;
    #endif
    #ifdef GPS_FIX_HDG_ERR
      hdg_errE5 = 0;
    #endif
    #ifdef GPS_FIX_TIME_ERR
      time_err_ns = 0;
    #endif

    #ifdef GPS_FIX_GEOID_HEIGHT
      geoidHt.init();
    #endif

    #ifdef GPS_FIX_SATELLITES
      satellites = 0;
    #endif

    #if defined(GPS_FIX_DATE) | defined(GPS_FIX_TIME)
      dateTime.init();
    #endif
    #if defined(GPS_FIX_TIME)
      dateTime_cs = 0;
    #endif

    status = STATUS_NONE;

    valid.init();

  } // init

  //-------------------------------------------------------------
  // Merge valid fields from the right fix into a "fused" fix
  //   on the left (i.e., /this/).
  //
  // Usage:  gps_fix left, right;
  //         left |= right;  // explicit merge

  gps_fix & operator |=( const gps_fix & r )
  {
    // Replace /status/  only if the right is more "accurate".
    if (r.valid.status && (!valid.status || (status < r.status)))
      status = r.status;

    #ifdef GPS_FIX_DATE
      if (r.valid.date) {
        dateTime.date  = r.dateTime.date;
        dateTime.month = r.dateTime.month;
        dateTime.year  = r.dateTime.year;
      }
    #endif

    #ifdef GPS_FIX_TIME
      if (r.valid.time) {
        dateTime.hours   = r.dateTime.hours;
        dateTime.minutes = r.dateTime.minutes;
        dateTime.seconds = r.dateTime.seconds;
        dateTime_cs      = r.dateTime_cs;
      }
    #endif

    #ifdef GPS_FIX_LOCATION
      if (r.valid.location) {
        location = r.location;
      }
    #endif

    #ifdef GPS_FIX_LOCATION_DMS
      if (r.valid.location) {
        latitudeDMS  = r.latitudeDMS;
        longitudeDMS = r.longitudeDMS;
      }
    #endif

    #ifdef GPS_FIX_ALTITUDE
      if (r.valid.altitude)
        alt = r.alt;
    #endif

    #ifdef GPS_FIX_HEADING
      if (r.valid.heading)
        hdg = r.hdg;
    #endif

    #ifdef GPS_FIX_SPEED
      if (r.valid.speed)
        spd = r.spd;
    #endif

    #ifdef GPS_FIX_VELNED
      if (r.valid.velned) {
        velocity_north = r.velocity_north;
        velocity_east  = r.velocity_east;
        velocity_down  = r.velocity_down;
      }
    #endif

    #ifdef GPS_FIX_SATELLITES
      if (r.valid.satellites)
        satellites = r.satellites;
    #endif

    #ifdef GPS_FIX_HDOP
      if (r.valid.hdop)
        hdop = r.hdop;
    #endif

    #ifdef GPS_FIX_VDOP
      if (r.valid.vdop)
        vdop = r.vdop;
    #endif

    #ifdef GPS_FIX_PDOP
      if (r.valid.pdop)
        pdop = r.pdop;
    #endif

    #ifdef GPS_FIX_LAT_ERR
      if (r.valid.lat_err)
        lat_err_cm = r.lat_err_cm;
    #endif

    #ifdef GPS_FIX_LON_ERR
      if (r.valid.lon_err)
        lon_err_cm = r.lon_err_cm;
    #endif

    #ifdef GPS_FIX_ALT_ERR
      if (r.valid.alt_err)
        alt_err_cm = r.alt_err_cm;
    #endif

    #ifdef GPS_FIX_SPD_ERR
      if (r.valid.spd_err)
        spd_err_mmps = r.spd_err_mmps;
    #endif

    #ifdef GPS_FIX_HDG_ERR
      if (r.valid.hdg_err)
        hdg_errE5 = r.hdg_errE5;
    #endif

    #ifdef GPS_FIX_TIME_ERR
      if (r.valid.time_err)
        time_err_ns = r.time_err_ns;
    #endif

    #ifdef GPS_FIX_GEOID_HEIGHT
      if (r.valid.geoidHeight)
        geoidHt = r.geoidHt;
    #endif

    // Update all the valid flags
    valid |= r.valid;

    return *this;

  } // operator |=

} NEOGPS_PACKED;

#endif