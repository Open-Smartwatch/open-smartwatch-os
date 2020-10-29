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

#include "DMS.h"

#include <Print.h>

//----------------------------------------------------------------
//   Note that no division is used, and shifts are on byte boundaries.  Fast!

void DMS_t::From( int32_t deg_1E7 )
{
  const uint32_t E7 = 10000000UL;

  if (deg_1E7 < 0) {
    deg_1E7 = -deg_1E7;
    hemisphere = SOUTH_H; // or WEST_H
  } else
    hemisphere = NORTH_H; // or EAST_H

  const uint32_t div_E32 = 429; // 1e-07 * 2^32
  degrees = ((deg_1E7 >> 16) * div_E32) >> 16;
  uint32_t remainder = deg_1E7 - degrees * E7;

  remainder *= 60; // to minutes * E7
  minutes = ((remainder >> 16) * div_E32) >> 16;
  remainder -= minutes * E7;

  remainder *= 60; // to seconds * E7
  uint32_t secs = ((remainder >> 16) * div_E32) >> 16;
  remainder -= secs * E7;

  const uint32_t div_1E4_E24 = 1677; // 0.00001 * 2^24
  seconds_frac  = (((remainder >> 8) * div_1E4_E24) >> 16); // thousandths
  seconds_whole = secs;

  // Carry if thousandths too big
  if (seconds_frac >= 1000) {
    seconds_frac -= 1000;
    seconds_whole++;
    if (seconds_whole >= 60) {
      seconds_whole -= 60;
      minutes++;
      if (minutes >= 60) {
        minutes -= 60;
        degrees++;
      }
    }
  }

} // From

//----------------------------------------------------------------

Print & operator << ( Print & outs, const DMS_t & dms )
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

  return outs;
  
} // operator <<

//----------------------------------------------------------------

void DMS_t::printDDDMMmmmm( Print & outs ) const
{
  outs.print( degrees );

  if (minutes < 10)
    outs.print( '0' );
  outs.print( minutes );
  outs.print( '.' );

  //  Calculate the fractional minutes from the seconds,
  //     *without* using floating-point numbers.

  uint16_t mmmm = seconds_whole * 166;  // same as 10000/60, less .66666...
  mmmm += (seconds_whole * 2 + seconds_frac/2 ) / 3;  // ... plus the remaining .66666
      // ... plus the seconds_frac, scaled by 10000/(60*1000) = 1/6, which
      //        is implemented above as 1/2 * 1/3 

  //  print leading zeroes, if necessary
  if (mmmm < 1000)
    outs.print( '0' );
  if (mmmm <  100)
    outs.print( '0' );
  if (mmmm <   10)
    outs.print( '0' );
  outs.print( mmmm );
}