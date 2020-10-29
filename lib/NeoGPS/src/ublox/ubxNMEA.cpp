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

#include "ublox/ubxNMEA.h"

// Disable the entire file if derived types are not allowed,
//    *or* if no PUBX messages are enabled.
#if defined( NMEAGPS_DERIVED_TYPES) & \
    (defined(NMEAGPS_PARSE_PUBX_00) | defined(NMEAGPS_PARSE_PUBX_04))

//---------------------------------------------

bool ubloxNMEA::parseField(char chr)
{
  if (nmeaMessage >= (nmea_msg_t) PUBX_FIRST_MSG) {

    switch (nmeaMessage) {

      case PUBX_00: return parsePUBX_00( chr );

      #if defined(NMEAGPS_PARSE_PUBX_04) | defined(NMEAGPS_RECOGNIZE_ALL)
        case PUBX_04: return parsePUBX_04( chr );
      #endif

      default: 
        break;
    }

  } else

    // Delegate
    return NMEAGPS::parseField(chr);


  return true;

} // parseField

//----------------------------

bool ubloxNMEA::parsePUBX_00( char chr )
{
  bool ok = true;

  switch (fieldIndex) {
    case 1:
      // The first field is actually a message subtype
      if (chrCount == 0)
        ok = (chr == '0');

      else if (chrCount == 1) {
        switch (chr) {
          #if defined(NMEAGPS_PARSE_PUBX_00) | defined(NMEAGPS_RECOGNIZE_ALL)
            case '0': break;
          #endif
          #if defined(NMEAGPS_PARSE_PUBX_04) | defined(NMEAGPS_RECOGNIZE_ALL)
            case '4': nmeaMessage = (nmea_msg_t) PUBX_04; break;
          #endif
          default : ok = false;
        }

      } else // chrCount > 1
        ok = (chr == ',');
      break;

    #ifdef NMEAGPS_PARSE_PUBX_00
      case  2: return parseTime( chr );
      PARSE_LOC(3);
      case  7: return parseAlt( chr );
      case  8: return parseFix( chr );
      case  9: // use Horizontal accuracy for both lat and lon errors
        #if defined(GPS_FIX_LAT_ERR)
          ok = parse_lat_err( chr );
          #if defined(GPS_FIX_LON_ERR)
            // When the lat_err field is finished,
            //   copy it to the lon_err field.
            if (chr == ',') {
              m_fix.valid.lon_err = m_fix.valid.lat_err;
              if (m_fix.valid.lon_err)
                m_fix.lon_err_cm = m_fix.lat_err_cm;
            }
          #endif

        #elif defined(GPS_FIX_LON_ERR)
          ok = parse_lon_err( chr );
        #endif
        break;
      case 10: return parse_alt_err( chr ); // vertical accuracy
      case 11:
        #ifdef GPS_FIX_SPEED
          ok = parseSpeed( chr ); // PUBX,00 provides speed in km/h!

          if ((chr == ',') && m_fix.valid.speed) {
            uint32_t kph    = m_fix.spd.int32_000();
            uint32_t nmiph  = (kph * 1000) / gps_fix::M_PER_NMI;
            m_fix.spd.whole = nmiph / 1000;
            m_fix.spd.frac  = (nmiph - m_fix.spd.whole*1000);
            // Convert to Nautical Miles/Hour
          }
        #endif
        break;
      case 12: return parseHeading( chr );
      case 13: return parseVelocityDown( chr );
      case 15: return parseHDOP( chr );
      case 16: return parseVDOP( chr );
      case 18: return parseSatellites( chr );
    #endif
  }

  return ok;

} // parsePUBX_00

//---------------------------------------------

bool ubloxNMEA::parsePUBX_04( char chr )
{
  #ifdef NMEAGPS_PARSE_PUBX_04
    switch (fieldIndex) {
        case 2: return parseTime( chr );
        case 3: return parseDDMMYY( chr );
    }
  #endif

  return true;

} // parsePUBX_04

//---------------------------------------------

bool ubloxNMEA::parseFix( char chr )
{
  if (chrCount == 0) {
    NMEAGPS_INVALIDATE( status );
    if (chr == 'N')
      m_fix.status = gps_fix::STATUS_NONE;
    else if (chr == 'T')
      m_fix.status = gps_fix::STATUS_TIME_ONLY;
    else if (chr == 'R')
      m_fix.status = gps_fix::STATUS_EST;
    else if (chr == 'G')
      m_fix.status = gps_fix::STATUS_STD;
    else if (chr == 'D')
      m_fix.status = gps_fix::STATUS_DGPS;

  } else if (chrCount == 1) {

    if (((chr == 'T') && (m_fix.status == gps_fix::STATUS_TIME_ONLY)) ||
        ((chr == 'K') && (m_fix.status == gps_fix::STATUS_EST)) ||
        (((chr == '2') || (chr == '3')) &&
         ((m_fix.status == gps_fix::STATUS_STD) ||
          (m_fix.status == gps_fix::STATUS_DGPS))) ||
        ((chr == 'F') && (m_fix.status == gps_fix::STATUS_NONE)))
      // status based on first char was ok guess
      m_fix.valid.status = true;

    else if ((chr == 'R') && (m_fix.status == gps_fix::STATUS_DGPS)) {
      m_fix.status = gps_fix::STATUS_EST; // oops, was DR instead
      m_fix.valid.status = true;
    }
  }
  
  return true;
}

//---------------------------------------------

bool ubloxNMEA::parseVelocityDown( char chr )
{
  #ifdef GPS_FIX_VELNED
    if (chrCount == 0)
      NMEAGPS_INVALIDATE( velned );

    // Checks for alias size.
    char test[ (int)sizeof(m_fix.velocity_down) - (int)sizeof(gps_fix::whole_frac) ];

    gps_fix::whole_frac *temp = (gps_fix::whole_frac *) &m_fix.velocity_down; // an alias for parsing

    if (parseFloat( *temp, chr, 3 )) { // 0.001 m/s

      if (chr == ',') {
        // convert the temporary whole_frac values in place
        m_fix.valid.velned = (chrCount > 0);
        if (m_fix.valid.velned) {
          m_fix.velocity_down = (temp->int32_000() + 5) / 10L; // mm/s to cm/s
        }
      }
    }
  #endif
  
  return true;

} // parseVelocityDown

#endif // DERIVED types and at least one PUBX message enabled
