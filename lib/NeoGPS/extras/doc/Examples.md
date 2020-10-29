Examples
======
In an attempt to be reusable in a variety of different programming styles, this library supports:
* sync or async operation (main `loop()` vs interrupt processing)
* fused or not fused (multiple reports into one fix)
* optional buffering of fixes
* optional floating point
* configurable message sets, including hooks for implementing proprietary NMEA messages
* configurable message fields
* multiple protocols from same device

These example programs demonstrate how to use the classes in the different programming styles:
* [NMEA](/examples/NMEA/NMEA.ino) - sync, single fix, standard NMEA only (RMC sentence only)
* [NMEA_isr](/examples/NMEA_isr/NMEA_isr.ino) - **async**, single fix, standard NMEA only (RMC sentence only)
* [NMEAblink](/examples/NMEAblink/NMEAblink.ino) - sync, single fix, standard NMEA only, minimal example, only blinks LED
* [NMEAloc](/examples/NMEAloc/NMEAloc.ino) - sync, single fix, minimal example using only standard NMEA RMC sentence
* [NMEAlocDMS](/examples/NMEAlocDMS/NMEAlocDMS.ino) - same as NMEAloc.ino, but displays location in Degrees, Minutes and Seconds
* [NMEAaverage](/examples/NMEAaverage/NMEAaverage.ino) - sync, single fix, averages a high-accuracy location over time
* [NMEAtimezone](/examples/NMEAtimezone/NMEAtimezone.ino) - same as NMEAloc.ino, but displays local time instead of UTC (GMT)
* [NMEASDlog](/examples/NMEASDlog/NMEASDlog.ino) - **async**, buffered fixes, standard NMEA only (RMC sentence only), logging to SD card
* [PUBX](/examples/PUBX/PUBX.ino) - sync, coherent fix, standard NMEA + ublox proprietary NMEA
* [ublox](/examples/ublox/ublox.ino) - sync or **async**, coherent fix, ublox binary protocol UBX

Preprocessor symbol `USE_FLOAT` can be used in [Streamers.cpp](/src/Streamers.cpp) to select integer or floating-point output.

See also important information in `NMEAorder.ino` below, and the [Installing](Installing.md), [Configurations](Configurations.md) and [Troubleshooting](Troubleshooting.md) sections.

##Diagnostics
Several programs are provided to help diagnose GPS device problems:

### Connection and Baud Rate

* [NMEAdiagnostic](/examples/NMEAdiagnostic/NMEAdiagnostic.ino)
 
This program listens for sentences and, if none are detected, tries a different baud rate.  When sentences are detected, the correct baud rate is displayed.  The received data may help you determine the problem (e.g., dropped characters or binary protocol).

See the [Troubleshooting](Troubleshooting.md) section for more details.

### Sentence order

* [NMEAorder](/examples/NMEAorder/NMEAorder.ino)

This program determines the order of NMEA sentences sent during each 1-second interval:

```
NMEAorder.INO: started
fix object size = 44
NMEAGPS object size = 72
Looking for GPS device on Serial1
.....................
Sentence order in each 1-second interval:
  RMC
  VTG
  GGA
  GSA
  GSV
  GSV
  GSV
  GSV
  GLL
```

