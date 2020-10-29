#ifndef DMS_H
#define DMS_H

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
#include <stdint.h>
class Print;

enum Hemisphere_t { NORTH_H = 0, SOUTH_H = 1, EAST_H = 0, WEST_H = 1 };

class DMS_t
{
public:
  uint8_t      degrees;
  uint8_t      minutes       ;//NEOGPS_BF(6);
  Hemisphere_t hemisphere    ;//NEOGPS_BF(2); compiler bug!
  uint8_t      seconds_whole NEOGPS_BF(6);
  uint16_t     seconds_frac  NEOGPS_BF(10); // 1000ths

  void init() { degrees = minutes = seconds_whole = seconds_frac = 0;
                hemisphere = NORTH_H; }

  float secondsF() const { return seconds_whole + 0.001 * seconds_frac; };
  char  NS      () const { return (hemisphere == SOUTH_H) ? 'S' : 'N'; };
  char  EW      () const { return (hemisphere ==  WEST_H) ? 'W' : 'E'; };

  //.............................................................................
  // A utility function to convert from integer 'lat' or 'lon', scaled by 10^7

  void  From( int32_t deg_1E7 );

  // Print DMS as the funky NMEA DDDMM.mmmm format
  void printDDDMMmmmm( Print & outs ) const;

} NEOGPS_PACKED;

extern Print & operator << ( Print & outs, const DMS_t & );

#endif