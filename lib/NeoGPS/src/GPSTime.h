#ifndef GPSTIME_H
#define GPSTIME_H

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

#include "NeoTime.h"

class GPSTime
{
  GPSTime();

  static NeoGPS::clock_t s_start_of_week;

public:

    /**
     * GPS time is offset from UTC by a number of leap seconds.  To convert a GPS
     * time to UTC time, the current number of leap seconds must be known.
     * See http://en.wikipedia.org/wiki/Global_Positioning_System#Leap_seconds
     */
    static uint8_t leap_seconds;

    /**
     * Some receivers report time WRT start of the current week, defined as
     * Sunday 00:00:00.  To save fairly expensive date/time calculations,
     * the UTC start of week is cached
     */
    static void start_of_week( NeoGPS::time_t & now )
      {
        now.set_day();
        s_start_of_week =
          (NeoGPS::clock_t) now  -  
          (NeoGPS::clock_t) ((((now.day-1  ) * 24L + 
                                now.hours  ) * 60L + 
                                now.minutes) * 60L +
                                now.seconds);
      }

    static NeoGPS::clock_t start_of_week()
    {
      return s_start_of_week;
    }

    /*
     * Convert a GPS time-of-week to UTC.
     * Requires /leap_seconds/ and /start_of_week/.
     */
    static NeoGPS::clock_t TOW_to_UTC( uint32_t time_of_week )
      { return (NeoGPS::clock_t)
          (start_of_week() + time_of_week - leap_seconds); }

    /**
     * Set /fix/ timestamp from a GPS time-of-week in milliseconds.
     * Requires /leap_seconds/ and /start_of_week/.
     **/
    static bool from_TOWms
      ( uint32_t time_of_week_ms, NeoGPS::time_t &dt, uint16_t &ms )
    {
//trace << PSTR("from_TOWms(") << time_of_week_ms << PSTR("), sow = ") << start_of_week() << PSTR(", leap = ") << leap_seconds << endl;
      bool ok = (start_of_week() != 0) && (leap_seconds != 0);
      if (ok) {
        NeoGPS::clock_t tow_s = time_of_week_ms/1000UL;
        dt = TOW_to_UTC( tow_s ); 
        ms = (uint16_t)(time_of_week_ms - tow_s*1000UL);
      }
      return ok;
    }
};

#endif