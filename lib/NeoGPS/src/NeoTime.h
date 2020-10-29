#ifndef TIME_H
#define TIME_H

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
#include "CosaCompat.h"

namespace NeoGPS {

//------------------------------------------------------
//  Enable/disable run-time modification of the epoch.
//  If this is defined, epoch mutators are available.
//  If this is not defined, the epoch is a hard-coded constant.
//     Only epoch accessors are available.
//#define TIME_EPOCH_MODIFIABLE

/**
 * Number of seconds elapsed since January 1 of the Epoch Year,
 * 00:00:00 +0000 (UTC).
 */
typedef uint32_t clock_t;

const uint8_t  SECONDS_PER_MINUTE = 60;
const uint8_t  MINUTES_PER_HOUR   = 60;
const uint16_t SECONDS_PER_HOUR   = (uint16_t) SECONDS_PER_MINUTE * MINUTES_PER_HOUR;
const uint8_t  HOURS_PER_DAY      = 24;
const uint32_t SECONDS_PER_DAY    = (uint32_t) SECONDS_PER_HOUR * HOURS_PER_DAY;
const uint8_t  DAYS_PER_WEEK      = 7;

/**
 * Common date/time structure
 */
struct time_t {

  enum weekday_t {
    SUNDAY    = 1,
    MONDAY    = 2,
    TUESDAY   = 3,
    WEDNESDAY = 4,
    THURSDAY  = 5,
    FRIDAY    = 6,
    SATURDAY  = 7
  };

  // NTP epoch year and weekday (Monday)
  static const uint16_t NTP_EPOCH_YEAR      = 1900;
  static const uint8_t  NTP_EPOCH_WEEKDAY   = MONDAY;

  // POSIX epoch year and weekday (Thursday)
  static const uint16_t POSIX_EPOCH_YEAR    = 1970;
  static const uint8_t  POSIX_EPOCH_WEEKDAY = THURSDAY;

  // Y2K epoch year and weekday (Saturday)
  static const uint16_t Y2K_EPOCH_YEAR      = 2000;
  static const uint8_t  Y2K_EPOCH_WEEKDAY   = SATURDAY;

  uint8_t seconds;    //!< 00-59
  uint8_t minutes;    //!< 00-59
  uint8_t hours;      //!< 00-23
  uint8_t day;        //!< 01-07 Day of Week
  uint8_t date;       //!< 01-31 Day of Month
  uint8_t month;      //!< 01-12
  uint8_t year;       //!< 00-99

  /**
   * Constructor.
   */
  time_t() {}

  /**
   * Construct from seconds since the Epoch.
   * @param[in] c clock.
   */
  time_t(clock_t c);

  /**
   * Initialize to January 1 of the Epoch Year, 00:00:00
   */
  void init();

  /**
   * Convert to seconds.
   * @return seconds from epoch.
   */
  operator clock_t() const;

  /**
   * Offset by a number of seconds.
   * @param[in] offset in seconds.
   */
  void operator +=( clock_t offset )
    { *this = offset + operator clock_t(); }

  /**
   * Set day member from current value.  This is a relatively expensive
   * operation, so the weekday is only calculated when requested.
   */
  void set_day()
  {
    day = weekday_for(days());
  }

  /**
   * Convert to days.
   * @return days from January 1 of the epoch year.
   */
  uint16_t days() const;

  /**
   * Calculate day of the current year.
   * @return days from January 1, which is day zero.
   */
  uint16_t day_of_year() const;

  /**
   * Calculate 4-digit year from internal 2-digit year member.
   * @return 4-digit year.
   */
  uint16_t full_year() const
  {
    return full_year(year);
  }

  /**
   * Calculate 4-digit year from a 2-digit year
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static uint16_t full_year( uint8_t year )
  {
    uint16_t y = year;

    if (y < pivot_year())
      y += 100 * (epoch_year()/100 + 1);
    else
      y += 100 * (epoch_year()/100);

    return y;
  }

  /**
   * Determine whether the current year is a leap year.
   * @returns true if the two-digit /year/ member is a leap year.
   */
  bool is_leap() const
  {
    return is_leap(full_year());
  }

  /**
   * Determine whether the 4-digit /year/ is a leap year.
   * @param[in] year (4-digit).
   * @return true if /year/ is a leap year.
   */
  static bool is_leap(uint16_t year)
  {
    if (year % 4) return false;
    uint16_t y = year % 400;
    return (y == 0) || ((y != 100) && (y != 200) && (y != 300));
  }

