Merging
===========
Because different NMEA sentences contain different pieces of a fix, they have to be "merged" to determine a complete picture.  Some sentences contain only date and time.  Others contain location and altitude, but not speed and heading (see table [here](Choosing.md)).

There are several ways to use the GPS fix data: without merging, implicit merging, and **explicit merging ([the default](#3-explicit-merging))**.  NeoGPS allows you to choose how you want multiple sentences to be merged:

### 1. NO MERGING

In this mode, `gps.read()` will return a fix that is populated from just one sentence.  You will probably receive multiple sentences per update interval, depending on your GPS device.  

If you are interested in just a few pieces of information, and those pieces can be obtained from one or two sentences, you can wait for that specific sentence to arrive, and then use one or more members of the fix at that time.  Make sure that "no merging" is selected in NMEAGPS_cfg.h by commenting out these lines:
```
//#define NMEAGPS_EXPLICIT_MERGING
//#define NMEAGPS_IMPLICIT_MERGING
```
Then write your loop to wait for the specific sentence:
```
void loop()
{
  while (gps.available( gps_port )) {
    const gps_fix & rmc = gps.read();
 
    if (gps.nmeaMessage == NMEAGPS::NMEA_RMC) {
      // All GPS-related work is performed inside this if statement

      if (rmc.valid.speed && (rmc.speed <= 5))
        Serial.println( F("Too slow!") );
        
      if (rmc.valid.location &&  ... or any rmc member
    }
  }
  
  // Can't access rmc out here...
```
If you are interested in pieces of information that are grouped by some detailed criteria (e.g., field values), you must select "no merging" and then manually merge the fixes of interest.  The `merged` copy will be safe to access at any time:
```
gps_fix merged;

void loop()
{
  while (gps.available( gps_port )) {
    const gps_fix & fix = gps.read();
 
    if ((gps.nmeaMessage == NMEAGPS::NMEA_RMC) &&
        (fix.valid.heading &&
          ((4500 <= fix.heading_cd()) && (fix.heading_cd() < 9000)))){

      merged |= fix;

      if (merged.valid.satellites && (rmc.satellites > 5))
        Serial.println( F("Moar sats!") );
    }
  }
  
  // Can't access 'fix' out here, but 'merged' can be used...
```
### 2. IMPLICIT MERGING
If you are interested in more pieces of information, perhaps requiring more kinds of sentences to be decoded, but don't really care about what time the pieces were received, you could enable implicit merging:
```
//#define NMEAGPS_EXPLICIT_MERGING
#define NMEAGPS_IMPLICIT_MERGING
```
As sentences are received, they are accumulated internally.  Previous field values are retained until they are overwritten by another sentence.  When `gps.available`, the accumulated fix can be obtained with `gps.read()`.

Note: The members in an implicitly-merged fix may not be coherent (see [Coherency](Coherency.md).  Also, checksum errors can cause the internal fix to be completely reset.  Be sure your sketch checks the [valid flags](Data%20Model.md#validity) before using any fix data.

### 3. EXPLICIT MERGING
This is the default setting.  To enable explicit merging, make sure this is in NMEAGPS_cfg.h:
```
#define NMEAGPS_EXPLICIT_MERGING
//#define NMEAGPS_IMPLICIT_MERGING

#define NMEAGPS_FIX_MAX 1
```
NMEAGPS_FIX_MAX must be at least one to use EXPLICIT merging.

As sentences are received, they are merged internally.  When the LAST_SENTENCE_IN_INTERVAL is received, the internal fix is marked as "available", and it can be accessed by calling `gps.read()`.  When the next sentence arrives, the internal fix structure will be emptied.

```
gps_fix_t currentFix;

void loop()
{
  while (gps.available( gps_port ))
    currentFix = gps.read();
 
  check_position( currentFix );
```

Explicit merging is also required to implement [Coherency](Coherency.md).
