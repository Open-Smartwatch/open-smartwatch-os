Configuration
=============
All configuration items are conditional compilations: a `#define` controls an `#if`/`#endif` section.
Comment out any items to be disabled or excluded from your build.
Where possible, checks are performed to verify that you have chosen a "valid" 
configuration: you may see `#error` messages in the build log.  See also [Choosing Your Configuration](Choosing.md) and [Troubleshooting](Troubleshooting.md).

# class gps_fix
The following configuration items are near the top of GPSfix_cfg.h:
```
// Enable/Disable individual parts of a fix, as parsed from fields of a $GPxxx sentence
#define GPS_FIX_DATE
#define GPS_FIX_TIME
#define GPS_FIX_LOCATION
#define GPS_FIX_LOCATION_DMS
#define GPS_FIX_ALTITUDE
#define GPS_FIX_SPEED
#define GPS_FIX_HEADING
#define GPS_FIX_SATELLITES
#define GPS_FIX_HDOP
#define GPS_FIX_VDOP
#define GPS_FIX_PDOP
#define GPS_FIX_LAT_ERR
#define GPS_FIX_LON_ERR
#define GPS_FIX_ALT_ERR
#define GPS_FIX_GEOID_HEIGHT
```
See the [Data Model](Data%20Model.md) page and `GPSfix.h` for the corresponding members that are enabled or disabled by these defines.

========================
# class NMEAGPS
The following configuration items are near the top of NMEAGPS_cfg.h.
#### Enable/Disable parsing the fields of a $GPxxx sentence
```
#define NMEAGPS_PARSE_GGA
#define NMEAGPS_PARSE_GLL
#define NMEAGPS_PARSE_GSA
#define NMEAGPS_PARSE_GSV
#define NMEAGPS_PARSE_GST
#define NMEAGPS_PARSE_RMC
#define NMEAGPS_PARSE_VTG
#define NMEAGPS_PARSE_ZDA
```
#### Select the last sentence in an update interval
This is used to determine when the GPS quiet time begins and when a batch of coherent sentences have been merged.  It is crucial to know when fixes can be marked as available, and when you can perform some time-consuming operations.
```
#define LAST_SENTENCE_IN_INTERVAL NMEAGPS::NMEA_GLL
```
You can use `NMEAorder.ino` to determine the last sentence sent by your device.
#### Enable/Disable No, Implicit, Explicit Merging
If you want NO merging, comment out both defines.  Otherwise, uncomment the IMPLICIT or EXPLICIT define.
```
#define NMEAGPS_EXPLICIT_MERGING
//#define NMEAGPS_IMPLICIT_MERGING
```
See [Merging](Merging.md) for more information.
#### Define the fix buffer size.
The NMEAGPS object will hold on to this many fixes before an overrun occurs.  The buffered fixes can be obtained by calling `gps.read()`.  You can specify zero, but you have to be sure to call `gps.read()` before the next sentence starts.
```
#define NMEAGPS_FIX_MAX 1
```
#### Enable/Disable interrupt-style processing
Define how fixes are dropped when the fix buffer is full.
```
#define NMEAGPS_KEEP_NEWEST_FIXES true
```
   true  = the oldest fix will be dropped, and the new fix will be saved.
   false = the new fix will be dropped, and all old fixes will be saved.
#### Enable/Disable interrupt-style processing
If you are using one of the NeoXXSerial libraries to `attachInterrupt`, this must be uncommented to guarantee safe access to the buffered fixes with `gps.read()`.  For  normal polling-style processing, it must be commented out.
```
//#define NMEAGPS_INTERRUPT_PROCESSING
```
#### Enable/Disable the talker ID and manufacturer ID processing.
There are two kinds of NMEA sentences:

1. Standard NMEA sentences begin with "$ttccc", where
      "tt" is the talker ID (e.g., GP), and
      "ccc" is the variable-length sentence type (e.g., RMC).

    For example, "$GPGLL,..." is a GLL sentence (Geographic Lat/Long) 
    transmitted by talker "GP".  This is the most common talker ID.  Some devices
    may report "$GNGLL,..." when a mix of GPS and non-GPS satellites have been
    used to determine the GLL data (e.g., GLONASS+GPS).

