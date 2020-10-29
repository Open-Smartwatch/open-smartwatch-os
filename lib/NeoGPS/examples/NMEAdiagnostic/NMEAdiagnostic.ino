#include <NMEAGPS.h>

//======================================================================
//  Program: NMEAdiagnostic.ino
//
//  Description:  This program tries different baud rates until
//    valid NMEA sentences are detected.  Some GPS devices may
//    have a binary mode that does not emit NMEA sentences.  You
//    may have to send a special command or use a utility program
//    to configure it to emit NMEA sentences instead of binary messages.
//
//  Prerequisites:
//     1) Your GPS device has been correctly powered.
//          Be careful when connecting 3.3V devices.
//     2) Your GPS device is correctly connected to an Arduino serial port.
//          See GPSport.h for the default connections.
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  License:
//    Copyright (C) 2014-2017, SlashDevin
//
//    This file is part of NeoGPS
//
//    NeoGPS is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    NeoGPS is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with NeoGPS.  If not, see <http://www.gnu.org/licenses/>.
//
//======================================================================

#include <GPSport.h>
#include <Streamers.h>

// Check configuration

#ifndef NMEAGPS_RECOGNIZE_ALL
  #error You must define NMEAGPS_RECOGNIZE_ALL in NMEAGPS_cfg.h!
#endif

#ifdef NMEAGPS_IMPLICIT_MERGING
  #error You must *undefine* NMEAGPS_IMPLICIT_MERGING in NMEAGPS_cfg.h! \
   Please use EXPLICIT or NO_MERGING.
#endif

#ifdef NMEAGPS_INTERRUPT_PROCESSING
  #error You must *NOT* define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

static NMEAGPS  gps          ; // This parses received characters
static gps_fix  all_data     ; // A composite of all GPS data fields
static uint32_t last_rx = 0UL; // The last millis() time a character was
                               // received from GPS.
static uint32_t valid_sentence_received = 0UL;
static bool     last_sentence_received  = false;
static uint32_t baudStartTime = 0UL;
static uint8_t  warnings = 0;
static uint8_t  errors   = 0;

//--------------------------

static void hang()
{
  DEBUG_PORT.println( F("\n** NMEAdiagnostic completed **\n") );

  if (warnings) {
    DEBUG_PORT.print( warnings );
    DEBUG_PORT.print( F(" warnings") );
  }
  if (warnings && errors)
    DEBUG_PORT.print( F(" and ") );
  if (errors) {
    DEBUG_PORT.print( errors );
    DEBUG_PORT.print( F(" errors") );
  }
  if (warnings || errors)
    DEBUG_PORT.println();

  DEBUG_PORT.flush();

  for (;;)
    ;

} // hang

//--------------------------
// Baud rates to check

static long  baud_table[] =
  { 1200, 2400, 4800, 9600, 14400, 19200, 28800, 31250, 38400, 
    57600, 115200 };
static const uint8_t num_bauds          = sizeof(baud_table)/sizeof(baud_table[0]);
static const uint8_t INITIAL_BAUD_INDEX = 3; // 9600
static       uint8_t baud_index         = INITIAL_BAUD_INDEX;
static       bool    triedDifferentBaud = false;

//--------------------------

static void tryBaud()
{
  long baud = baud_table[baud_index];
  DEBUG_PORT.print( F("\n____________________________\n\nChecking ") );
  DEBUG_PORT.print( baud );
  DEBUG_PORT.print( F(" baud...\n") );
  DEBUG_PORT.flush();

//if (baud == 9600) baud = 17000;
  gpsPort.begin( baud );
  baudStartTime = millis();

} // tryBaud

//--------------------------

static void tryAnotherBaudRate()
{
  gpsPort.end();
  while (gpsPort.available())
    gpsPort.read();

  if (baud_index == INITIAL_BAUD_INDEX) {
    baud_index = 0;

  } else {
    baud_index++;
    if (baud_index == INITIAL_BAUD_INDEX)
      baud_index++; // skip it, we already tried it

    if (baud_index >= num_bauds) {
      baud_index = INITIAL_BAUD_INDEX;
      DEBUG_PORT.print( F("\n  All baud rates tried!\n") );
      hang();
    }
  }

  tryBaud();

  triedDifferentBaud = true;

} // tryAnotherBaudRate

//------------------------------------

static const uint16_t MAX_SAMPLE = 256;
static uint8_t        someChars[ MAX_SAMPLE ];
static uint16_t       someCharsIndex = 0;

static void dumpSomeChars()
{
  if (someCharsIndex > 0) {
    DEBUG_PORT.print( F("Received data:\n") );

    const uint16_t  bytes_per_line = 32;
          char      ascii[ bytes_per_line ];
          uint8_t  *ptr            = &someChars[0];

    for (uint16_t i=0; i<someCharsIndex; ) {
      uint16_t j;
      
      for (j=0; (i<someCharsIndex) && (j<bytes_per_line); i++, j++) {
        uint8_t c = *ptr++;
        if (c < 0x10)
          DEBUG_PORT.print('0');
        DEBUG_PORT.print( c, HEX );
        if ((' ' <= c) && (c <= '~'))
          ascii[ j ] = c;
        else
          ascii[ j ] = '.';
      }

      uint16_t jmax = j;
      while (j++ < bytes_per_line)
        DEBUG_PORT.print( F("  ") );
      DEBUG_PORT.print( ' ' );
      
      for (j=0; j<jmax; j++)
        DEBUG_PORT.print( ascii[ j ] );
      DEBUG_PORT.print( '\n' );
    }
    DEBUG_PORT.flush();

    someCharsIndex = 0;
  }
} // dumpSomeChars

