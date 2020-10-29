#ifndef NMEAGPS_H
#define NMEAGPS_H

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

#include "CosaCompat.h"

#include <Arduino.h>
#ifdef __AVR__
  #include <avr/interrupt.h>
#endif

#include "GPSfix.h"
#include "NMEAGPS_cfg.h"

//------------------------------------------------------
//
// NMEA 0183 Parser for generic GPS Modules.
//
// As bytes are received from the device, they affect the
// internal FSM and set various members of the current /fix/.
// As multiple sentences are received, they are (optionally)
// merged into a single fix.  When the last sentence in a time
// interval (usually 1 second) is received, the fix is stored
// in the (optional) buffer of fixes.
//
// Only these NMEA messages are parsed:
//      GGA, GLL, GSA, GST, GSV, RMC, VTG, and ZDA.

class NMEAGPS
{
    NMEAGPS & operator =( const NMEAGPS & );
    NMEAGPS( const NMEAGPS & );

public:

    NMEAGPS();

    //.......................................................................
    // NMEA standard message types (aka "sentences")

    enum nmea_msg_t {
        NMEA_UNKNOWN,

        #if defined(NMEAGPS_PARSE_GGA) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_GGA,
        #endif

        #if defined(NMEAGPS_PARSE_GLL) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_GLL,
        #endif

        #if defined(NMEAGPS_PARSE_GSA) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_GSA,
        #endif

        #if defined(NMEAGPS_PARSE_GST) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_GST,
        #endif

        #if defined(NMEAGPS_PARSE_GSV) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_GSV,
        #endif

        #if defined(NMEAGPS_PARSE_RMC) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_RMC,
        #endif

        #if defined(NMEAGPS_PARSE_VTG) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_VTG,
        #endif

        #if defined(NMEAGPS_PARSE_ZDA) | defined(NMEAGPS_RECOGNIZE_ALL)
          NMEA_ZDA,
        #endif

        NMEAMSG_END // a bookend that tells how many enums there were
      };

    CONST_CLASS_DATA nmea_msg_t NMEA_FIRST_MSG = (nmea_msg_t) (NMEA_UNKNOWN+1);
    CONST_CLASS_DATA nmea_msg_t NMEA_LAST_MSG  = (nmea_msg_t) (NMEAMSG_END-1);

    //=======================================================================
    // FIX-ORIENTED methods: available, read, overrun and handle
    //=======================================================================
    // The typical sketch should have something like this in loop():
    //
    //    while (gps.available( Serial1 )) {
    //      gps_fix fix = gps.read();
    //      if (fix.valid.location) {
    //         ...
    //      }
    //    }

    //.......................................................................
    // The available(...) functions return the number of *fixes* that
    //   are available to be "read" from the fix buffer.  The GPS port
    //   object is passed in so a char can be read if port.available().

    uint8_t available( Stream & port )
      {
        if (processing_style == PS_POLLING)
          while (port.available())
            handle( port.read() );
        return _available();
      }
    uint8_t available() const volatile { return _available(); };

    //.......................................................................
    // Return the next available fix.  When no more fixes
    //   are available, it returns an empty fix.

    const gps_fix read();

    //.......................................................................
    //  The OVERRUN flag is set whenever a fix is not read by the time
    //  the next update interval starts.  You must clear it when you
    //  detect the condition.

    bool overrun() const { return _overrun; }
    void overrun( bool val ) { _overrun = val; }

    //.......................................................................
    // As characters are processed, they can be categorized as
    // INVALID (not part of this protocol), OK (accepted),
    // or COMPLETED (end-of-message).

    enum decode_t { DECODE_CHR_INVALID, DECODE_CHR_OK, DECODE_COMPLETED };

    //.......................................................................
    //  Process one character, possibly saving a buffered fix.
    //    It implements merging and coherency.
    //    This can be called from an ISR.

    decode_t handle( uint8_t c );

    //=======================================================================
    // CHARACTER-ORIENTED methods: decode, fix and is_safe
    //=======================================================================
    //
    //    *** MOST APPLICATIONS SHOULD USE THE FIX-ORIENTED METHODS ***
    //
    //    Using `decode` is only necessary if you want finer control
    //    on how fix information is filtered and merged.
    //
    // Process one character of an NMEA GPS sentence.  The internal state
    // machine tracks what part of the sentence has been received.  As the
    // sentence is received, members of the /fix/ structure are updated.
    // This character-oriented method *does not* buffer any fixes, and
    // /read()/ will always return an empty fix.
    //
    // @return DECODE_COMPLETED when a sentence has been completely received.

    NMEAGPS_VIRTUAL decode_t decode( char c );

    //.......................................................................
    //  Current fix accessor.
    //    *** MOST APPLICATIONS SHOULD USE read() TO GET THE CURRENT FIX  ***
    //    /fix/ will be constantly changing as characters are received.
    //
    //  For example, fix().longitude() may return nonsense data if
    //  characters for that field are currently being processed in /decode/.

    gps_fix & fix() { return m_fix; };

    //  NOTE: /is_safe/ *must* be checked before accessing members of /fix/.
    //  If you need access to the current /fix/ at any time, you should
    //  use the FIX-ORIENTED methods.

