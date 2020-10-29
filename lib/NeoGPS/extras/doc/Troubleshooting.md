Troubleshooting
===============
Most problems are caused by these kinds of errors:
   1. [Device connection](#gps-device-connection-problems) (use `NMEAdiagnostic.ino`)
   2. [Poor reception](#poor-reception) (use `NMEA.ino`)
   3. [Configuration](#configuration-errors) (use `NMEAtest.ino`)
   4. [Quiet Time Interval](#quiet-time-interval) (use `NMEAorder.ino`)
   5. [Trying to do too many things](#trying-to-do-too-many-things) at the same time
   6. [When all else fails...](#when-all-else-fails)

__________________
## GPS device connection problems
The example program `NMEAdiagnostic.ino` can several kinds of problems:

1) The GPS device may not be correctly wired,
2) The GPS device is correctly wired but running at the wrong baud rate,
3) The GPS device is running at the correct baud rate, but it never sends the LAST_SENTENCE_IN_INTERVAL, as defined in NMEAGPS_cfg.h

To help diagnose the problem, use [NMEAdiagnostic.ino](/examples/NMEAdiagnostic/NMEAdiagnostic.ino).  Create an NMEAdiagnostic subdirectory, copy the same NeoGPS files into that subdirectory, along with NMEAdiagnostic.ino.  Build and upload the sketch.

##### 1) Not correctly wired
If the GPS device is not correctly connected, [NMEAdiagnostic.ino](/examples/NMEAdiagnostic/NMEAdiagnostic.ino) will display:
```
NMEAdiagnostic.INO: started
Looking for GPS device on Serial1

____________________________

Checking 9600 baud...


Check GPS device and/or connections.  No data received.
```
The example program [NMEA.ino](/examples/NMEA/NMEA.ino) will display the following, stopping after printing the header:
```
NMEA.INO: started
  fix object size = 31
  gps object size = 84
Looking for GPS device on Serial1

GPS quiet time is assumed to begin after a GST sentence is received.
  You should confirm this with NMEAorder.ino

Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,
```
You may have the GPS device connected to the wrong pins (GPS RX should be connected to Arduino TX, and GPS TX should be connected to Arduino RX), or the .INO may be using the wrong serial object: review `GPSport.h` for the expected connections, or delete the include and declare your own serial port variable.

##### 2) Wrong baud rate
If the wrong baud rate is chosen, [NMEAdiagnostic.ino](/examples/NMEAdiagnostic/NMEAdiagnostic.ino) will try each baud rate to determine the correct baud rate for your GPS device.  For each baud rate, it will display:
```
Checking 4800 baud...
No valid sentences, but characters are being received.
Check baud rate or device protocol configuration.

Received data:
884A0C4C8C480C8808884A0C8C480C4A4CC80A0C027759495995A5084C0A4C88 .J.L.H....J..H.JL....wYIY...L.L.
```
If the example program [NMEA.ino](/examples/NMEA/NMEA.ino) is using the wrong baud rate, it will print lines of empty data:
```
Local time,Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,HDOP,Rx ok,Rx err,Rx chars,
,,,,,,,,0,3,181,
,,,,,,,,0,1,422,
```
Notice how the received character count (Rx chars) is increasing, but nothing was decoded successfully (Rx ok always 0), and a few sentences had errors.  You should review your GPS device's documentation to find the correct baud rate, or use [NMEAdiagnostic](/examples/NMEAdiagnostic/NMEAdiagnostic.ino) to auto-detect the correct baud rate.
##### 3) Wrong LAST_SENTENCE_IN_INTERVAL in NMEAGPS_cfg.h
It is very important that the correct LAST_SENTENCE_IN_INTERVAL is chosen.  It is used to determine what sentences are in a particular interval (usually 1 second) and when the GPS quiet time begins (see below).

Choosing the wrong LAST_SENTENCE_IN_INTERVAL may cause GPS characters to be dropped (because the quiet time is assumed to begin at the wrong time), or prevent any update intervals from being completed (because the sketch waits for the wrong sentence to arrive).

If the wrong LAST_SENTENCE_IN_INTERVAL is chosen, [NMEAdiagnostic.ino](/examples/NMEAdiagnostic/NMEAdiagnostic.ino) will display:
```
NMEAdiagnostic.INO: started
Looking for GPS device on Serial1

____________________________

Checking 9600 baud...
Received GSV
Received GSV
Received GLL
Received RMC
Received VTG
Received GGA
Received GSA
Received GSV
Received GSV
Received GSV
Received GLL
Received RMC
Received VTG
Received GGA
Received GSA
Received GSV
Received GSV
Received GSV
Received GLL
Received RMC
Received VTG


**** NMEA sentence(s) detected!  ****
Received data:
47504753562C332C312C31312C30322C37302C3330322C32372C30352C33352C GPGSV,3,1,11,02,70,302,27,05,35,
3139372C32342C30362C34392C3035332C33312C30392C31372C3037332C3230 197,24,06,49,053,31,09,17,073,20
2A37360D0A2447504753562C332C322C31312C31322C35352C3236362C32352C *76..$GPGSV,3,2,11,12,55,266,25,

Device baud rate is 9600

GPS data fields received:

  Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,Sats,Rx ok,Rx err,Rx chars,
  3,2016-05-24 01:56:45.00,456013948,-720168555,,816,25450,7,21,0,1317,

Warning: LAST_SENTENCE_IN_INTERVAL defined to be RMC, but was never received.
  Please use NMEAorder.ino to determine which sentences your GPS device sends, and then
  use the last one for the definition in NMEAGPS_cfg.h.

** NMEAdiagnostic completed **

1 warnings
```
As instructed, you should run the NMEAorder.ino sketch to determine which sentence is last.  You may be able to see a slight pause in the "Received XXX" messages above, which would also identify the last sentence.  Edit NMEAGPS_cfg.ino and change this line:
```
#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL
```
__________________
## Poor reception
If the GPS device is correctly connected, and the sketch is receiving complete NMEA sentences, without data errors, [NMEA.ino](/examples/NMEA/NMEA.ino) displays "mostly" empty fields:
```
Local time,Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,HDOP,Rx ok,Rx err,Rx chars,
,0,,,,,,,3,0,259,
,0,,,,,,,6,0,521,
```
Notice how "Rx ok" and "Rx chars" increase each second, and "Rx err" is usually zero.  This means that the sketch is receiving complete NMEA sentences, without data errors.

Because the fields are all empty, the GPS device is not receiving signals from any satellites.  Check the antenna connection and orientation, and make sure you have an unobstructed view of the sky: go outside for the best reception, although getting close to a window may help.

Although different GPS devices behave differently when they do not have a fix, you may be able to determine how many satellites are being received from what is being reported.

As soon as the GPS device receives a signal from just one satellite, the status may change to "1" and the date and time will be reported:
```
Local time,Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,HDOP,Rx ok,Rx err,Rx chars,
2015-09-14 15:07:30,1,2015-09-14 19:07:30.00,,,,,,3,0,259,
2015-09-14 15:07:31,1,2015-09-14 19:07:30.00,,,,,,3,0,521,
```
If it continues to report only date and time, you do not have an unobstructed view of the sky: only one satellite signal is being received.

When signals from three satellites are being received, the GPS device will start reporting latitude and longitude.
When signals from four or more satellites are being received, the GPS device will start reporting altitude.

You can also enable other `gps_fix` fields or NMEA sentences.  In **GPSfix_cfg.h**, uncomment this line:
```
#define GPS_FIX_SATELLITES
```
In **NMEAGPS_cfg.h** uncomment these lines:
```
#define NMEAGPS_PARSE_GGA
#define NMEAGPS_PARSE_GSA
#define NMEAGPS_PARSE_GSV

#define NMEAGPS_PARSE_SATELLITES
#define NMEAGPS_PARSE_SATELLITE_INFO
#define NMEAGPS_MAX_SATELLITES (20)
```
This will display additional fields for how many satellites are in view, whether they are being "tracked", and their individual signal strengths.
```
Local time,Status,UTC Date/Time,Lat,Lon,Hdg,Spd,Alt,HDOP,VDOP,PDOP,Lat err,Lon err,Alt err,Sats,[sat elev/az @ SNR],Rx ok,Rx err,Rx chars,
2015-09-14 16:03:07,3,2015-09-14 20:03:07.00,,,,,,,,,,,,2,[2 71/27@14,5 65/197@33,],8,0,476,
2015-09-14 16:03:08,3,2015-09-14 20:03:08.00,,,,,,,,,,,,2,[2 71/27@14,5 65/197@33,],16,0,952,
```
This shows that only two satellites are being tracked.  You must move to a position with a better view of the sky.

__________________
## Quiet Time Interval
Because GPS devices send lots of data, it is possible for the Arduino input buffer to overflow.  Many other libraries' examples will fail when modified to print too much information, or write to an SD card (see also [next section](#trying-to-do-too-many-things)).

NeoGPS examples are structured in a way that takes advantage of a "quiet time" in the data stream.  GPS devices send a batch of sentences, once per second.  After the last sentence in the batch has been sent, the GPS device will not send any more data until the next interval.  The example programs watch for that last sentence.  When it is received, it is safe to perform time-consuming operations.

It is **critical** to know the order of the sentences sent by your specific device.  If you do not know what sentence is last in a batch, use the example program `NMEAorder.ino` to list the sentence order.  When you know the last sentence, review `NMEAGPS_cfg.h` to confirm that the correct value on this line:

```
#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL
```

Most example programs depend on this statement.  `NMEAdiagnostic.ino` will emit a warning if that sentence is never recevied.  If `LAST_SENTENCE_IN_INTERVAL` is not correct for your device, the example programs will probably lose GPS data, especially on `SoftwareSerial` data ports.  The example programs may behave like they are using the wrong baud rate: empty fields, increasing Rx Errors, and increasing Rx Chars.  Basically, the example programs are [Trying To Do Too Much](#trying-to-do-too-many-things) at the wrong time.  With the correct `LAST_SENTENCE_IN_INTERVAL`, the example programs will not lose GPS data.

## Configuration errors
Because there are so many configurable items, it is possible that your configuration prevents acquiring the desired GPS information.

The compiler **cannot** catch message set dependencies: the enum 
`nmea_msg_t` is always available.  So even though a `fix` member is enabled, 
you may have disabled all messages that would have set its value.  
`NMEAtest.ino` can be used to check some configurations.

For example, if your application needs altitude, you **must** enable the GGA sentence.  No other sentence provides the altitude member (see [this table](Choosing.md)).  If `NMEA_PARSE_GGA` is not defined,  `gps.decode()` will return COMPLETED after a GGA is received, but no parts of the GGA sentence will have been parsed, and altitude will never be valid.  NeoGPS will _recognize_ the GGA sentence, but it will not be parsed.

The compiler will catch any attempt to use parts of a `fix` that have been 
configured out: you will see something like `gps_fix does not have member 
xxx`.

There are also compile-time checks to make sure the configuration is valid.  For example, if you enable `NMEAGPS_PARSE_SATELLITES` so that you can access the array of satellite information, you *must* enable `GPS_FIX_SATELLITES`.  An error message will tell you to do that.  Until you disable `NMEAGPS_PARSE_SATELLITES` **or** enable `GPS_FIX_SATELLITES`, it will not compile.

__________________
## Trying to do too many things
Many libraries and their examples, and I mean almost all of 'em, are not structured in a way that lets you do more than one thing in a sketch.   The result: the example program works great, but adding anything to it breaks it.

#### Printing too much
Many programmers run into trouble because they try to print too much debug info.  The Arduino `Serial.print` function will "block" until those output characters can be stored in a buffer.  While the sketch is blocked at `Serial.print`, the GPS device is probably still sending data.  The _input_ buffer on an Arduino is only 64 bytes, about the size of one NMEA sentence.  After 64 bytes have been received stored, all other data is dropped!  Depending on the GPS baud rate and the Serial Monitor baud rate, it may be very easy to lose GPS characters.

It is crucial to call `gps.available( gps_port )` or `serial.read()` frequently, and  _never_ to call a blocking function that takes more than (64*11/baud) seconds.  If the GPS is running at 9600, you cannot block for more than 70ms.  If your debug `Serial` is also running at 9600, you cannot write more than 64 bytes consecutively (i.e., in less than 70ms).

#### Blocking operations
Most Arduino libraries are written in a blocking fashion.  That is, if you call a library's function, it will not return from that function until the operation has been completed.  If that operation takes a long time, GPS characters will be dropped.

Many programmers want to write GPS data to an SD card.  This is completely reasonable to do, but an `SD.write` can block long enough to cause the input buffer to overflow.  SD libraries have their own buffers, and when they are filled, the library performs SPI operations to "flush" the buffer to the SD card.  While that is happening, the GPS device is _still_ sending data, and it will eventually overflow the serial input buffer.

This is a very common problem!  Here are some diagrams to help explain the timing for the Adafruit_GPS library.  First, lets look at how the incoming GPS data relates to reading and parsing it:

<img src="images/GPS%20Timing%200.jpg"/>

Note how loop calls GPS.read, and when it has read all the chars that have been received up to that point, it returns.  loop may get called lots of times while it's waiting for the chars to come in.  Eventually, the whole sentence is received, newNMEAreceived returns true, and your sketch can `parse` the new data (not needed for **NeoGPS**).

The problem is that if you try to do anything that takes "too long", `GPS.read` won't get called.  The incoming chars stack up in the input buffer until it's full.  After that, the chars will be dropped:

<img src="images/GPS%20Timing%201.jpg"/>

The next sentence, a GPRMC, continues to come in while `Serial.print` and `SD.write` are doing their thing... and data gets lost.

Fortunately, there are two ways to work around this:

#### **1)** Use an interrupt-driven approach

The received data could be handled by an **I**nterrupt **S**ervice **R**outine.   The example program [NMEA_isr.INO](/examples/NMEA_isr/NMEA_isr.ino) shows how to handle the received GPS characters *during* the RX interrupt.  This program uses one of the replacement **NeoXXSerial** libraries to attach an interrupt handler to the GPS serial port.

When a character is received, the ISR is called, where it is immediately decoded.  Normally, the character is stored in an input buffer, and you have to call `available()` and then `read()` to retrieve the character.  Handling it in the ISR totally avoids having to continuously call `Serial1.read()`, and is much more efficient.  Your program does not have to be structured around the GPS quiet time.

The ISR can also save (i.e., "buffer") complete fixes as they are completed.  This allows the main sketch to perform an operation that takes multiple update intervals.  This is especially important if your GPS is sending updates 10 times per second (10Hz), and your SD card takes ~150ms to complete a logging write.

Normally, this would cause a loss of data.  You can specify the number of fixes to be stored by the ISR for later use: simply set NMEAGPS_FIX_MAX to 2.  This will allow 2 complete fixes, accumulated from several sentences each, to be stored until `loop()` can call `gps.read()`.  This is referred to as "fix-oriented operation" in the Data Model description.

While interrupt handling is considered a more advanced technique, it is similar to the existing Arduino [attachInterrupt](https://www.arduino.cc/en/Reference/AttachInterrupt) function for detecting when pin change.

Which **NeoXXLibrary** should you use?

* If `Serial` or `Serial1` is connected to the GPS device, you can use [NeoHWSerial](https://github.com/SlashDevin/NeoHWSerial).
* If the Input Capture pin can be connected to the GPS device, as required for AltSoftSerial, you can use [NeoICSerial](https://github.com/SlashDevin/NeoICSerial).
* If neither of those connections is possible, you can [NeoSWSerial](https://github.com/SlashDevin/NeoSWSerial) on almost any pair of digital pins.  It only supports a few baud rates, though.

#### **2)** Restructure `loop()` to do time-consuming operations during the GPS [quiet time](#quiet-time-interval).

All non-ISR example programs are structured this way.  The "quiet time" is perfect for doing other things:

<img src="images/GPS%20Timing%202.jpg"/>

All you need to do is hold on to the GPS information (date, time, location, etc.) until the quiet time comes around.  You'll need to take the same approach for each additional task.  For additional sensors, hold on to the temperature, acceleration, whatever, until the quiet time comes around.  *Then* perform the blocking operation, like `SD.write`, and no GPS data will be lost.

This is why NeoGPS uses a `fix` structure: it can be
   * _populated_ as the characters are received,
   * _copied/merged_ when a sentence is complete, and then
   * _used_ anytime (for fast operations) or during the quiet time (for slow operations).

You do not have to call a "parse" function after a complete sentence has been received -- the data was parsed as it was received.  Essentially, the processing time for parsing is spread out across the receipt of all characters.  When the last character of the sentence is received (i.e. `gps.available()` or `gps.decode(c) == DECODE_COMPLETED`), the relevant members of `gps.fix()` have already been populated.

These example programs are structured so that the (relatively) slow printing operations are performed during the GPS quiet time.  Simply replace those trace/print statements with your specific code.

__________________
## When all else fails

If you still do not have enough time to complete your tasks during the GPS quiet time, you can
   * Increase the baud rate on the debug port (takes less time to print)
   * Increase the baud rate on the GPS port (increases quiet time)
   * Configure the GPS device to send fewer sentences (decreases parsing time, increases quiet time)
   * Use a binary protocol for your specific device (decreases parsing time, increases quiet time)
   * Watch for a specific message to be COMPLETED, then begin your specific processing.  This may cause some sentences to lose characters, but they may not be necessary.  See comments regarding `LAST_SENTENCE_IN_INTERVAL` above.
   * Use the interrupt-driven approach, described above.  It is *guaranteed* to work!