2. Proprietary NMEA sentences (i.e., those unique to a particular manufacturer) 
    begin with "$Pmmmccc", where
      "P" is the NMEA-defined prefix indicator for proprietary messages,
      "mmm" is the 3-character manufacturer ID, and
      "ccc" is the variable-length sentence type (it can be empty).

No validation of manufacturer ID and talker ID is performed in this
base class.  For example, although "GP" is a common talker ID, it is not
guaranteed to be transmitted by your particular device, and it IS NOT REQUIRED. 
If you need validation of these IDs, or you need to use the extra information
provided by some devices, you have two independent options:

1. Enable SAVING the ID: When `decode` returns DECODE_COMPLETED, the `talker_id`
and/or `mfr_id` members will contain ID bytes.  The entire sentence will be
parsed, perhaps modifying members of `fix`.  You should enable one or both IDs
if you want the information in all sentences *and* you also want to know the ID
bytes.  This adds 2 bytes of RAM for the talker ID, and 3 bytes of RAM for the
manufacturer ID.

2. Enable PARSING the ID:  The virtual `parse_talker_id` and `parse_mfr_id` will
receive each ID character as it is received.  If it is not a valid ID, return
`false` to abort processing the rest of the sentence.  No CPU time will be wasted
on the invalid sentence, and no `fix` members will be modified.  You should
enable this if you want to ignore some IDs.  You must override `parse_talker_id`
and/or `parse_mfr_id` in a derived class.

```
#define NMEAGPS_SAVE_TALKER_ID
#define NMEAGPS_PARSE_TALKER_ID

#define NMEAGPS_SAVE_MFR_ID
#define NMEAGPS_PARSE_MFR_ID
```
#### Enable/Disable parsing of NMEA proprietary messages
If you are deriving a class from NMEAGPS to parse proprietary messages, you must uncomment this define:
```
//#define NMEAGPS_PARSE_PROPRIETARY
```
#### Enable/Disable tracking the current satellite array
You can also enable tracking the detailed information for each satellite, and how many satellites you want to track.
Although many GPS receivers claim to have 66 channels of tracking, 16 is usually the maximum number of satellites 
tracked at any one time.
```
#define NMEAGPS_PARSE_SATELLITES
#define NMEAGPS_PARSE_SATELLITE_INFO
#define NMEAGPS_MAX_SATELLITES (20)
```
#### Enable/disable gathering interface statistics:
Uncommenting this define will allow counting the number of sentences and characters received and the number of checksum (CS) errors.  If the CS errors are increasing, you could be losing characters or the connection could be noisy.
```
#define NMEAGPS_STATS
```
#### Enable/disable UTC sub-second resolution

Fixes will be received at approximately 1-second intervals, but there can 
be 30ms *or more* of variance in those intervals.  If you need to know the exact UTC time at *any* time, 
with sub-second accuracy, you must calculate the
offset between the Arduino micros() clock and the UTC time in a received fix.
There are two ways to do this:

**1. When the GPS quiet time ends and the new update interval begins.**

The timestamp will be set when the first character (the '$') of 
the new batch of sentences arrives from the GPS device.  This is fairly
accurate, but it will be delayed from the PPS edge by the GPS device's
fix calculation time (usually ~100us).  There is very little variance
in this calculation time (usually < 30us), so all timestamps are 
delayed by a nearly-constant amount.
```
    #define NMEAGPS_TIMESTAMP_FROM_INTERVAL
```
NOTE:  At update rates higher than 1Hz, the updates may arrive with 
some increasing variance.

**2. From the PPS pin of the GPS module.**

It is up to the application developer to decide how to capture that event.  For example, you could:

a) simply poll for it in loop and call UTCsecondStart(micros());

b) use attachInterrupt to call a Pin Change Interrupt ISR to save 
   the micros() at the time of the interrupt (see NMEAGPS.h), or

c) connect the PPS to an Input Capture pin.  Set the 
   associated TIMER frequency, calculate the elapsed time
   since the PPS edge, and add that to the current micros().
