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

#include "Garmin/GrmNMEA.h"

// Disable the entire file if derived types are not allowed,
//    *or* if no PUBX messages are enabled.
#if defined( NMEAGPS_DERIVED_TYPES) & \
    defined( GARMINGPS_PARSE_F )

#include "GPSTime.h"

//----------------------------------------------------------------

bool GarminNMEA::parseField( char chr )
{
  if (nmeaMessage >= (nmea_msg_t) PGRM_FIRST_MSG) {

    switch (nmeaMessage) {

      case PGRMF: return parseF( chr );

      default: 
        break;
    }

  } else

    // Delegate
    return NMEAGPS::parseField(chr);


  return true;

} // parseField

//----------------------------------------------------------------
// Garmin Proprietary NMEA Sentence strings (alphabetical)

#if defined(GARMINGPS_PARSE_F) | defined(NMEAGPS_RECOGNIZE_ALL)
  static const char garminF[] __PROGMEM =  "F";
#endif

static const char * const garmin_nmea[] __PROGMEM =
  {
    #if defined(GARMINGPS_PARSE_F) | defined(NMEAGPS_RECOGNIZE_ALL)
      garminF,
    #endif
  };

const NMEAGPS::msg_table_t GarminNMEA::garmin_msg_table __PROGMEM =
  {
    GarminNMEA::PGRM_FIRST_MSG,
    (const msg_table_t *) &NMEAGPS::nmea_msg_table,  //  <-- link to standard message table
    sizeof(garmin_nmea)/sizeof(garmin_nmea[0]),
    garmin_nmea
  };

//----------------------------------------------------------------

bool GarminNMEA::parseF( char chr )
{
  #ifdef GARMINGPS_PARSE_F
    switch (fieldIndex) {
        case 3: return parseDDMMYY( chr );
        case 4: return parseTime( chr );
        case 5: return parseLeapSeconds( chr );  // <--  you will write this
        PARSE_LOC(6);
        //case 10: return parseFix( char ); // not needed, because next field sets status
        case 11: return parseFix( chr );
        case 12: return parseSpeedKph( chr );
        case 13: return parseHeading( chr );
        case 14: return parsePDOP( chr );
        //case 15: return parseTDOP( chr ); // not yet supported
    }
  #endif

  return true;

} // parseF

//----------------------------------------------------------------

bool GarminNMEA::parseLeapSeconds( char chr )
{
  static uint8_t newLeapSeconds; // just used for parsing

  if (NMEAGPS::parseInt( newLeapSeconds, chr )) {
    GPSTime::leap_seconds = newLeapSeconds; // assign parsed value to global
  }

  return true;

} // parseGPSleapSeconds

#endif // DERIVED types and at least one PGRM message enabled
