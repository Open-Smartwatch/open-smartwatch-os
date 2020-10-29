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

  protected:
    //.......................................................................
    // Table entry for NMEA sentence type string and its offset
    // in enumerated nmea_msg_t.  Proprietary sentences can be implemented
    // in derived classes by adding a second table.  Additional tables
    // can be singly-linked through the /previous/ member.  The instantiated
    // class's table is the head, and should be returned by the derived
    // /msg_table/ function.  Tables should be sorted alphabetically.

    struct msg_table_t {
      uint8_t             offset;  // nmea_msg_t enum starting value
      const msg_table_t  *previous;
      uint8_t             size;    // number of entries in table array
      const char * const *table;   // array of NMEA sentence strings
    };

    static const msg_table_t  nmea_msg_table __PROGMEM;

    NMEAGPS_VIRTUAL const msg_table_t *msg_table() const
      { return &nmea_msg_table; };

    //.......................................................................
    //  These virtual methods can accept or reject
    //    the talker ID (for standard sentences) or
    //    the manufacturer ID (for proprietary sentences).
    //  The default is to accept *all* IDs.
    //  Override them if you want to reject certain IDs, or you want
    //    to handle COMPLETED sentences from certain IDs differently.

    #ifdef NMEAGPS_PARSE_TALKER_ID
      NMEAGPS_VIRTUAL bool parseTalkerID( char ) { return true; };
    #endif

    #ifdef NMEAGPS_PARSE_PROPRIETARY
      #ifdef NMEAGPS_PARSE_MFR_ID
        NMEAGPS_VIRTUAL bool parseMfrID( char ) { return true; };
      #endif
    #endif

  public:
    //.......................................................................
    // Set all parsed data to initial values.

    void data_init()
    {
      fix().init();

      #ifdef NMEAGPS_PARSE_SATELLITES
        sat_count = 0;
      #endif
    }

    //.......................................................................

    enum merging_t { NO_MERGING, EXPLICIT_MERGING, IMPLICIT_MERGING };
    static const merging_t
      merging = NMEAGPS_MERGING; // see NMEAGPS_cfg.h

    enum processing_style_t { PS_POLLING, PS_INTERRUPT };
    static const processing_style_t
      processing_style = NMEAGPS_PROCESSING_STYLE;  // see NMEAGPS_cfg.h

    static const bool keepNewestFixes = NMEAGPS_KEEP_NEWEST_FIXES;

    static const bool validateChars () { return NMEAGPS_VALIDATE_CHARS; }
    static const bool validateFields() { return NMEAGPS_VALIDATE_FIELDS; }

    //.......................................................................
    //  Control access to this object.  This preserves atomicity when
    //     the processing style is interrupt-driven.

    void lock() const
      {
        if (processing_style == PS_INTERRUPT)
          noInterrupts();
      }

    void unlock() const
      {
        if (processing_style == PS_INTERRUPT)
          interrupts();
      }

  protected:
    //=======================================================================
    //   PARSING FINITE-STATE MACHINE
    //=======================================================================

    //  Current fix
    gps_fix m_fix;

    // Current parser state
    uint8_t      crc;            // accumulated CRC in the sentence
    uint8_t      fieldIndex;     // index of current field in the sentence
    uint8_t      chrCount;       // index of current character in current field
    uint8_t      decimal;        // digits received after the decimal point
    struct {
      bool     negative          NEOGPS_BF(1); // field had a leading '-'
      bool     _comma_needed     NEOGPS_BF(1); // field needs a comma to finish parsing
      bool     group_valid       NEOGPS_BF(1); // multi-field group valid
      bool     _overrun          NEOGPS_BF(1); // an entire fix was dropped
      bool     _intervalComplete NEOGPS_BF(1); // automatically set after LAST received
      #if (NMEAGPS_FIX_MAX == 0)
        bool   _fixesAvailable   NEOGPS_BF(1);
      #endif
      #ifdef NMEAGPS_PARSE_PROPRIETARY
        bool   proprietary       NEOGPS_BF(1); // receiving proprietary message
      #endif
    } NEOGPS_PACKED;

    #ifdef NMEAGPS_PARSING_SCRATCHPAD
      union {
        uint32_t U4;
        uint16_t U2[2];
        uint8_t  U1[4];
      } scratchpad;
    #endif

    bool comma_needed()
    {
      #ifdef NMEAGPS_COMMA_NEEDED
        return _comma_needed;
      #else
        return false;
      #endif
    }

    void comma_needed( bool value )
    {
      #ifdef NMEAGPS_COMMA_NEEDED
        _comma_needed = value;
      #endif
    }

    // Internal FSM states
    enum rxState_t {
        NMEA_IDLE,             // Waiting for initial '$'
        NMEA_RECEIVING_HEADER, // Parsing sentence type field
        NMEA_RECEIVING_DATA,   // Parsing fields up to the terminating '*'
        NMEA_RECEIVING_CRC     // Receiving two-byte transmitted CRC
    };
    CONST_CLASS_DATA uint8_t NMEA_FIRST_STATE = NMEA_IDLE;
    CONST_CLASS_DATA uint8_t NMEA_LAST_STATE  = NMEA_RECEIVING_CRC;

    rxState_t rxState NEOGPS_BF(8);

    //.......................................................................

    uint8_t _available() const volatile { return _fixesAvailable; };

    //.......................................................................
    //  Buffered fixes.

    #if (NMEAGPS_FIX_MAX > 0)
      gps_fix buffer[ NMEAGPS_FIX_MAX ]; // could be empty, see NMEAGPS_cfg.h
      uint8_t _fixesAvailable;
      uint8_t _firstFix;
      uint8_t _currentFix;
    #endif

    //.......................................................................
    // Indicate that the next sentence should initialize the internal data.
    //    This is useful for coherency or custom filtering.

    bool intervalComplete() const { return _intervalComplete; }
    void intervalComplete( bool val ) { _intervalComplete = val; }

    //.......................................................................
    //  Identify when an update interval is completed, according to the
    //  most recently-received sentence.  In this base class, it just
    //  looks at the nmeaMessage member.  Derived classes may have
    //  more complex, specific conditions.

    NMEAGPS_VIRTUAL bool intervalCompleted() const
      { return (nmeaMessage == LAST_SENTENCE_IN_INTERVAL); }
                               // see NMEAGPS_cfg.h

    //.......................................................................
    //  When a fix has been fully assembled from a batch of sentences, as
    //  determined by the configured merging technique and ending with the
    //  LAST_SENTENCE_IN_INTERVAL, it is stored in the (optional) buffer
    //  of fixes.  They are removed with /read()/.

    void storeFix();

    //=======================================================================
    //   PARSING METHODS
    //=======================================================================

    //.......................................................................
    // Try to recognize an NMEA sentence type, after the IDs have been accepted.

    decode_t parseCommand( char c );
    decode_t parseCommand( const msg_table_t *msgs, uint8_t cmdCount, char c );

    //.......................................................................
    // Parse various NMEA sentences

    bool parseGGA( char chr );
    bool parseGLL( char chr );
    bool parseGSA( char chr );
    bool parseGST( char chr );
    bool parseGSV( char chr );
    bool parseRMC( char chr );
    bool parseVTG( char chr );
    bool parseZDA( char chr );

    //.......................................................................
    // Depending on the NMEA sentence type, parse one field of an expected type.

    NMEAGPS_VIRTUAL bool parseField( char chr );

    //.......................................................................
    // Parse the primary NMEA field types into /fix/ members.

    bool parseFix        ( char chr ); // aka STATUS or MODE
    bool parseTime       ( char chr );
    bool parseDDMMYY     ( char chr );
    bool parseLat        ( char chr );
    bool parseNS         ( char chr );
    bool parseLon        ( char chr );
    bool parseEW         ( char chr );
    bool parseSpeed      ( char chr );
    bool parseSpeedKph   ( char chr );
    bool parseHeading    ( char chr );
    bool parseAlt        ( char chr );
    bool parseGeoidHeight( char chr );
    bool parseHDOP       ( char chr );
    bool parseVDOP       ( char chr );
    bool parsePDOP       ( char chr );
    bool parse_lat_err   ( char chr );
    bool parse_lon_err   ( char chr );
    bool parse_alt_err   ( char chr );
    bool parseSatellites ( char chr );

    // Helper macro for parsing the 4 consecutive fields of a location
    #define PARSE_LOC(i) case i: return parseLat( chr );\
      case i+1: return parseNS ( chr ); \
      case i+2: return parseLon( chr ); \
      case i+3: return parseEW ( chr );

    //.......................................................................
    // Parse floating-point numbers into a /whole_frac/
    // @return true when the value is fully populated.

    bool parseFloat( gps_fix::whole_frac & val, char chr, uint8_t max_decimal );

    //.......................................................................
    // Parse floating-point numbers into a uint16_t
    // @return true when the value is fully populated.

    bool parseFloat( uint16_t & val, char chr, uint8_t max_decimal );

    //.......................................................................
    // Parse NMEA lat/lon dddmm.mmmm degrees

    bool parseDDDMM
      (
        #if defined( GPS_FIX_LOCATION )
          int32_t & val,
        #endif
        #if defined( GPS_FIX_LOCATION_DMS )
          DMS_t & dms,
        #endif
        char chr
      );

    //.......................................................................
    // Parse integer into 8-bit int
    // @return true when non-empty value

    bool parseInt( uint8_t &val, uint8_t chr )
    {
      negative = false;
      bool is_comma = (chr == ',');

      if (chrCount == 0) {
        if (is_comma)
          return false; // empty field!

        if (((validateChars() || validateFields()) && (chr == '-')) ||
            (validateChars() && !isdigit( chr )))
          sentenceInvalid();
        else
          val = (chr - '0');

      } else if (!is_comma) {

        if (validateChars() && !isdigit( chr ))
          sentenceInvalid();
        else
          val = (val*10) + (chr - '0');
      }
      return true;
    }

    //.......................................................................
    // Parse integer into signed 8-bit int
    // @return true when non-empty value

    bool parseInt( int8_t &val, uint8_t chr )
    {
      bool is_comma = (chr == ',');

      if (chrCount == 0) {
        if (is_comma)
          return false; // empty field!

        negative = (chr == '-');
        if (negative) {
          comma_needed( true ); // to negate
          val = 0;
        } else if (validateChars() && !isdigit( chr )) {
          sentenceInvalid();
        } else {
          val = (chr - '0');
        }
      } else if (!is_comma) {
        val = (val*10) + (chr - '0');

      } else if (negative) {
        val = -val;
      }

      return true;
    }

    //.......................................................................
    // Parse integer into 16-bit int
    // @return true when non-empty value

    bool parseInt( uint16_t &val, uint8_t chr )
    {
      negative = false;

      bool is_comma = (chr == ',');
      if (chrCount == 0) {
        if (is_comma)
          return false; // empty field!

        if (((validateChars() || validateFields()) && (chr == '-')) ||
            (validateChars() && !isdigit( chr )))
          sentenceInvalid();
        else
          val = (chr - '0');

      } else if (!is_comma) {

        if (validateChars() && !isdigit( chr ))
          sentenceInvalid();
        else
          val = (val*10) + (chr - '0');
      }
      return true;
    }

    //.......................................................................
    // Parse integer into 32-bit int
    // @return true when non-empty value

    bool parseInt( uint32_t &val, uint8_t chr )
    {
      negative = false;

      bool is_comma = (chr == ',');
      if (chrCount == 0) {
        if (is_comma)
          return false; // empty field!

        if (((validateChars() || validateFields()) && (chr == '-')) ||
            (validateChars() && !isdigit( chr )))
          sentenceInvalid();
        else
          val = (chr - '0');

      } else if (!is_comma) {

        if (validateChars() && !isdigit( chr ))
          sentenceInvalid();
        else
          val = (val*10) + (chr - '0');
      }
      return true;
    }

  private:
    void sentenceBegin       ();
    void sentenceOk          ();
    void sentenceInvalid     ();
    void sentenceUnrecognized();
    void headerReceived      ();

