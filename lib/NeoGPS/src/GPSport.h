#ifndef GPSport_h
#define GPSport_h

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

//-----------------------------------------------------------
//  There are 2 ways this file can be used:
//
//  I.  AS IS, which tries to *guess* which port a beginner should use.
//      If you include <SoftwareSerial.h>, <NeoSWSerial.h>, <AltSoftSerial.h>, 
//      <NeoICSerial.h>  *OR*  <NeoHWSerial.h> before this file (in the INO) 
//      or on line 152 below, one of those ports will be used for the GPS.  
//      This file cannot always guess what you want, so you may need to use it 
//      in the 2nd way...
//
//  II. *REPLACE EVERYTHING* in this file to *specify* which port and 
//      which library you want to use for that port.  Just declare the
//      port here.  You must declare three things: 
//
//         1) the "gpsPort" variable (used by all example programs),
//         2) the string for the GPS_PORT_NAME (displayed by all example programs), and
//         3) the DEBUG_PORT to use for Serial Monitor print messages (usually Serial).
//
//      The examples will display the device you have selected during setup():
//
//         DEBUG_PORT.println( F("Looking for GPS device on " GPS_PORT_NAME) );
//
//      Choosing the serial port connection for your GPS device is probably
//      the most important design decision, because it has a tremendous impact
//      on the processor load.  Here are the possible configurations, 
//      from BEST to WORST:
//
// ---  The BEST choice is to use a HardwareSerial port. You could use Serial 
//      on any board, but you will have to disconnect the Arduino RX pin 0 
//      from the GPS TX pin to upload a new sketch over USB.  This is a very 
//      reliable connection:
//
//             #define gpsPort Serial
//             #define GPS_PORT_NAME "Serial"
//             #define DEBUG_PORT Serial
//
//      If you have a Mega, Leo or Due board, you could use Serial1, 
//      Serial2 or Serial3:
//
//             #define gpsPort Serial1
//             #define GPS_PORT_NAME "Serial1"
//             #define DEBUG_PORT Serial
//
//      These extra ports do not have to be disconnected to upload new
//      sketches over USB.
//
//      Use NeoHWSerial if you want to handle GPS characters
//      in an Interrupt Service Routine (see NMEA_isr.INO).
//      Also uncomment NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h.
//
//             #include <NeoHWSerial.h>
//             #define gpsPort NeoSerial
//             #define GPS_PORT_NAME "NeoSerial"
//             #define DEBUG_PORT NeoSerial
//
//      Note that the DEBUG_PORT must be NeoSerial, not Serial
//      (see NeoHWSerial documentation for additional requirements).
//
// ---  The SECOND BEST choice is AltSoftSerial.  It uses 10x to 500x
//      more processor time than a HardwareSerial port, but it is the BEST
//      *software* serial port library.  It only works on two specific pins
//      (see the AltSoftSerial documentation).
//
//             #include <AltSoftSerial.h>
//             AltSoftSerial gpsPort; // 8 & 9 for an UNO
//             #define GPS_PORT_NAME "AltSoftSerial"
//             #define DEBUG_PORT Serial
//
//      Use NeoICSerial if you want to handle GPS characters
//      in an Interrupt Service Routine (see NMEA_isr.INO).
//      Also uncomment NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h.
//
//             #include <NeoICSerial.h>
//             NeoICSerial gpsPort; // 8 & 9 for an UNO
//             #define GPS_PORT_NAME "NeoICSerial"
//             #define DEBUG_PORT Serial
//
// ---  The THIRD BEST choice is NeoSWSerial.  It is almost as efficient as
//      AltSoftSerial, but it only supports baud rates 9600, 19200 or 38400.
//
//             #include <NeoSWSerial.h>
//             NeoSWSerial gpsPort(3, 2);
//             #define GPS_PORT_NAME "NeoSWSerial(3,2)"
//             #define DEBUG_PORT Serial
//
//      NeoSWSerial supports handling GPS characters in an Interrupt Service
//      Routine (see NMEA_isr.INO).  If you want to do that, also uncomment 
//      NMEAGPS_INTERRUPT_PROCESSING in NMEAGPS_cfg.h.
//
// ---  The WORST choice is SoftwareSerial.  IT IS NOT RECOMMENDED because
//      it disables interrupts for the entire time a character is sent OR
//      received.  This interferes with other parts of your sketch or with
//      other libraries.  It prevents receiving or sending data on any other 
//      port.  It cannot transmit and receive at the same time.  95% of the
//      CPU time will be spent twiddling its thumbs.  It may support 57600,
//      but 115200 is rarely reliable.
//
//             #include <SoftwareSerial.h>
//             SoftwareSerial gpsPort(2,3);
//             #define GPS_PORT_NAME "SoftwareSerial(8,9)"
//             #define DEBUG_PORT Serial
//
//      You should *seriously* consider using other pins or lower baud rates, 
//      so that you could use one of the other recommended ports.
//-----------------------------------------------------------

// DEFAULT file contents:
//   *GUESS* which port should be used.  If you know what port you want to use,
//             *DELETE* the rest of this file and declare your own GPS port variable,
//             GPS port name string, and debug print port variable (see above).