```
    #define NMEAGPS_TIMESTAMP_FROM_PPS
```
#### Enable/Disable derived types
Although normally disabled, this must be enabled if you derive any classes from NMEAGPS.
```
//#define NMEAGPS_DERIVED_TYPES
```

The ublox-specific files require this define (see [ublox](ublox.md) section).
#### Enable/Disable guaranteed comma field separator
Some devices may omit trailing commas at the end of some sentences.  This may prevent the last field from being parsed correctly, because the parser for some types keep the value in an intermediate state until the complete field is received (e.g., parseDDDMM, parseFloat and parseZDA).

Enabling this will inject a simulated comma when the end of a sentence is received and the last field parser indicated that it still needs one.
```
//#define NMEAGPS_COMMA_NEEDED
```
#### Enable/Disable recognizing all sentence types
Some applications may want to recognize a sentence type without actually parsing any of the fields.  Uncommenting this define will allow the `gps.nmeaMessage` member to be set when *any* standard message is seen, even though that message is not enabled by a `#defineNMEAGPS_PARSE_xxx`.  No valid flags will be true for disabled sentences.
```
#define NMEAGPS_RECOGNIZE_ALL
```
#### Enable/Disable parsing scratchpad
Sometimes, a little extra space is needed to parse an intermediate form. This define enables extra space.  Parsers that require the scratchpad can either `#ifdef` an `#error` when the scratchpad is disabled, or let the compiler generate an error when attempting to use the `scratchpad` union (see NMEAGPS.h).
```
//#define NMEAGPS_PARSING_SCRATCHPAD
```

========================
# ublox-specific configuration items

See the [ublox](ublox.md) section.

========================
# Floating-point output.
Streamers.cpp is used by the example programs for printing members of `fix()`.  It is not required for parsing the GPS data stream, and this file may be deleted.  It is an example of checking validity flags and formatting the various members of `fix()` for textual streams (e.g., Serial prints or SD writes).

Streamers.cpp has one configuration item:
```
#define USE_FLOAT
```
This is local to this file, and is only used by the example programs.  This file is _not_ required unless you need to stream one of these types: bool, char, uint8_t, int16_t, uint16_t, int32_t, uint32_t, F() strings, `gps_fix` or `NMEAGPS`.

Most example programs have a choice for displaying fix information once per day.  (Untested!)
```
#define PULSE_PER_DAY
```
========================
# Platforms
The following configuration items are near the top of NeoGPS_cfg.h.
#### Enable/Disable packed bitfields
```
#define NEOGPS_PACKED_DATA
```
8-bit Arduino platforms can address memory by bytes or words.  This allows passing data by reference or 
address, as long as it is one or more bytes in length.  The `gps_fix` class has some members which are 
only one bit; these members cannot be passed by reference or address, only by value.  NeoGPS uses an 
appropriate passing technique, depending on the size of these members.

32-bit Arduino platforms require *all* memory accesses to be 32-bit aligned, which precludes passing 
bitfield, byte, or word members by reference or address.  Rather than penalize the 8-bit platforms with 
unpacked classes and structs, the `NEOGPS_PACKED_DATA` can be disabled on 32-bit platforms.  This 
increases the RAM requirements, but these platforms typically have more available RAM.

========================
# Typical configurations
A few common configurations are defined as follows

**Minimal**: no fix members, no messages (pulse-per-second only)

**DTL**: date, time, lat/lon, GPRMC messsage only.

**Nominal**: date, time, lat/lon, altitude, speed, heading, number of 
satellites, HDOP, GPRMC and GPGGA messages.

**Full**: Nominal plus talker ID, VDOP, PDOP, lat/lon/alt errors, satellite array with satellite info, all messages, and parser statistics.

**PUBX**: Nominal fix items, standard sentences _disabled_, proprietary sentences PUBX 00 and 04 enabled, and required PARSE_MFR_ID and DERIVED_TYPES

These configurations are available in the [configs](/extras/configs) subdirectory.

========================
# Configurations of other libraries

**TinyGPS** uses the **Nominal** configuration + a second `fix`.

**TinyGPSPlus** uses the **Nominal** configuration + statistics + a second fix + timestamps for each `fix` member.

**Adafruit_GPS** uses the **Nominal** configuration + geoid height and IGNORES CHECKSUM!
