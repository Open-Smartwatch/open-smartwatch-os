#ifndef UBX_CFG_H
#define UBX_CFG_H

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

//--------------------------------------------------------------------
// Enable/disable the parsing of specific UBX messages.
//
// Configuring out a message prevents its fields from being parsed.
// However, the message type will still be recognized by /decode/ and 
// stored in member /rx_msg/.  No valid flags would be available.

#define UBLOX_PARSE_STATUS
#define UBLOX_PARSE_TIMEGPS
#define UBLOX_PARSE_TIMEUTC
#define UBLOX_PARSE_POSLLH
//#define UBLOX_PARSE_DOP
//#define UBLOX_PARSE_PVT
#define UBLOX_PARSE_VELNED
//#define UBLOX_PARSE_SVINFO
//#define UBLOX_PARSE_CFGNAV5
//#define UBLOX_PARSE_MONVER
//#define UBLOX_PARSE_HNR_PVT

#if defined(UBLOX_PARSE_DOP) & \
    ( !defined(GPS_FIX_HDOP) & \
      !defined(GPS_FIX_VDOP) & \
      !defined(GPS_FIX_PDOP) )
  #warning UBX DOP message is enabled, but all gps_fix DOP members are disabled.
#endif

//--------------------------------------------------------------------
// Identify the last UBX message in an update interval.
//    (There are two parts to a UBX message, the class and the ID.)
// For coherency, you must determine which UBX message is last!
// This section *should* pick the correct last UBX message.

#if defined(UBLOX_PARSE_HNR_PVT)
  #define UBX_LAST_MSG_CLASS_IN_INTERVAL ublox::UBX_HNR
  #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_HNR_PVT
#else
  #define UBX_LAST_MSG_CLASS_IN_INTERVAL ublox::UBX_NAV

  #if defined(UBLOX_PARSE_VELNED)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_VELNED
  #elif defined(UBLOX_PARSE_DOP)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_DOP
  #elif defined(UBLOX_PARSE_POSLLH)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_POSLLH
  #elif defined(UBLOX_PARSE_STATUS)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_STATUS
  #elif defined(UBLOX_PARSE_PVT)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_PVT
  #elif defined(UBLOX_PARSE_SVINFO)
    #define UBX_LAST_MSG_ID_IN_INTERVAL    ublox::UBX_NAV_SVINFO
  #endif
#endif

#endif