  /**
   * Calculate how many days are in the specified year.
   * @param[in] year (4-digit).
   * @return number of days.
   */
  static uint16_t days_per(uint16_t year)
  {
    return (365 + is_leap(year));
  }

  /**
   * Determine the day of the week for the specified day number
   * @param[in] dayno number as counted from January 1 of the epoch year.
   * @return weekday number 1..7, as for the /day/ member.
   */
  static uint8_t weekday_for(uint16_t dayno)
  {
    return ((dayno+epoch_weekday()-1) % DAYS_PER_WEEK) + 1;
  }

  /**
   * Check that all members, EXCEPT FOR day, are set to a coherent date/time.
   * @return true if valid date/time.
   */
  bool is_valid() const
  {
    return
      ((year <= 99) &&
       (1 <= month) && (month <= 12) &&
       ((1 <= date) &&
        ((date <= pgm_read_byte(&days_in[month])) ||
         ((month == 2) && is_leap() && (date == 29)))) &&
       (hours   <= 23) &&
       (minutes <= 59) &&
       (seconds <= 59));
  }

  /**
   * Set the epoch year for all time_t operations. Note that the pivot
   * year defaults to the epoch_year % 100. Valid years will be in the
   * range epoch_year..epoch_year+99. Selecting a different pivot year
   * will slide this range to the right.
   * @param[in] y epoch year to set.
   * See also /full_year/.
   */
  #ifdef TIME_EPOCH_MODIFIABLE
    static void epoch_year(uint16_t y)
    {
      s_epoch_year = y;
      epoch_offset( s_epoch_year % 100 );
      pivot_year( epoch_offset() );
    }
  #endif

  /**
   * Get the epoch year.
   * @return year.
   */
  static uint16_t epoch_year()
  {
    return (s_epoch_year);
  }

  static uint8_t epoch_weekday()             { return s_epoch_weekday; };
#ifdef TIME_EPOCH_MODIFIABLE
  static void    epoch_weekday( uint8_t ew ) { s_epoch_weekday = ew; };
#endif

  /**
   * The pivot year determine the range of years WRT the epoch_year
   * For example, an epoch year of 2000 and a pivot year of 80 will
   * allow years in the range 1980 to 2079. Default 0 for Y2K_EPOCH.
   */
  static uint8_t pivot_year()             { return s_pivot_year; };
  #ifdef TIME_EPOCH_MODIFIABLE
    static void    pivot_year( uint8_t py ) { s_pivot_year = py;   };
  #endif

  #ifdef TIME_EPOCH_MODIFIABLE
    /**
     * Use the current year for the epoch year. This will result in the
     * best performance of conversions, but dates/times before January 1
     * of the epoch year cannot be represented.
     */
    static void use_fastest_epoch();
  #endif

  /**
   * Parse a character string and fill out members.
   * @param[in] s PROGMEM character string with format "YYYY-MM-DD HH:MM:SS".
   * @return success.
   */
  bool parse(str_P s);

  static const uint8_t days_in[] PROGMEM; // month index is 1..12, PROGMEM

protected:
  static uint8_t  epoch_offset() { return s_epoch_offset; };

  #ifdef TIME_EPOCH_MODIFIABLE
    static void     epoch_offset( uint8_t eo ) { s_epoch_offset = eo; };

    static uint16_t s_epoch_year;
    static uint8_t  s_pivot_year;
    static uint8_t  s_epoch_offset;
    static uint8_t  s_epoch_weekday;
  #else
    static const uint16_t s_epoch_year    = Y2K_EPOCH_YEAR;
    static const uint8_t  s_pivot_year    = s_epoch_year % 100;
    static const uint8_t  s_epoch_offset  = s_pivot_year;
    static const uint8_t  s_epoch_weekday = Y2K_EPOCH_WEEKDAY;
  #endif

} NEOGPS_PACKED;

}; // namespace NeoGPS

class Print;

/**
 * Print the date/time to the given stream with the format "YYYY-MM-DD HH:MM:SS".
 * @param[in] outs output stream.
 * @param[in] t time structure.
 * @return iostream.
 */
Print & operator <<( Print & outs, const NeoGPS::time_t &t );

#endif
