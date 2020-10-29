#include <NMEAGPS.h>

//======================================================================
//  Program: NMEAblink.ino
//
//  Prerequisites:
//     1) NMEA.ino works with your device
//
//  Description:  This program will toggle the LED once per second,
//     when the LAST_SENTENCE_IN_INTERVAL is received.
//
//     Because no actual GPS data is used, you could disable all
//       messages (except the LAST_SENTENCE) and all gps_fix members.
//       It would still receive a 'fix' oncer per second, without
//       without using any RAM or CPU time to parse or save
//       the (unused) values.  Essentially, this app uses the LAST_SENTENCE
//       as a 1PPS signal.
//
//     Note: Because this example does not use 'Serial', you
//       could use 'Serial' for the gpsPort, like this:
//
//       #define gpsPort Serial
//
//     See GPSport.h for more information.
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

static NMEAGPS   gps;
static const int led = 13;

//--------------------------

void setup()
{
  gpsPort.begin(9600);

  pinMode(led, OUTPUT);
}

//--------------------------

void loop()
{
  if (gps.available( gpsPort)) {
    gps.read(); // don't really do anything with the fix...

    digitalWrite( led, !digitalRead(led) ); // toggle the LED
  }
}
