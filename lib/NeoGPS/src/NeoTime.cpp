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

// For strtoul declaration
#include <stdlib.h>

#include <Print.h>

Print & operator<<( Print& outs, const NeoGPS::time_t& t )
{
  outs.print( t.full_year( t.year ) );
  outs.write( '-' );
  if (t.month < 10) outs.write( '0' );
  outs.print( t.month );
  outs.write( '-' );
  if (t.date < 10) outs.write( '0' );
  outs.print( t.date );
  outs.write( ' ' );
  if (t.hours < 10) outs.write( '0' );
  outs.print( t.hours );
  outs.write( ':' );
  if (t.minutes < 10) outs.write( '0' );
  outs.print( t.minutes );
  outs.write( ':' );
  if (t.seconds < 10) outs.write( '0' );
  outs.print( t.seconds );

  return outs;
}

using NeoGPS::time_t;

bool time_t::parse(str_P s)
{
  static size_t BUF_MAX = 32;
  char buf[BUF_MAX];
  strcpy_P(buf, s);
  char* sp = &buf[0];
  uint16_t value = strtoul(sp, &sp, 10);

  if (*sp != '-') return false;
  year = value % 100;
  if (full_year() != value) return false;

  value = strtoul(sp + 1, &sp, 10);
  if (*sp != '-') return false;
  month = value;

  value = strtoul(sp + 1, &sp, 10);
  if (*sp != ' ') return false;
  date = value;

  value = strtoul(sp + 1, &sp, 10);
  if (*sp != ':') return false;
  hours = value;

  value = strtoul(sp + 1, &sp, 10);
  if (*sp != ':') return false;
  minutes = value;

  value = strtoul(sp + 1, &sp, 10);
  if (*sp != 0) return false;
  seconds = value;

  return (is_valid());
}

#ifdef TIME_EPOCH_MODIFIABLE
  uint16_t time_t::s_epoch_year    = Y2K_EPOCH_YEAR;
  uint8_t  time_t::s_epoch_offset  = 0;
  uint8_t  time_t::s_epoch_weekday = Y2K_EPOCH_WEEKDAY;
  uint8_t  time_t::s_pivot_year    = 0;
#endif

const uint8_t time_t::days_in[] __PROGMEM = {
  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

time_t::time_t(clock_t c)
{
  uint16_t dayno = c / SECONDS_PER_DAY;
  c -= dayno * (uint32_t) SECONDS_PER_DAY;
  day = weekday_for(dayno);

  uint16_t y = epoch_year();
  for (;;) {
    uint16_t days = days_per( y );
    if (dayno < days) break;
    dayno -= days;
    y++;
  }
  bool leap_year = is_leap(y);
  y -= epoch_year();
  y += epoch_offset();
  while (y > 100)
    y -= 100;
  year = y;

  month = 1;
  for (;;) {
    uint8_t days = pgm_read_byte(&days_in[month]);
    if (leap_year && (month == 2)) days++;
    if (dayno < days) break;
    dayno -= days;
    month++;
  }
  date = dayno + 1;

  hours = c / SECONDS_PER_HOUR;

  uint16_t c_ms;
  if (hours < 18) // save 16uS
    c_ms = (uint16_t) c - (hours * (uint16_t) SECONDS_PER_HOUR);
  else
    c_ms = c - (hours * (uint32_t) SECONDS_PER_HOUR);
  minutes = c_ms / SECONDS_PER_MINUTE;
  seconds = c_ms - (minutes * SECONDS_PER_MINUTE);
}

void time_t::init()
{
  seconds =
  hours   =
  minutes = 0;
  date    = 1;
  month   = 1;
  year    = epoch_year() % 100;
  day     = epoch_weekday();
}

time_t::operator clock_t() const
{
  clock_t c = days() * SECONDS_PER_DAY;
  if (hours < 18)
    c += hours * (uint16_t) SECONDS_PER_HOUR;
  else
    c += hours * (uint32_t) SECONDS_PER_HOUR;
  c += minutes * (uint16_t) SECONDS_PER_MINUTE;
  c += seconds;

  return (c);
}

uint16_t time_t::days() const
{
  uint16_t day_count = day_of_year();

  uint16_t y = full_year();
  while (y-- > epoch_year())
    day_count += days_per(y);

  return (day_count);
}

uint16_t time_t::day_of_year() const
{
  uint16_t dayno = date - 1;
  bool leap_year = is_leap();

  for (uint8_t m = 1; m < month; m++) {
    dayno += pgm_read_byte(&days_in[m]);
    if (leap_year && (m == 2)) dayno++;
  }

  return (dayno);
}

#ifdef TIME_EPOCH_MODIFIABLE
  void time_t::use_fastest_epoch()
  {
    // Figure out when we were compiled and use the year for a really
    // fast epoch_year. Format "MMM DD YYYY"
    const char* compile_date = (const char *) PSTR(__DATE__);
    uint16_t    compile_year = 0;
    for (uint8_t i = 7; i < 11; i++)
      compile_year = compile_year*10 + (pgm_read_byte(&compile_date[i]) - '0');

    // Temporarily set a Y2K epoch so we can figure out the day for
    // January 1 of this year
    epoch_year      ( Y2K_EPOCH_YEAR );
    epoch_weekday   ( Y2K_EPOCH_WEEKDAY );

    time_t this_year(0);
    this_year.year = compile_year % 100;
    this_year.set_day();
    uint8_t compile_weekday = this_year.day;

    epoch_year   ( compile_year );
    epoch_weekday( compile_weekday );
    pivot_year   ( this_year.year );
  }
#endif