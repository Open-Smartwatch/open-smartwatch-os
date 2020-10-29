#include <NMEAGPS.h>

//======================================================================
//  Program: NMEArevGeoCache.ino
//
//  Description:  Activates a servo when the current location is
//                      close enough to the target location.
//
//  Prerequisites:
//     1) NMEA.ino works with your device (correct TX/RX pins and baud rate)
//     2) The RMC sentence has been enabled.
//     3) Your device sends an RMC sentence (e.g., $GPRMC).
//
//  Additional Hardware examples:
//     16x2 Character LCD: https://www.adafruit.com/products/181)
//     Servo             : TPro Micro SG90 https://www.adafruit.com/products/169
//
//  'Serial' is for debug output to the Serial Monitor window.
//
//  Credits:
//    This is simplified version of bnordlund9's Geobox: 
//        http://www.youtube.com/watch?v=g0060tcuofg
//  
//    Engagement Box by Kenton Harris 11/12/2012
//  
//    Reverse Geocache idea by Mikal Hart of http://arduiniana.org/
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

//------------------------------------------------------------
// Check that the config files are set up properly

#if !defined( NMEAGPS_PARSE_RMC ) &  \
    !defined( NMEAGPS_PARSE_GGA ) &  \
    !defined( NMEAGPS_PARSE_GLL )
  #error You must uncomment at least one of NMEAGPS_PARSE_RMC, NMEAGPS_PARSE_GGA or NMEAGPS_PARSE_GLL in NMEAGPS_cfg.h!
#endif

#if !defined( GPS_FIX_LOCATION )
  #error You must uncomment GPS_FIX_LOCATION in GPSfix_cfg.h!
#endif

//------------------------------------------------------------
//  Additional Hardware includes
#include <LiquidCrystal.h>
//#include <NewLiquidCrystal_emul.h>

#include <PWMServo.h>
PWMServo   servoLatch;
const int  servoLock   = 180;                  // angle (deg) of "locked" servo
const int  servoUnlock = 105;                  // angle (deg) of "unlocked" servo

using namespace NeoGPS;
NMEAGPS gps;

      float range;                // current distance from HERE to THERE
const float CLOSE_ENOUGH = 300.0; // miles, change for your needs

LiquidCrystal lcd(7, 8, 6, 10, 11, 12);

const int  fixLEDPin = 4;
const int  servoPin  = 9;

// The target location, in degrees * 10,000,000
Location_t there( 384602500,  -1098191667L );
               // 38°27'36.2"N 109°49'15.4"W

void setup()
{
  servoLatch.attach(SERVO_PIN_A);
  servoLatch.write(servoLock);
  delay(50);

  lcd.begin(16, 2);

  Serial.begin(9600);
  Serial.println( F("Debug GPS Test:") );

  gpsPort.begin(9600);

  // Configure GPS (these are Adafruit GPS commands - your brand may be different)
  gpsPort.print
    ( F( "$PMTK314,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\r\n" // RMC only...
         "$PMTK220,1000*1F\r\n" ) );    // ...and 1 Hz update rate

} // setup

void loop()
{
  static uint8_t  warningState = 0;
  static uint32_t lastFixTime, lastDotTime;

  while (gps.available( gpsPort )) {
    gps_fix fix = gps.read(); // save the latest

    // Set the "fix" LED to on or off
    bool gpsWasFixed = fix.valid.status && (fix.status >= gps_fix::STATUS_STD);
    digitalWrite( fixLEDPin, gpsWasFixed );

    // When we have a location, calculate how far away we are from "there".
    if (fix.valid.location) {
      lastFixTime = millis();

      range = fix.location.DistanceMiles( there );

      //for GPS debug
      DEBUG_PORT.print( F("Here: ") );
      //printDMS( DEBUG_PORT, fix.location );
      printAsFloat( DEBUG_PORT, fix.location );

      DEBUG_PORT.print( F(" There: ") );
      //printDMS( DEBUG_PORT, there );
      printAsFloat( DEBUG_PORT, there );

      DEBUG_PORT.print( F("Range: ") );
      DEBUG_PORT.print( range );
      DEBUG_PORT.println( F(" Miles") );

      // Are we there?
      if (range < CLOSE_ENOUGH) {
        lcd.setCursor(0,1);
        lcd.println( F("Box Unlocking...") );
        delay(500);
        servoLatch.write(servoUnlock);

        lcd.clear();
        lcd.setCursor(0,1);
        lcd.print( F("Happy Birthday") );

        for (;;); // hang here
      }

      // Nope, just give a little feedback...
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print( F("Signal Locked") );
      lcd.setCursor(0,1);
      lcd.print( range );
      lcd.print( F(" Miles") );

      warningState = 0; // restart in case we lose the fix later
    }
  }

  // Display warnings when the GPS hasn't had a fix for a while

  if (millis() - lastFixTime > 2000UL) {

    if (warningState == 0) {

      // First, show the warning message...
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print( F("Acquiring Signal") );

      warningState = 1;

    } else if (warningState < 10) {

      // ...then some dots, two per second...
      if (millis() - lastDotTime > 500UL ) {
        lastDotTime = millis();
        lcd.setCursor( warningState-1, 1 );
        lcd.print( '.' );

        warningState++;
      }

    } else if (warningState == 10) {
      // ... then tell them what to do.
      lcd.setCursor(0,1);
      lcd.println( F("Take box outside") );

      // Don't display anything else until location is valid
      warningState++; // 11
    }
  }
} // loop

//----------------------------------------------------------------

#include "DMS.h"

void printDMS( Print & outs, const Location_t & loc )
{
  DMS_t dms;
  dms.From( loc.lat() );

  outs.print( dms.NS() );
  outs << dms;

  dms.From( loc.lon() );
  outs.print( dms.EW() );
  outs << dms;

} // printDMS

//----------------------------------------------------------------

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
  int32_t factor = 1000000L;
  while ((degE7 < factor) && (factor > 1L)){
    outs.print( '0' );
    factor /= 10L;
  }
  
  // Print fractional degrees
  outs.print( degE7 );

} // printL

//----------------------------------------------------------------

void printAsFloat( Print & outs, const Location_t &loc )
{
  printL( outs, loc.lat() );     // display decimal degrees or...
//  printDMS( outs, loc.lat() ); // ... display degrees minutes seconds
  
  outs.print( F(", ") );

  printL( outs, loc.lat() );
//  printDMS( outs, loc.lat() );
}
