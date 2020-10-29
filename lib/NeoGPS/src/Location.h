#ifndef NEOGPS_LOCATION_H
#define NEOGPS_LOCATION_H

#include "NeoGPS_cfg.h"

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

class NMEAGPS;

namespace NeoGPS {

class Location_t
{
public:
    CONST_CLASS_DATA float LOC_SCALE = 1.0e-7;

    Location_t() {}
    Location_t( int32_t lat, int32_t lon )
      : _lat(lat), _lon(lon)
        {}
    Location_t( float lat, float lon )
      : _lat(lat / LOC_SCALE), _lon(lon / LOC_SCALE)
        {}
    Location_t( double lat, double lon )
      : _lat(lat / LOC_SCALE), _lon(lon / LOC_SCALE)
        {}

    int32_t  lat() const      { return _lat; };
    void     lat( int32_t l ) { _lat = l; };
    float    latF() const     { return ((float) lat()) * LOC_SCALE; };
    void     latF( float v )  { _lat = v / LOC_SCALE; };

    int32_t  lon() const { return _lon; };
    void     lon( int32_t l ) { _lon = l; };
    float    lonF() const     { return ((float) lon()) * LOC_SCALE; };
    void     lonF( float v )  { _lon = v / LOC_SCALE; };

    void init() { _lat = _lon = 0; };

    CONST_CLASS_DATA float EARTH_RADIUS_KM = 6371.0088;
    CONST_CLASS_DATA float RAD_PER_DEG     = PI / 180.0;
    CONST_CLASS_DATA float DEG_PER_RAD     = 180.0 / PI;
    CONST_CLASS_DATA float MI_PER_KM       = 0.621371;

    //-----------------------------------
    // Distance calculations

    static float DistanceKm( const Location_t & p1, const Location_t & p2 )
      {
        return DistanceRadians( p1, p2 ) * EARTH_RADIUS_KM;
      }
    float DistanceKm( const Location_t & p2 ) const
      { return DistanceKm( *this, p2 ); }

    static float DistanceMiles( const Location_t & p1, const Location_t & p2 )
      {
        return DistanceRadians( p1, p2 ) * EARTH_RADIUS_KM * MI_PER_KM;
      }
    float DistanceMiles( const Location_t & p2 ) const
      { return DistanceMiles( *this, p2 ); }

    static float DistanceRadians( const Location_t & p1, const Location_t & p2 );
    float DistanceRadians( const Location_t & p2 ) const
      { return DistanceRadians( *this, p2 ); }

    static float EquirectDistanceRadians
      ( const Location_t & p1, const Location_t & p2 );
    float EquirectDistanceRadians( const Location_t & p2 ) const
      { return EquirectDistanceRadians( *this, p2 ); }

    static float EquirectDistanceKm( const Location_t & p1, const Location_t & p2 )
      {
        return EquirectDistanceRadians( p1, p2 ) * EARTH_RADIUS_KM;
      }
    float EquirectDistanceKm( const Location_t & p2 ) const
      { return  EquirectDistanceKm( *this, p2 ); }

    static float EquirectDistanceMiles( const Location_t & p1, const Location_t & p2 )
      {
        return EquirectDistanceRadians( p1, p2 ) * EARTH_RADIUS_KM * MI_PER_KM;
      }
    float EquirectDistanceMiles( const Location_t & p2 ) const
      { return  EquirectDistanceMiles( *this, p2 ); }

    //-----------------------------------
    // Bearing calculations

    static float BearingTo( const Location_t & p1, const Location_t & p2 ); // radians
    float BearingTo( const Location_t & p2 ) const // radians
      { return BearingTo( *this, p2 ); }

    static float BearingToDegrees( const Location_t & p1, const Location_t & p2 )
      { return BearingTo( p1, p2 ) * DEG_PER_RAD; }
    float BearingToDegrees( const Location_t & p2 ) const // radians
      { return BearingToDegrees( *this, p2 ); }

    //-----------------------------------
    // Offset a location (note distance is in radians, not degrees)
    void OffsetBy( float distR, float bearingR );

//private: //---------------------------------------
    friend class NMEAGPS; // This does not work?!?

    int32_t       _lat;  // degrees * 1e7, negative is South
    int32_t       _lon;  // degrees * 1e7, negative is West

} NEOGPS_PACKED;

} // NeoGPS

#endif