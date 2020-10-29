#ifndef _UBXNMEA_H_
#define _UBXNMEA_H_

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

#include "NMEAGPS_cfg.h"

// Disable the entire file if derived types are not allowed.
#ifdef NMEAGPS_DERIVED_TYPES

#include "NMEAGPS.h"

#include "PUBX_cfg.h"

// Ublox proprietary messages do not have a message type.  These
// messages start with "$PUBX," which ends with the manufacturer ID.  The
// message type is actually specified by the first numeric field.  In order
// to parse these messages, /parse_mfr_ID/ must be overridden to set the
// /nmeaMessage/ to PUBX_00 during /parseCommand/.  When the first numeric
// field is completed by /parseField/, it may change /nmeamessage/ to one 
// of the other PUBX message types.

#if !defined(NMEAGPS_PARSE_PUBX_00) & !defined(NMEAGPS_PARSE_PUBX_04)

  // No ublox Proprietary messages defined, ignore rest of file.
  #define UBLOXGPS_BASE NMEAGPS
  #define UBLOXGPS_BASE_LAST_MSG NMEA_LAST_MSG
#else

  #define UBLOXGPS_BASE ubloxNMEA
  #define UBLOXGPS_BASE_LAST_MSG  PUBX_LAST_MSG

  #if !defined(NMEAGPS_PARSE_PROPRIETARY)
    #error NMEAGPS_PARSE_PROPRIETARY must be defined in NMEAGPS_cfg.h in order to parse PUBX messages!
  #endif

  #if !defined(NMEAGPS_PARSE_MFR_ID)
    #error NMEAGPS_PARSE_MFR_ID must be defined in NMEAGPS_cfg.h in order to parse PUBX messages!
  #endif

//=============================================================
// NMEA 0183 Parser for ublox Neo-6 GPS Modules.
//
// @section Limitations
// Very limited support for ublox proprietary NMEA messages.
// Only NMEA messages of types PUBX,00 and PUBX,04 are parsed.
//

class ubloxNMEA : public NMEAGPS
{
    ubloxNMEA( const ubloxNMEA & );

public:

    ubloxNMEA() {};

    /** ublox proprietary NMEA message types. */
    enum pubx_msg_t {
        PUBX_00 = NMEA_LAST_MSG+1,
        #if defined(NMEAGPS_PARSE_PUBX_04) | defined(NMEAGPS_RECOGNIZE_ALL)
          PUBX_04,
        #endif
        PUBX_END
    };
    static const nmea_msg_t PUBX_FIRST_MSG = (nmea_msg_t) PUBX_00;
    static const nmea_msg_t PUBX_LAST_MSG  = (nmea_msg_t) (PUBX_END-1);

protected:
    bool parseMfrID( char chr )
      { bool ok;
        switch (chrCount) {
          case  1: ok = (chr == 'U'); break;
          case  2: ok = (chr == 'B'); break;
          default: if (chr == 'X') {
                     ok = true;
                     nmeaMessage = (nmea_msg_t) PUBX_00;
                   } else
                     ok = false;
                   break;
        }
        return ok;
      };

    bool parsePUBX_00( char chr );
    bool parsePUBX_04( char chr );

    bool parseField( char chr );

    bool parseFix( char chr );
    bool parseVelocityDown( char chr );

} NEOGPS_PACKED;

#endif // at least one PUBX message enabled

#endif // NMEAGPS_DERIVED_TYPES enabled

#endif