//----------------------------------------------------------------

void displaySentences()
{
  // We received one or more sentences, display the baud rate
  DEBUG_PORT.print( F("\n\n**** NMEA sentence(s) detected!  ****\n") );
  dumpSomeChars();
  DEBUG_PORT << F("\nDevice baud rate is ") <<
    baud_table[ baud_index ] << '\n';

  DEBUG_PORT.print( F("\nGPS data fields received:\n\n  ") );
  trace_header( DEBUG_PORT );
  DEBUG_PORT.print( F("  ") );
  trace_all( DEBUG_PORT, gps, all_data );

  if (!last_sentence_received) {
    warnings++;
    DEBUG_PORT.print( F("\nWarning: LAST_SENTENCE_IN_INTERVAL defined to be ") );
    DEBUG_PORT.print( gps.string_for( LAST_SENTENCE_IN_INTERVAL ) );
    DEBUG_PORT.println( F(", but was never received.\n"
                      "  Please use NMEAorder.ino to determine which sentences your GPS device sends, and then\n"
                      "  use the last one for the definition in NMEAGPS_cfg.h.") );
  }

} // displaySentences

//----------------------------------------------------------------
//  Listen to see if the GPS device is correctly 
//  connected and functioning.

static void listenForSomething()
{
  uint32_t current_ms         = millis();
  uint32_t ms_since_last_rx   = current_ms - last_rx;
  bool     waited_long_enough = (current_ms - baudStartTime) > 1000UL;

  if ((ms_since_last_rx > 5) && waited_long_enough) {

    // The GPS device has not sent any characters for at least 5ms.
    //   See if we've been getting chars sometime during the last second.
    //   If not, the GPS may not be working or connected properly.

    bool getting_chars = (someCharsIndex > 0);

    // Try to diagnose the problem
    
    static uint8_t tries = 1;
    bool           tryNext = false;

    if (!getting_chars) {

      if (tries++ >= 3) {
        errors++;
        DEBUG_PORT.println( F("\nCheck GPS device and/or connections.  No data received.\n") );
        tryNext = true;
      }

    } else if (valid_sentence_received) {
      uint8_t s  = valid_sentence_received/1000;
      uint8_t ms = valid_sentence_received - s*1000;

      DEBUG_PORT.print( F("Valid sentences were received ") );
      DEBUG_PORT.print( s );
      DEBUG_PORT.print( '.' );
      if (ms < 100)
        DEBUG_PORT.print( '0' );
      if (ms < 10)
        DEBUG_PORT.print( '0' );
      DEBUG_PORT.print( ms );
      DEBUG_PORT.println(
        F(" seconds ago.\n"
          "  The GPS update rate may be lower than 1Hz,\n"
          "  or the connections may be bad." ) );
      displaySentences();
      hang();

    } else {
        DEBUG_PORT.println(
          F("No valid sentences, but characters are being received.\n"
            "  Check baud rate or device protocol configuration.\n" ) );

      dumpSomeChars();
      delay( 2000 );
      tryNext = true;
    }

    if (tryNext) {
      tries = 1;
      tryAnotherBaudRate();
      valid_sentence_received = 0UL;
    }
  }
  
} // listenForSomething

//------------------------------------

static void GPSloop()
{
  while (gpsPort.available()) {
    last_rx = millis();

    uint8_t c = gpsPort.read();

    if (someCharsIndex < MAX_SAMPLE)
      someChars[ someCharsIndex++ ] = c;

    if (gps.decode( c ) == NMEAGPS::DECODE_COMPLETED) {
      all_data |= gps.fix();
      valid_sentence_received = last_rx;

      if (gps.nmeaMessage == LAST_SENTENCE_IN_INTERVAL)
        last_sentence_received = true;

      DEBUG_PORT.print( F("Received ") );
      DEBUG_PORT.println( gps.string_for( gps.nmeaMessage ) );

      static uint8_t sentences_printed = 0;
             bool    long_enough       = (millis() - baudStartTime > 3000);

      if (long_enough ||
          (
            (sentences_printed++ >= 20) &&
            (someCharsIndex      >= MAX_SAMPLE)
          ) ) {
        displaySentences();
        hang();
      }
    }
  }

  if (!valid_sentence_received ||
      (millis() - valid_sentence_received > 3000UL))
    listenForSomething();

} // GPSloop

//--------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ;

  DEBUG_PORT.print( F("NMEAdiagnostic.INO: started\n") );
  DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );

  if (sizeof(gps_fix) <= 2) {
    warnings++;
    DEBUG_PORT.print( F("\nWarning: no fields are enabled in GPSfix_cfg.h.\n  Only the following information will be displayed:\n    ") );
    trace_header( DEBUG_PORT );
  }

  #if !defined( NMEAGPS_PARSE_GGA ) & !defined( NMEAGPS_PARSE_GLL ) & \
      !defined( NMEAGPS_PARSE_GSA ) & !defined( NMEAGPS_PARSE_GSV ) & \
      !defined( NMEAGPS_PARSE_RMC ) & !defined( NMEAGPS_PARSE_VTG ) & \
      !defined( NMEAGPS_PARSE_ZDA ) & !defined( NMEAGPS_PARSE_GST )
    warnings++;
    DEBUG_PORT.println( F("\nWarning: no messages are enabled for parsing in NMEAGPS_cfg.h.\n  No fields will be valid, including the 'status' field.") );
  #endif

  DEBUG_PORT.flush();

  tryBaud();

} // setup

//--------------------------

void loop()
{
  GPSloop();
}
