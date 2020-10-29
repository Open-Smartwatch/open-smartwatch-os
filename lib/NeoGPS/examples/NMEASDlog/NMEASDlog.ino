#include <Arduino.h>
#include <NMEAGPS.h>

//======================================================================
//  Program: NMEASDlog.ino
//
//  Description:  This program is an interrupt-driven GPS logger.
//    It uses the alternative serial port libraries NeoHWSerial,
//    NeoSWSerial, or NeoICSerial.
//
//  Prerequisites:
//     1) You have completed the requirements for NMEA_isr.ino
//     2) You have connected an SPI SD card and verified it is working
//        with other SD utilities.
//     3) For logging faster than the default 1Hz rate, you have
//        identified the required commands for your GPS device.
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

//#include <Streamers.h>

//----------------------------------------------------------------
// Check configuration

#if !defined( GPS_FIX_TIME ) || !defined( GPS_FIX_LOCATION )
  #error You must define TIME and LOCATION in GPSfix_cfg.h
#endif

#if !defined( NMEAGPS_PARSE_RMC )
  #error You must define NMEAGPS_PARSE_RMC in NMEAGPS_cfg.h!
#endif

#ifndef NMEAGPS_INTERRUPT_PROCESSING
  #error You must define NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h!
#endif

static const int LED = 13;

static NMEAGPS  gps; 

//----------------------------------------------------------------
// SD card includes and declarations

#include <SPI.h>
#include <SdFat.h>

SdFat SD;
const int chipSelect = 8;

//----------------------------------------------------------------
// For testing, it may be more convenient to simply print the
//   GPS fix fields to the Serial Monitor.  Simply uncomment
//   this define to skip all SD operations.  An SD card module
//   does not have to be connected.
#define SIMULATE_SD

#ifdef SIMULATE_SD

  auto &logfile = DEBUG_PORT;

#else

  File logfile;

#endif

//----------------------------------------------------------------
//  Utility to print a long integer like it's a float 
//    with 9 significant digits.

void printL( Print & outs, int32_t degE7 )
{
  // Extract and print negative sign
  if (degE7 < 0) {
    degE7 = -degE7;
    outs.print( '-' );
  }

  // Whole degrees
  int32_t deg = degE7 / 10000000L;
  outs.print( deg );
  outs.print( '.' );

  // Get fractional degrees
  degE7 -= deg*10000000L;

  // Print leading zeroes, if needed
  if (degE7 < 10L)
    outs.print( F("000000") );
  else if (degE7 < 100L)
    outs.print( F("00000") );
  else if (degE7 < 1000L)
    outs.print( F("0000") );
  else if (degE7 < 10000L)
    outs.print( F("000") );
  else if (degE7 < 100000L)
    outs.print( F("00") );
  else if (degE7 < 1000000L)
    outs.print( F("0") );
  
  // Print fractional degrees
  outs.print( degE7 );
}

//----------------------------------------------------------------
//  Because the SD write can take a long time, GPSisr will store 
//  parsed data in the NMEAGPS 'buffer' array until GPSloop can get to it.
//
//  The number of elements you should have in the array depends on
//  two things: the speed of your SD card, and the update rate you
//  have chosen.
//
//  For an update rate of 10Hz (100ms period), two fixes are probably
//  enough.  Most cards take ~100ms to complete a write of 512 bytes.
//  With FIX_MAX=2, two complete fixes can be stored, which were
//  received in 200ms.  A third fix can be started, giving a little
//  more time before an overrun occurs, a total of about 250ms.
//
//  If your card is slower or your update rate is faster, you should
//  first build and run this program to determine the speed of your
//  card.  The time it takes to log one record is printed to the log
//  file.
//
//  After the program has run for a minute or two, remove the
//  card and examine the loggingTimes.  You may see that an interval
//  was skipped, and you will also see an OVERRUN message on the
//  DEBUG_PORT (usually Serial).
//
//  You should calculate a new FIX_MAX from the maximum loggingTime 
//  you see in the log file:
//
//    FIX_MAX = (max loggingTime)/(update period) + 1;
//
//  For example, if the max loggingTime is 160ms, and the update period is
//  100ms (10Hz), then FIX_MAX = 160/100 + 1 = 2.
//
//  Change the FIX_MAX value, build and run the program again.  The
//  SD log file should now contain all records, and no OVERRUN
//  messages should have been printed on the DEBUG_PORT.
//
//  If you do see an OVERRUN message, examine the loggingTime to see
//  if it exceeded the maximum value from the previous build, and
//  increase FIX_MAX.
//
//  If you are also printing data to a Serial device, you could be
//  printing too much information.  In general, you must print less than
//  (baudrate/10) characters per second.  For example, if your baudrate 
//  is 9600, you must print less than 960 characters per second.  And if
//  the update rate is 10Hz, you must print no more than 96 characters
//  per update.
//
//  There are also restrictions on how much you should print to Serial in one 
//  section of code.  If you print more than 64 characters (the output buffer 
//  size), then some prints will block until all characters can be stored in the
//  output buffer.
//
//  For example, if you try to print 80 characters, the first 64 characters
//  will be immediately stored in the output buffer. However, the last 16 
//  characters must wait until the output buffer has room for 16 more 
//  characters.  That takes 16 * (10/baudrate) milliseconds.  At 9600 baud
//  that will take 17ms.  The loggingTimes will show no less than 17ms per
//  record, and will occasionally include the longer SD write time of ~100ms.

