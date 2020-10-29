Data Model
==========
Rather than holding onto individual fields, the concept of a **fix** is used to group data members of the GPS acquisition into a C structure (a `struct` type called `gps_fix`).  This also facilitates merging pieces received at different times (i.e., in separate sentences) into a single easy-to-use structure.

The main `NMEAGPS gps;` object you declare in your sketch parses received characters,  gradually assembling a `fix`.  Most programs will call `gps.read()` to obtain the completed fix structure (see [Usage](#Usage) below).

Given a variable declaration of type `gps_fix`:
```
gps_fix fix;
```
...this `fix` variable (or any other variable of type `gps_fix`) contains the following members:
  * `fix.status`, a status code
    * `enum` values STATUS_NONE, STATUS_EST, STATUS_TIME_ONLY, STATUS_STD or STATUS_DGPS
  * a [location](Location.md) structure (i.e., latitude and longitude), accessed with
    * `fix.latitudeL()` and `fix.longitudeL()` for the higher-precision integer degrees, scaled by 10,000,000 (10 significant digits)
    * `fix.latitude()` and `fix.longitude()` for the lower-precision floating-point degrees (~7 significant digits)
    * NOTE: these lat/lon values are
      * positive for North or East degrees and negative for South or West degrees.
      * stored in a 'fix.location' structure, like a 2D coordinate.  The `location_t` class provides additional methods for distance, bearing and offset calculations, as described [here](Location.md).
    * `fix.latitudeDMS` and `fix.latitudeDMS` are structures (see DMS.h) that each contain
      * `fix.longitudeDMS.degrees` in integer degrees
      * `fix.latitudeDMS.degrees`, in integer minutes
      * `fix.longitudeDMS.seconds_whole`, in integer seconds
      * `fix.latitudeDMS.seconds_frac`, in integer thousandths of a second
      * `fix.latitudeDMS.secondsF()`, in floating-point seconds
      * hemisphere indicator, accessed with
        * `fix.longitudeDMS.hemisphere` (enum values NORTH_H, SOUTH_H, EAST_H or WEST_H)
        * `fix.longitudeDMS.EW()` (char values `E` or `W`)
        * `fix.latitudeDMS.NS()` (char values `N` or `S`)
      * NOTE: An integer degree value (scaled by 10<sup>7</sup> can be used to set the DMS structure by using `fix.latitudeDMS.From( otherLatitude );`
  * an altitude (above ellipsoid, aka Mean Sea Level), accessed with
    * `fix.altitude_cm()`, in integer centimeters
    * `fix.altitude()`, in floating-point meters
    * `fix.alt.whole`, in integer meters
    * `fix.alt.frac`, in integer centimeters, to be added to the whole part
  * a speed, accessed with
    * `fix.speed_kph()`, in floating-point kilometers per hour
    * `fix.speed_mph()`, in floating-point miles per hour
    * `fix.speed()`, in floating-point knots (nautical miles per hour)
    * `fix.speed_mkn()`, in integer knots, scaled by 1000
    * `fix.spd.whole`, in integer knots
    * `fix.spd.frac`, in integer thousandths of a knot, to be added to the whole part
  * a heading, accessed with
    * `fix.heading_cd()`, in integer hundredths of a degree
    * `fix.heading()`, in floating-point degrees
  * velocity components in the North, East and Down directions, accessed with
    * `fix.velocity_north`, in integer cm/s
    * `fix.velocity_east`, in integer cm/s
    * `fix.velocity_down`, in integer cm/s
  * `fix.hdop`, `fix.vdop` and `fix.pdop`, in integer thousandths of the DOP.
    * [Dilution of Precision](https://en.wikipedia.org/wiki/Dilution_of_precision_(navigation)) is a unitless measure of the current satellite constellation geometry WRT how 'good' it is for determining a position.  This is _independent_ of signal strength and many other factors that may be internal to the receiver. &nbsp;&nbsp;**It cannot be used to determine position accuracy in meters.**  Instead, use the LAT/LON/ALT error in cm members, which are populated by GST sentences.
  * latitude, longitude and altitude error, accessed with
    * `fix.lat_err_cm`, `fix.lon_err_cm` and `fix.alt_err_cm`, in integer centimeters
    * `fix.lat_err()`, `fix.lon_err()` and `fix.alt_err()`, in floating-point meters
  * speed, heading and time **errors**, accessed with
    * `fix.spd_err_mmps`, in integer mm/s
    * `fix.hdg_errE5`, in integer degrees * 100000
    * `fix.time_err_ns`, in integer nanoseconds<br>**or with**
    * `fix.spd_err()` in floating-point m/s
    * `fix.hdg_err()` in floating-point degrees
    * `fix.time_err()` in floating-point seconds
  * geoid height above ellipsoid (see [here](https://en.wikipedia.org/wiki/Geoid) for description), accessed with
    * `fix.geoidHeight_cm`, in integer centimeters
    * `fix.geoidHeight()`, in floating-point meters
    * `fix.geoidHt.whole`, in integer meters
    * `fix.geoidHt.frac`, in integer centimeters to be added to the whole part
  * `fix.satellites`, a satellite count
  * a date/time structure (see [Time.h](/src/Time.h)), accessed with
    * `fix.dateTime.year`,
    * `fix.dateTime.month`,
    * `fix.dateTime.date`, the day-of-month,
    * `fix.dateTime.hours`,
    * `fix.dateTime.minutes`, 
    * `fix.dateTime.seconds`, and
    * `fix.dateTime.day`, the day-of-the-week. This member is expensive to calculate, so it is *uninitialized* until you call the `set_day()` method. If you need the day-of-the-week, be sure to call `set_day` whenever the `year`, `month` or `date` members are changed.  In general, call `fix.dateTime.set_day()` whenever `fix` is assigned (e.g., `fix = gps.read()`).<br><br>
    `Time` operations allow converting to and from total seconds offset from a *de facto* starting time (e.g., an epoch date/time "origin").  There are constants in Time.h for NTP, POSIX and Y2K epochs.  Simply change the `static` members `s_epoch_year` and `s_epoch_weekday` in Time.h, and all date/time operations will be based on that epoch.  This does not affect GPS times, but it will allow you to easily convert a GPS time to/from an NTP or POSIX time value (seconds).<br><br>
    The [NMEAtimezone.ino](/examples/NMEAtimezone/NMEAtimezone.ino) example program shows how to convert the GPS time (UTC) into a local time.  Basically, a `Time` structure is converted to seconds (from the epoch start), then the time zone offset *in seconds* is added, and then the offset seconds are converted back to a time structure, with corrected day, month, year, hours and minutes members.
  * `fix.dateTime_cs`, in integer hundredths of a second
    * `fix.dateTime_ms()`, in milliseconds
    * `fix.dateTime_us()`, in microseconds
  * a collection of boolean `valid` flags for each of the above members, accessed with
    * `fix.valid.status`
    * `fix.valid.date` for year, month, day-of-month
    * `fix.valid.time` for hours, minutes, seconds and centiseconds
    * `fix.valid.location` for latitude and longitude
    * `fix.valid.altitude`
    * `fix.valid.speed`
    * `fix.valid.heading`
    * `fix.valid.hdop`, `fix.valid.vdop` and `fix.valid.pdop`
    * `fix.valid.lat_err`, `fix.valid.lon_err` and `fix.valid.alt_err`
    * `fix.valid.geoidHeight`

## Validity
Because the GPS device may *not* have a fix, each member of a `gps_fix` can be marked as valid or invalid.  That is, the GPS device may not know the lat/long yet.  To check whether the  fix member has been received, test the corresponding `valid` flag (described above).  For example, to check if lat/long data has been received:
```
  if (my_fix.valid.location) {
    Serial.print( my_fix.latitude() );
    Serial.print( ',' );
    Serial.println( my_fix.longitude() );
  }
```
You should also know that, even though you have enabled a particular member (see [GPSfix_cfg.h](/src/GPSfix_cfg.h)), it **may not have a value** until the related NMEA sentence sets it.  And if you have not enabled that sentence for parsing in `NMEAGPS_cfg.h`, it will **never** be valid.

## Other GPS-related information
There is additional information that is not related to a fix.  Instead, it contains information about parsing or a [**G**lobal **N**avigation **S**atellite **S**ystem](https://en.wikipedia.org/wiki/Satellite_navigation).   GNSS's currently include GPS (US), GLONASS (Russia), Beidou (China) and Galileo (EU). The main `NMEAGPS gps` object you declare in your sketch contains:
  * `gps.UTCsecondStart()`, the Arduino `micros()` value when the current UTC second started
  * `gps.UTCms()`, the number of milliseconds since the last received UTC time, calculated from `micros()` and `gps.UTCsecondStart`.
  * `gps.UTCus()`, the number of microseconds since the last received UTC time, calculated from `micros()` and `gps.UTCsecondStart`.
  * `gps.nmeaMessage`, the latest received message type.  This is an ephemeral value, because multiple sentences are merged into one `fix` structure.  If you only check this after a complete fix is received, you will only see the LAST_SENTENCE_IN_INTERVAL.
    * enum values NMEA_GLL, NMEA_GSA, NMEA_GST, NMEA_GSV, NMEA_RMC, NMEA_VTG or NMEA_ZDA
  * `gps.satellies[]`, an array of satellite-specific information, where each element contains
    * `gps.satellies[i].id`, satellite ID
    * `gps.satellies[i].elevation`, satellite elevation in 0-90 integer degrees
    * `gps.satellies[i].azimuth`, satellite azimuth in 0-359 integer degrees
    * `gps.satellies[i].snr`, satellite signal-to-noise ratio in 0-99 integer dBHz
    * `gps.satellies[i].tracked`, satellite being tracked flag, a boolean
  * `gps.sat_count`, the number of elements in the `gps.satellites[]` array
  * `gps.talker_id[]`, talker ID, a two-character array (not NUL-terminated)
  * `gps.mfr_id[]`, manufacturer ID, a three-character array (not NUL-terminated)
  * an internal fix structure,  `gps.fix()`.  Most sketches **should not** use `gps.fix()` directly!

## Usage
First, declare an instance of `NMEAGPS`:
```
NMEAGPS gps;
```
Next, tell the `gps` object to handle any available characters on the serial port:
```
void loop()
{
  while (gps.available( gps_port )) {
```
The `gps` object will check if there are any characters available, and if so, read them from the port and parse them into its internal fix.  Many characters will have to be read before the current fix is complete, so `gps.available` will return `false` until the fix is complete; the body of `while` loop will be skipped many times, and the rest of `loop()` will be executed.

When a fix is finally completed, `gps.available` will return `true`.  Now your sketch can "read" the completed fix structure from the `gps` object:
```
void loop()
{
  while (gps.available( gps_port )) {
    gps_fix fix = gps.read();
```
The local `fix` variable now contains all the GPS fields that were parsed from the `gps_port`.  You can access them as described above:
```
void loop()
{
  while (gps.available( gps_port )) {
    gps_fix fix = gps.read();
    if (fix.valid.time) {
       ...
```
Note that the `fix` variable is local to that `while` loop; it cannot be accessed elsewhere in your sketch.  If you need to access the fix information elsewhere, you must declare a global fix variable:
```
gps_fix currentFix;

void loop()
{
  while (gps.available( gps_port )) {
    currentFix = gps.read();
    if (currentFix.valid.time) {
       ...
```
Any part of your sketch can use the information in `currentFix`.

Please note that the fix structure is much smaller than the raw character data (sentences).  A fix is nominally 1/4 the size of one sentence (~30 bytes vs ~120 bytes).  If two sentences are sent during each update interval, a fix could be 1/8 the size required for buffering two sentences.

In this fix-oriented program structure, the methods `gps.available` and `gps.read` are manipulating entire `gps_fix` structures.  Multiple characters and sentences are used internally to fill out a single fix: members are "merged" from sentences into one fix structure (described [here](Merging.md)).

That program structure is very similar to the typical serial port reading loop:
```
void loop()
{
  while (serial.available()) {
    char c = serial.read();
       ... do something with the character ...;
  }
```
However, the fix-oriented methods operate on complete *fixes*, not individual characters, fields or sentences.

Note: If you find that you need to filter or merge data with a finer level of control,  you may need to use a different [Merging option](Merging.md), [Coherency](Coherency.md), or the more-advanced [Character-Oriented methods](/doc/CharOriented.md).  

## Examples
Some examples of accessing fix values:
```
gps_fix fix_copy = gps.read();

int32_t lat_10e7 = fix_copy.lat; // scaled integer value of latitude
float lat = fix_copy.latitude(); // float value of latitude

Serial.print( fix_copy.latDMS.degrees );
Serial.print( ' ' );
Serial.print( fix_copy.latDMS.minutes );
Serial.print( F("' " );
Serial.print( fix_copy.latDMS.seconds );

if (fix_copy.dateTime.month == 4) // test for the cruelest month
  cry();

// Count how satellites are being received for each GNSS
for (uint8_t i=0; i < gps.sat_count; i++) {
  if (gps.satellites[i].tracked) {
    if (gps.satellites[i] . id <= 32)
      GPS_satellites++;
    if (gps.satellites[i] . id <= 64)
      SBAS_satellites++;
    if (gps.satellites[i] . id <= 96)
      GLONASS_satellites++;
  }
}
```

And some examples of accessing valid flags in a `fix` structure:
```
if (fix_copy.valid.location)
  // we have a lat/long!

if (fix_copy.valid.time)
  // the copy has hours, minutes and seconds
```
Here's an example for accessing the altitude
```
    if (fix_copy.valid.altitude) {
      z2 = fix_copy.altitude_cm();
      vz = (z2 - z1) / dt;
      z1 = z2;

      // Note: if you only care about meters, you could also do this:
      //    z = fix_copy.alt.whole;
    }
```
You can also check a collection of flags before performing a calculation involving 
multiple members:
```
    if (fix_copy.valid.altitude && fix_copy.valid.date && fix_copy.valid.time) {
      dt = (clock_t) fix_copy.dateTime - (clock_t) fix_copy.dateTime;
      dz = fix_copy.alt.whole - last_alt;  // meters
      vz = dz / dt;                         // meters per second vertical velocity
    }
```
Bonus: The compiler will optimize this into a single bit mask operation.

The example printing utility file, [Streamers.cpp](/src/Streamers.cpp#L100) shows how to access each fix member and print its value.

## Options
Except for `status`, each of these `gps_fix` members is conditionally compiled; any, all, or *no* members can be selected for parsing, storing and merging.  This allows you to configuring NeoGPS to use the minimum amount of RAM for the particular members of interest.  See [Configurations](Configurations.md) for how to edit [GPSfix_cfg.h](/src/GPSfix_cfg.h) and [NMEAGPS_cfg.h](/src/NMEAGPS_cfg.h#L67), respectively.

## Precision
Integers are used for all members, retaining full precision of the original data.
```
    gps_fix fix = gps.read();
    if (fix.valid.location) {
      // 32-bit ints have 10 significant digits, so you can detect very
      // small changes in position:
      d_lat = fix_copy.lat - last_lat;
    }
```

Optional floating-point accessors are provided for many members.
```
    if (fix_copy.valid.location) {
      float lat = fix_copy.latitude();

      // floats only have about 6 significant digits, so this
      // computation is useless for detecting small movements:
      foobar = (lat - target_lat);
```