The last sentence is of particular interest, as it is used to determine when the quiet time begins.  All example programs **depend** on knowing the last sentence (see [Quiet Time Interval](Troubleshooting#quiet-time-interval)).

### Self-test Program

* [NMEAtest](/examples/NMEAtest/NMEAtest.ino)

For this program, **No GPS device is required**.  Test bytes are streamed from PROGMEM character arrays.  Various strings are passed to `decode` and the expected pass or fail results are displayed.  If **NeoGPS** is correctly configured, you should see this on your SerialMonitor:

```
NMEA test: started
fix object size = 44
NMEAGPS object size = 72
Test string length = 75
PASSED 11 tests.
------ Samples ------
Results format:
  Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,HDOP,VDOP,PDOP,Lat err,Lon err,Alt err,Sats,[sat],

Input:  $GPGGA,092725.00,4717.11399,N,00833.91590,E,1,8,1.01,499.6,M,48.0,M,,0*5B
Results:  3,2000-01-01 09:27:25.00,472852332,85652650,,,49960,1010,,,,,,8,[],

Input:  $GPRMC,092725.00,A,2520.69213,S,13101.94948,E,0.004,77.52,091202,,,A*43
Results:  3,2002-12-09 09:27:25.00,-253448688,1310324913,7752,4,,,,,,,,,[],

Input:  $GPRMC,162254.00,A,3647.6643,N,8957.5193,W,0.820,188.36,110706,,,A*49
Results:  3,2006-07-11 16:22:54.00,367944050,-899586550,18836,820,,,,,,,,,[],

Input:  $GPRMC,235959.99,A,2149.65726,N,16014.69256,W,8.690,359.99,051015,9.47,E,A*26
Results:  3,2015-10-05 23:59:59.99,218276210,-1602448760,35999,8690,,,,,,,,,[],

Input:  $GNGLL,0105.60764,S,03701.70233,E,225627.00,A,A*6B
Results:  3,2000-01-01 22:56:27.00,-10934607,370283722,,,,,,,,,,,[],

Input:  $GPGGA,064951.000,2307.1256,N,12016.4438,E,1,8,0.95,39.9,M,17.8,M,,*63
Results:  3,2000-01-01 06:49:51.00,231187600,1202740633,,,3990,950,,,,,,8,[],

Input:  $GPRMC,064951.000,A,2307.1256,N,12016.4438,E,0.03,165.48,260406,3.05,W,A*2C
Results:  3,2006-04-26 06:49:51.00,231187600,1202740633,16548,30,,,,,,,,,[],

Input:  $GPVTG,165.48,T,,M,0.03,N,0.06,K,A*36
Results:  3,,,,16548,30,,,,,,,,,[],

Input:  $GPGSA,A,3,29,21,26,15,18,09,06,10,,,,,2.32,0.95,2.11*00
Results:  3,,,,,,,950,,2320,,,,,[],

Input:  $GPGSV,3,1,09,29,36,029,42,21,46,314,43,26,44,020,43,15,21,321,39*7D
Results:  ,,,,,,,,,,,,,9,[29,21,26,15,],

Input:  $GPGSV,3,2,09,18,26,314,40,09,57,170,44,06,20,229,37,10,26,084,37*77
Results:  ,,,,,,,,,,,,,9,[29,21,26,15,18,9,6,10,],

Input:  $GPGSV,3,3,09,07,,,26*73
Results:  ,,,,,,,,,,,,,9,[29,21,26,15,18,9,6,10,7,],

Input:  $GNGST,082356.00,1.8,,,,1.7,1.3,2.2*60
Results:  ,2000-01-01 08:23:56.00,,,,,,,,,170,130,,,[29,21,26,15,18,9,6,10,7,],

Input:  $GNRMC,083559.00,A,4717.11437,N,00833.91522,E,0.004,77.52,091202,,,A,V*33
Results:  3,2002-12-09 08:35:59.00,472852395,85652537,7752,4,,,,,,,,,[29,21,26,15,18,9,6,10,7,],

Input:  $GNGGA,092725.00,4717.11399,N,00833.91590,E,1,08,1.01,499.6,M,48.0,M,,*45
Results:  3,2000-01-01 09:27:25.00,472852332,85652650,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GLZDA,225627.00,21,09,2015,00,00*70
Results:  ,2015-09-21 22:56:27.00,,,,,,,,,,,,,[29,21,26,15,18,9,6,10,7,],

--- floating point conversion tests ---

Input:  $GPGGA,092725.00,3242.9000,N,11705.8169,W,1,8,1.01,499.6,M,48.0,M,,0*49
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969483,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8170,W,1,8,1.01,499.6,M,48.0,M,,0*41
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969500,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8171,W,1,8,1.01,499.6,M,48.0,M,,0*40
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969517,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8172,W,1,8,1.01,499.6,M,48.0,M,,0*43
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969533,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8173,W,1,8,1.01,499.6,M,48.0,M,,0*42
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969550,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8174,W,1,8,1.01,499.6,M,48.0,M,,0*45
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969567,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8175,W,1,8,1.01,499.6,M,48.0,M,,0*44
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969583,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],

Input:  $GPGGA,092725.00,3242.9000,N,11705.8176,W,1,8,1.01,499.6,M,48.0,M,,0*47
Results:  3,2000-01-01 09:27:25.00,327150000,-1170969600,,,49960,1010,,,,,,8,[29,21,26,15,18,9,6,10,7,],
```

### Benchmark

*  [NMEAbenchmark](/examples/NMEAbenchmark/NMEAbenchmark.ino)
  
For this program, **No GPS device is required**.  GGA, RMC and GSV sentences can be tested.  Bytes are streamed from PROGMEM character arrays and parsed to determine execution times.  All times are displayed in microseconds:

```
NMEAbenchmark: started
fix object size = 22
NMEAGPS object size = 29
GGA time = 844
GGA no lat time = 497
```