//----------------------------------------------------------------

static void GPSloop()
{
  if (gps.available()) {

    gps_fix fix = gps.read();

    // Log the fix information if we have a location and time
    
    if (fix.valid.location && fix.valid.time) {

      static uint16_t lastLoggingTime  = 0;
             uint16_t startLoggingTime = millis();

      // If you like the CSV format implemented in Streamers.h,
      //   you could replace all these prints with 
      // trace_all( logFile, fix ); // uncomment include Streamers.h

      printL( logfile, fix.latitudeL() );
      logfile.print( ',' );
      printL( logfile, fix.longitudeL() );
      logfile.print(',');

      if (fix.dateTime.hours < 10)
        logfile.print( '0' );
      logfile.print(fix.dateTime.hours);
      logfile.print( ':' );
      if (fix.dateTime.minutes < 10)
        logfile.print( '0' );
      logfile.print(fix.dateTime.minutes);
      logfile.print( ':' );
      if (fix.dateTime.seconds < 10)
        logfile.print( '0' );
      logfile.print(fix.dateTime.seconds);
      logfile.print( '.' );
      if (fix.dateTime_cs < 10)
         logfile.print( '0' ); // leading zero for .05, for example
      logfile.print(fix.dateTime_cs);
      logfile.print(',');

      logfile.print( lastLoggingTime ); // write how long the previous logging took
      logfile.println();

      // flush() is used to empty the contents of the SD buffer to the SD. 
      // If you don't call flush, the data will fill up the SdFat buffer 
      // of 512bytes and flush itself automatically.
      //
      // To avoid losing data or corrupting the SD card file system, you must 
      // call flush() at least once (or close()) before powering down or pulling 
      // the SD card.
      //
      // It is *strongly* recommended that you use some external event 
      // to close the file.  For example, staying within 50m of the moving
      // average location for 1 minute, or using a switch to start and stop 
      // logging.  It would also be good to provide a visual indication 
      // that it is safe to power down and/or remove the card,  perhaps via
      // the LED.
      //
      // To reduce the amount of data that may be lost by an abnormal shut down,
      // you can call flush() periodically.
      //
      // Depending on the amount of data you are printing, you can save 
      // *a lot* of CPU time by not flushing too frequently.  BTW, flushing
      // every time at 5Hz is too frequent.

      // This shows how to flush once a second.
      static uint16_t lastFlushTime = 0;

      if (startLoggingTime - lastFlushTime > 1000) {
        lastFlushTime = startLoggingTime; // close enough
        logfile.flush();
      }

      #ifdef SIMULATE_SD
        // Simulate the delay of writing to an SD card.  These times are
        //   very long.  This is intended to show (and test) the overrun detection.
        //
        // On a 1Hz GPS, delaying more than 2 seconds here, or more than
        //   2 seconds in two consecutive updates, will cause OVERRUN.
        //
        // Feel free to try different delays to simulate the actual behavior
        //   of your SD card.

        uint16_t t = random(0,5); // 0..4
        t += 3;                   // 3..7
        t = t*t*t*t;              // 81..2401ms
        delay( t ); // cause an OVERRUN
      #endif

      // All logging is finished, figure out how long that took.
      //   This will be written in the *next* record.    
      lastLoggingTime = (uint16_t) millis() - startLoggingTime;
    }
  }

} // GPSloop