    //.......................................................................
    //  Determine whether the members of /fix/ are "currently" safe.
    //  It will return true when a complete sentence and the CRC characters
    //  have been received (or after a CR if no CRC is present).
    //  It will return false after a new sentence starts.

    bool is_safe() const volatile { return (rxState == NMEA_IDLE); }

    //  NOTE:  When INTERRUPT_PROCESSING is enabled, is_safe()
    //  and fix() could change at any time (i.e., they should be
    //  considered /volatile/).

    //=======================================================================
    // DATA MEMBER accessors and mutators
    //=======================================================================

    //.......................................................................
    //  Convert a nmea_msg_t to a PROGMEM string.
    //    Useful for printing the sentence type instead of a number.
    //    This can return "UNK" if the message is not a valid number.

    const __FlashStringHelper *string_for( nmea_msg_t msg ) const;

    //.......................................................................
    // Most recent NMEA sentence type received.

    nmea_msg_t nmeaMessage NEOGPS_BF(8);

    //.......................................................................
    //  Storage for Talker and Manufacturer IDs

    #ifdef NMEAGPS_SAVE_TALKER_ID
      char talker_id[2];
    #endif

    #ifdef NMEAGPS_SAVE_MFR_ID
      char mfr_id[3];
    #endif

    //.......................................................................
    //  Various parsing statistics

    #ifdef NMEAGPS_STATS
      struct statistics_t {
          uint32_t ok;     // count of successfully parsed sentences
          uint32_t errors; // NMEA checksum or other message errors
          uint32_t chars;
          void init()
            {
              ok     = 0L;
              errors = 0L;
              chars  = 0L;
            }
      } statistics;
    #endif

    //.......................................................................
    // SATELLITE VIEW array

    #ifdef NMEAGPS_PARSE_SATELLITES
      struct satellite_view_t
      {
        uint8_t    id;
        #ifdef NMEAGPS_PARSE_SATELLITE_INFO
          uint8_t  elevation; // 0..99 deg
          uint16_t azimuth;   // 0..359 deg
          uint8_t  snr     NEOGPS_BF(7); // 0..99 dBHz
          bool     tracked NEOGPS_BF(1);
        #endif
      } NEOGPS_PACKED;

      satellite_view_t satellites[ NMEAGPS_MAX_SATELLITES ];
      uint8_t          sat_count; // in the above array

      bool satellites_valid() const { return (sat_count >= m_fix.satellites); }
    #endif

    //.......................................................................
    // Reset the parsing process.
    //   This is used internally after a CS error, or could be used
    //   externally to abort processing if it has been too long
    //   since any data was received.

    void reset()
    {
      rxState = NMEA_IDLE;
    }

    //=======================================================================
    // CORRELATING Arduino micros() WITH UTC.
    //=======================================================================

    #if defined(NMEAGPS_TIMESTAMP_FROM_PPS) |  \
        defined(NMEAGPS_TIMESTAMP_FROM_INTERVAL)
      protected:
        uint32_t _UTCsecondStart;
        #if defined(NMEAGPS_TIMESTAMP_FROM_INTERVAL) & \
            ( defined(GPS_FIX_DATE) | defined(GPS_FIX_TIME) )
          uint32_t _IntervalStart; // quiet time just ended
        #endif
      public:

        // The micros() value when the current UTC second started
        uint32_t UTCsecondStart() const
          { lock();
              uint32_t ret = _UTCsecondStart;
            unlock();
            return ret;
          };
        void     UTCsecondStart( uint32_t us ) { _UTCsecondStart = us; };

        // The elapsed time since the start of the current UTC second
        uint32_t UTCus() const { return micros() - UTCsecondStart(); };
        uint32_t UTCms() const { return UTCus() / 1000UL; };

        // If you have attached a Pin Change interrupt routine to the PPS pin:
        //
        //     const int PPSpin = 5;
        //     void PPSisr() { gps.UTCsecondStart( micros() ); };
        //     void setup()
        //     {
        //       attachInterrupt( digitalPinToInterrupt(PPSpin), PPSisr, RISING );
        //     }
        //
        // If you are using an Input Capture pin, calculate the elapsed
        //   microseconds since the capture time (based on the TIMER
        //   frequency):
        //
        // void savePPSus() // called as an ISR or from a test in loop
        // {
        //    uint32_t elapsedUS = (currentCount - captureCount) * countUS;
        //    gps.UTCsecondStart( micros() - elapsedUS );
        // }
    #endif

    //=======================================================================
    // COMMUNICATING WITH THE GPS DEVICE: poll, send and send_P
    //=======================================================================

    //.......................................................................
    // Request the specified NMEA sentence.  Not all devices will respond.

    static void poll( Stream *device, nmea_msg_t msg );

    //.......................................................................
    // Send a message to the GPS device.
    // The '$' is optional, and the '*' and CS will be added automatically.

    static void send( Stream *device, const char *msg );
    static void send_P( Stream *device, const __FlashStringHelper *msg );

  #include "NMEAGPSprivate.h"

} NEOGPS_PACKED;

#endif
