# Garmin Proprietary messages

To use the Garmin Proprietary NMEA message parser, you must enable the following in `NMEAGPS_cfg.h`:
```
#define NMEAGPS_PARSE_MFR_ID
#define NMEAGPS_DERIVED_TYPES
```

**NeoGPS** implements the following additional NMEA messages:

* $PGRMF - Fix data

You may want to change the configured PGRM messages in `src/Garmin/PGRM_cfg.h`.  It is currently configured to work with the example application, `PGRM.ino`.  `PGRM_cfg.h` has the following configuration items:
```
#define GARMINGPS_PARSE_PGRMF
```

As shown in the PGRM.ino example program, you should use an instance of `GarminNMEA` instead of the typical `NMEAGPS`:

```
#include <NeoGPS_cfg.h>
#include <Garmin/GrmNMEA.h>
#include <GPSTime.h>

   ...

static GarminNMEA gps; // This parses received characters
static gps_fix    fix;
```

A few different header files are included, but all other operations are identical.

Notice that the $PGRMF message sets the global variable, `GPSTime::leap_seconds`.  As reported in 
[Issue #90](https://github.com/SlashDevin/NeoGPS/issues/90), this is required for exact UTC 
calculations that span dates with different GPS leap seconds 
(see  [Wikipedia article](http://en.wikipedia.org/wiki/Global_Positioning_System#Leap_seconds)).