#if defined(SERIAL_PORT_HARDWARE_OPEN)

  #if SERIAL_PORT_HARDWARE_OPEN == Serial1

    //  This Arduino has more than one hardware serial port, 
    //      use Serial1 (or NeoSerial1).
    #define NEOGPS_USE_SERIAL1

  #endif

#endif

#ifdef NEOGPS_USE_SERIAL1

  #if defined( NMEAGPS_INTERRUPT_PROCESSING )
    #include <NeoHWSerial.h>
    #define gpsPort NeoSerial1
    #define GPS_PORT_NAME "NeoSerial1"
  #else
    #define gpsPort Serial1
    #define GPS_PORT_NAME "Serial1"
  #endif

#else
  //      Uncomment *zero* or *one* of the serial port library includes.
  //      If none of these includes are uncommented, Serial will be used.
  //
  //#include <NeoHWSerial.h>    // NeoSerial or NeoSerial1 Interrupt-style processing
  #include <AltSoftSerial.h>    // <-- DEFAULT.  Two specific pins required
  //#include <NeoICSerial.h>    // AltSoftSerial with Interrupt-style processing (see docs)
  //#include <NeoSWSerial.h>    // Any pins, only @ 9600, 19200 or 38400 baud
  //#include <SoftwareSerial.h> // NOT RECOMMENDED!

  #if defined( SoftwareSerial_h )
    #define SS_TYPE SoftwareSerial
    #warning SoftwareSerial is NOT RECOMMENDED.  Use AltSoftSerial or NeoSWSerial instead.

    #if defined( NMEAGPS_INTERRUPT_PROCESSING )
      #error You cannot use SoftwareSerial with INTERRUPT_PROCESSING.  Use NeoSWSerial or NeoICSerial instead.
    #endif

  #elif defined( NeoSWSerial_h )
    #define SS_TYPE NeoSWSerial

  #elif defined( AltSoftSerial_h )
    #define SS_TYPE AltSoftSerial
    #define RX_PIN -1  // doesn't matter because it only works...
    #define TX_PIN -1  //    ...on two specific pins

    #if defined( NMEAGPS_INTERRUPT_PROCESSING )
      #error You cannot use AltSoftSerial with INTERRUPT_PROCESSING.  Use NeoICSerial instead.
    #endif

  #elif defined( NeoICSerial_h )
    #define SS_TYPE NeoICSerial
    #define RX_PIN -1  // doesn't matter because it only works...
    #define TX_PIN -1  //    ...on two specific pins

  #elif defined( NeoHWSerial_h )
    #define gpsPort NeoSerial
    #define GPS_PORT_NAME "NeoSerial"
    #warning Using Serial (actually NeoSerial) for GPS connection.

  #else
    // No serial library files were included before this file, just use Serial.
    // #define gpsPort Serial
    #define GPS_PORT_NAME "Serial"
    #warning Using Serial for GPS connection.
    
    // You will see this warning if you want to use Serial for the GPS, because no
    // software serial port libraries were included.  That is a good choice.
    //
    // To use a different serial port for GPS device, you must include a serial header 
    // before "#include GPSport.h" in the INO.  It may be simpler to replace the 
    // entire contents of this file with your own declarations.  Follow the
    // instructions above for declaring your own "gpsPort" variable. Everything else
    // in this file should be deleted. See Installation instructions for more
    // information.
  #endif
#endif


#ifdef SS_TYPE

  //---------------------------------------------------------------
  // The current Board (an Uno?) does not have an extra serial port.
  // Use a software serial library to listen to the GPS device.
  //   You should expect to get some RX errors, which may
  //   prevent getting fix data every second.  YMMV.

  // Default Arduino RX pin number that is connected to the GPS TX pin
  #ifndef RX_PIN
    #define RX_PIN 4
  #endif

  // Default Arduino TX pin number that is connected to the GPS RX pin
  #ifndef TX_PIN
    #define TX_PIN 3
  #endif

  SS_TYPE gpsPort( RX_PIN, TX_PIN );

  //  A little preprocessor magic to get a nice string
  #define xstr(x) str(x)
  #define str(x) #x
  #define GPS_PORT_NAME \
    xstr(SS_TYPE) "( RX pin " xstr(RX_PIN) \
                  ", TX pin " xstr(TX_PIN) " )"

  #ifdef NEOGPS_USE_SERIAL1
    //  If you *really* want to do this, or you just happened to include 
    //  a software serial library header for something else, you're
    //  better off *not* including this file.  Just declare
    //  your own gpsPort in your INO file.

    #error You should be using Serial1 for the GPS device.  \
        Software serial libraries are very inefficient and unreliable when \
        used for GPS communications!
  #endif

#endif

//------------------------------------------------------------
// When NeoHWSerial is used, none of the built-in HardwareSerial
//   variables can be used: Serial, Serial1, Serial2 and Serial3
//   *cannot* be used.  Instead, you must use the corresponding
//   NeoSerial, NeoSerial1, NeoSerial2 or NeoSerial3.  This define
//   is used to substitute the appropriate Serial variable in
//   all debug prints.

#ifdef NeoHWSerial_h
  #define DEBUG_PORT NeoSerial
#else
  #define DEBUG_PORT Serial // default for most sketches
#endif

// End of guessing game.
//------------------------

#endif
