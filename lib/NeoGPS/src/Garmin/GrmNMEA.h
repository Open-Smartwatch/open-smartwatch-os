#ifndef _GRMNMEA_H_
#define _GRMNMEA_H_

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

#include "Garmin/PGRM_cfg.h"

#if !defined(GARMINGPS_PARSE_F)

  // No Garmin Proprietary messages defined, ignore rest of file.

#else

  #if !defined(NMEAGPS_PARSE_PROPRIETARY)
    #error NMEAGPS_PARSE_PROPRIETARY must be defined in NMEAGPS_cfg.h in order to parse PGRM messages!
  #endif

  #if !defined(NMEAGPS_PARSE_MFR_ID)
    #error NMEAGPS_PARSE_MFR_ID must be defined in NMEAGPS_cfg.h in order to parse PGRM messages!
  #endif


//=============================================================
// NMEA 0183 Parser for Garmin GPS Modules.
//
// @section Limitations
// Very limited support for Garmin proprietary NMEA messages.
// Only NMEA messages of types F are parsed (i.e., $PGRMF).
//

class GarminNMEA : public NMEAGPS
{
    GarminNMEA( const GarminNMEA & );

public:

    GarminNMEA() {};

    /** Garmin proprietary NMEA message types. */
    enum grm_msg_t {
        PGRM_BEGIN = NMEA_LAST_MSG,
        #if defined(GARMINGPS_PARSE_F) | defined(NMEAGPS_RECOGNIZE_ALL)
          PGRMF,
        #endif
        PGRM_END
    };
    static const nmea_msg_t PGRM_FIRST_MSG = (nmea_msg_t) (PGRM_BEGIN+1);
    static const nmea_msg_t PGRM_LAST_MSG  = (nmea_msg_t) (PGRM_END  -1);

protected:
    bool parseField( char chr );

    bool parseMfrID( char chr )
      { bool ok;
        switch (chrCount) {
          case  1: ok = (chr == 'G'); break;
          case  2: ok = (chr == 'R'); break;
          case  3: ok = (chr == 'M'); break;
          default: ok = false;
        }
        return ok;
      };

    bool parseF( char chr );

    bool parseLeapSeconds( char chr );

    static const msg_table_t garmin_msg_table __PROGMEM;

    NMEAGPS_VIRTUAL const msg_table_t *msg_table() const
      { return &garmin_msg_table; };

} NEOGPS_PACKED;

#endif // at least one GRM message enabled

#endif // NMEAGPS_DERIVED_TYPES enabled

#endif