//----------------------------------------------------------------

void GPSisr( uint8_t c )
{
  gps.handle( c );

} // GPSisr

//----------------------------------------------------------------
//  This routine waits for GPSisr to provide 
//  a fix that has a valid location.
//
//  The LED is slowly flashed while it's waiting.

static void waitForFix()
{
  DEBUG_PORT.print( F("Waiting for GPS fix...") );

  uint16_t lastToggle = millis();

  for (;;) {
    if (gps.available()) {
      if (gps.read().valid.location)
        break; // Got it!
    }

    // Slowly flash the LED until we get a fix
    if ((uint16_t) millis() - lastToggle > 500) {
      lastToggle += 500;
      digitalWrite( LED, !digitalRead(LED) );
      DEBUG_PORT.write( '.' );
    }
  }
  DEBUG_PORT.println();

  digitalWrite( LED, LOW );

  gps.overrun( false ); // we had to wait a while...

} // waitForFix

//----------------------------------------------------------------

void setup()
{
  DEBUG_PORT.begin(9600);
  while (!DEBUG_PORT)
    ; // wait for serial port to connect. 

  DEBUG_PORT.println( F("NMEASDlog.ino started!") );
  DEBUG_PORT.print( F("fix size = ") );
  DEBUG_PORT.println( sizeof(gps_fix) );
  DEBUG_PORT.print( NMEAGPS_FIX_MAX );
  DEBUG_PORT.println( F(" GPS updates can be buffered.") );
  
  if (gps.merging != NMEAGPS::EXPLICIT_MERGING)
    DEBUG_PORT.println( F("Warning: EXPLICIT_MERGING should be enabled for best results!") );

  gpsPort.attachInterrupt( GPSisr );
  gpsPort.begin( 9600 );

  //  Configure the GPS.  These are commands for MTK GPS devices.  Other
  //    brands will have different commands.
  gps.send_P( &gpsPort, F("PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0") ); // RMC only for MTK GPS devices
  gps.send_P( &gpsPort, F("PMTK220,100") ); // 10Hz update rate for MTK GPS devices

  // Enable the LED for blinking feedback
  pinMode( LED, OUTPUT );

  initSD();

  waitForFix();

} // setup

//----------------------------------------------------------------

void loop()
{
  GPSloop();

  if (gps.overrun()) {
    gps.overrun( false );
    DEBUG_PORT.println( F("DATA OVERRUN: fix data lost!") );
  }
}

//----------------------------------------------------------------

void initSD()
{
  #ifdef SIMULATE_SD

    DEBUG_PORT.println( F("  Simulating SD.") );
    
  #else

    DEBUG_PORT.println( F("Initializing SD card...") );

    // see if the card is present and can be initialized:
    if (!SD.begin(chipSelect)) {
      DEBUG_PORT.println( F("  SD card failed, or not present") );
      // don't do anything more:

      // Flicker the LED
      while (true) {
        digitalWrite(LED,HIGH);
        delay(75);
        digitalWrite(LED,LOW);
        delay(75);
      }
    }

    DEBUG_PORT.println( F("  SD card initialized.") );

    // Pick a numbered filename, 00 to 99.
    char filename[15] = "data_##.txt";

    for (uint8_t i=0; i<100; i++) {
      filename[5] = '0' + i/10;
      filename[6] = '0' + i%10;
      if (!SD.exists(filename)) {
        // Use this one!
        break;
      }
    }

    logfile = SD.open(filename, FILE_WRITE);
    if (!logfile) {
      DEBUG_PORT.print( F("Couldn't create ") ); 
      DEBUG_PORT.println(filename);

      // If the file can't be created for some reason this leaves the LED on
      //   so I know there is a problem
      digitalWrite(LED,HIGH);

      while (true) {}
    }

    DEBUG_PORT.print( F("Writing to ") ); 
    DEBUG_PORT.println(filename);

    // GPS Visualizer requires a header to identify the CSV fields.
    // If you are saving other data or don't need this, simply remove/change it
    logfile.println( F("latitude,longitude,time,loggingTime") ); 

    //trace_header( logfile ); // and uncomment #include Streamers.h
    
  #endif
} // initSD