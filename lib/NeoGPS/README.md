NeoGPS
======

This fully-configurable Arduino library uses _**minimal**_ RAM, PROGMEM and CPU time, 
requiring as few as _**10 bytes of RAM**_, **866 bytes of PROGMEM**, and **less than 1mS of CPU time** per sentence.  

It supports the following protocols and messages:

#### NMEA 0183
* GPGGA - System fix data
* GPGLL - Geographic Latitude and Longitude
* GPGSA - DOP and active satellites
* GPGST - Pseudo Range Error Statistics
* GPGSV - Satellites in View
* GPRMC - Recommended Minimum specific GPS/Transit data
* GPVTG - Course over ground and Ground speed
* GPZDA - UTC Time and Date

The "GP" prefix usually indicates an original [GPS](https://en.wikipedia.org/wiki/Satellite_navigation#GPS) source.  NeoGPS parses *all* Talker IDs, including
  * "GL" ([GLONASS](https://en.wikipedia.org/wiki/Satellite_navigation#GLONASS)),
  * "BD" or "GB" ([BeiDou](https://en.wikipedia.org/wiki/Satellite_navigation#BeiDou)),
  * "GA" ([Galileo](https://en.wikipedia.org/wiki/Satellite_navigation#Galileo)), and
  * "GN" (mixed)

This means that GLRMC, GBRMC or BDRMC, GARMC and GNRMC from the latest GPS devices (e.g., ublox M8N) will also be correctly parsed.  See discussion of Talker IDs in [Configurations](extras/doc/Configurations.md#enabledisable-the-talker-id-and-manufacturer-id-processing).

Most applications can be fully implemented with the standard NMEA messages above.  They are supported by almost all GPS manufacturers.  Additional messages can be added through derived classes (see ublox and Garmin sections below).

Most applications will use this simple, familiar loop structure:
```
NMEAGPS gps;
gps_fix fix;

void loop()
{
  while (gps.available( gps_port )) {
    fix = gps.read();
    doSomeWork( fix );
  }
}
```
For more information on this loop, see the [Usage](extras/doc/Data%20Model.md#usage) section on the [Data Model](extras/doc/Data%20Model.md) page.

(This is the plain Arduino version of the [CosaGPS](https://github.com/SlashDevin/CosaGPS) library for [Cosa](https://github.com/mikaelpatel/Cosa).)

Goals
======
In an attempt to be reusable in a variety of different programming styles, this library supports:
* resource-constrained environments (e.g., ATTINY targets)
* sync or async operation (reading in `loop()` vs interrupt processing)
* event or polling (deferred handling vs. continuous calls in `loop()`)
* coherent fixes (merged from multiple sentences) vs. individual sentences
* optional buffering of fixes
* optional floating point
* configurable message sets, including hooks for implementing proprietary NMEA messages
* configurable message fields
* multiple protocols from same device
* any kind of input stream (Serial, [NeoSWSerial](https://github.com/SlashDevin/NeoSWSerial), I2C, PROGMEM arrays, etc.)

Inconceivable!
=============

Don't believe it?  Check out these detailed sections:

Section  |  Description
-------- |  ------------
[License](LICENSE) | The Fine Print
[Installing](extras/doc/Installing.md) | Copying files
[Data Model](extras/doc/Data%20Model.md) | How to parse and use GPS data
[Configurations](extras/doc/Configurations.md) | Tailoring NeoGPS to your needs
[Performance](extras/doc/Performance.md) | 37% to 72% faster!  Really!
[RAM requirements](extras/doc/RAM.md) | Doing it without buffers!
[Program Space requirements](extras/doc/Program.md) | Making it fit
[Examples](extras/doc/Examples.md) | Programming styles
[Troubleshooting](extras/doc/Troubleshooting.md) | Troubleshooting
[Extending NeoGPS](extras/doc/Extending.md) | Using specific devices
[ublox](extras/doc/ublox.md) | ublox-specific code
[Garmin](extras/doc/Garmin.md) | Garmin-specific code
[Tradeoffs](extras/doc/Tradeoffs.md) | Comparing to other libraries
[Acknowledgements](extras/doc/Acknowledgements.md) | Thanks!
