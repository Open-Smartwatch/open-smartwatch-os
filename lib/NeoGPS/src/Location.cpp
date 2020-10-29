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

#include "Location.h"

using namespace NeoGPS;

//---------------------------------------------------------------------
//  Calculate dLon with integers, less one bit to avoid overflow
//     (360 * 1e7 = 3600000000, which is too big).  
//     Retains precision when points are close together.

int32_t safeDLon( int32_t p2, int32_t p1 )
{
  int32_t dLonL;
  int32_t halfDLon = (p2/2 - p1/2);
  if (halfDLon < -1800000000L/2) {
    dLonL = (p2 + 1800000000L) - (p1 - 1800000000L);
  } else if (1800000000L/2 < halfDLon) {
    dLonL = (p2 - 1800000000L) - (p1 + 1800000000L);
  } else {
    dLonL = p2 - p1;
  }
  return dLonL;

} // safeDLon

//---------------------------------------------------------------------

float Location_t::DistanceRadians
  ( const Location_t & p1, const Location_t & p2 )
{
  int32_t dLonL   = safeDLon( p2.lon(), p1.lon() );
  int32_t dLatL   = p2.lat() - p1.lat();

  if ((abs(dLatL)+abs(dLonL)) < 1000) {
    //  VERY close together.  Just use equirect approximation with precise integers.
    //    This is not needed for accuracy (that I can measure), but it is
    //    a quicker calculation.
    return EquirectDistanceRadians( p1, p2 );
  }

  // Haversine calculation from http://www.movable-type.co.uk/scripts/latlong.html
  float dLat      = dLatL * RAD_PER_DEG * LOC_SCALE;
  float haverDLat = sin(dLat/2.0);
  haverDLat *= haverDLat; // squared
  
  float dLon      = dLonL * RAD_PER_DEG * LOC_SCALE;
  float haverDLon = sin(dLon/2.0);
  haverDLon *= haverDLon; // squared
  
  float lat1 = p1.latF();
  lat1 *= RAD_PER_DEG;
  float lat2 = p2.latF();
  lat2 *= RAD_PER_DEG;
  float a = haverDLat + cos(lat1) * cos(lat2) * haverDLon;

  float c = 2 * atan2( sqrt(a), sqrt(1-a) );

  return c;

} // DistanceRadians

//---------------------------------------------------------------------

float Location_t::EquirectDistanceRadians
  ( const Location_t & p1, const Location_t & p2 )
{
  // Equirectangular calculation from http://www.movable-type.co.uk/scripts/latlong.html

  float dLat = (p2.lat() - p1.lat()) * RAD_PER_DEG * LOC_SCALE;
  float dLon = safeDLon( p2.lon(), p1.lon() ) * RAD_PER_DEG * LOC_SCALE;
  float x    = dLon * cos( p1.lat()  * RAD_PER_DEG * LOC_SCALE + dLat/2 );
  return sqrt( x*x + dLat*dLat );

} // EquirectDistanceRadians

//---------------------------------------------------------------------

float Location_t::BearingTo( const Location_t & p1, const Location_t & p2 )
{
  int32_t dLonL   = safeDLon( p2.lon(), p1.lon() );
  float   dLon    = dLonL * RAD_PER_DEG * LOC_SCALE;
  int32_t dLatL   = p2.lat() - p1.lat();
  float   lat1    = p1.lat() * RAD_PER_DEG * LOC_SCALE;
  float   cosLat1 = cos( lat1 );
  float   x, y, bearing;

  if ((abs(dLatL)+abs(dLonL)) < 1000) {
    //  VERY close together.  Just use equirect approximation with precise integers.
    x       = dLonL * cosLat1;
    y       = dLatL;
    bearing = PI/2.0 - atan2(y, x);

  } else {
    float lat2    = p2.lat() * RAD_PER_DEG * LOC_SCALE;
    float cosLat2 = cos(lat2);
    y             = sin(dLon) * cosLat2;
    x             = cosLat1   * sin(lat2) - sin(lat1) * cosLat2 * cos(dLon);
    bearing       = atan2(y, x);
  }

  if (bearing < 0.0)
    bearing += TWO_PI;

  return bearing;

} // BearingTo

//---------------------------------------------------------------------

void Location_t::OffsetBy( float distR, float bearingR )
{
  float lat1   = lat() * RAD_PER_DEG * LOC_SCALE;
  float newLat = asin( sin(lat1)*cos(distR) +  
                       cos(lat1)*sin(distR)*cos(bearingR) );
  float dLon   = atan2( sin(bearingR)*sin(distR)*cos(lat1),
                        cos(distR)-sin(lat1)*sin(newLat));

  _lat  = (newLat / (RAD_PER_DEG * LOC_SCALE));
  _lon += (dLon   / (RAD_PER_DEG * LOC_SCALE));

} // OffsetBy