#ifndef STREAMERS_H
#define STREAMERS_H

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

#include <Arduino.h>

extern Print & operator <<( Print & outs, const bool b );
extern Print & operator <<( Print & outs, const char c );
extern Print & operator <<( Print & outs, const uint16_t v );
extern Print & operator <<( Print & outs, const uint32_t v );
extern Print & operator <<( Print & outs, const int32_t v );
extern Print & operator <<( Print & outs, const uint8_t v );
extern Print & operator <<( Print & outs, const __FlashStringHelper *s );

class gps_fix;

/**
 * Print valid fix data to the given stream with the format
 *   "status,dateTime,lat,lon,heading,speed,altitude,satellites,
 *       hdop,vdop,pdop,lat_err,lon_err,alt_err"
 * The "header" above contains the actual compile-time configuration.
 * A comma-separated field will be empty if the data is NOT valid.
 * @param[in] outs output stream.
 * @param[in] fix gps_fix instance.
 * @return iostream.
 */
extern Print & operator <<( Print &outs, const gps_fix &fix );

class NMEAGPS;

extern void trace_header( Print & outs );
extern void trace_all( Print & outs, const NMEAGPS &gps, const gps_fix &fix );

#endif