Location
========
The `Location_t` class is a 2D point, containing a latitude and longitude in integer degrees * 10<sup>7</sup> (source [here](/src/Location.h)).

This class also provides geographic distance, bearing and offset functions.  Furthermore, they all take advantage of the increased precision of the integer coordinates.  Other libraries use floating-point coordinates, which have only 6 or 7 significant digits.  By using integer math, calculations maintain their original accuracy as long as possible.  For example, small distances can be calculated to millimeter accuracy.

The example program [NMEAaverage.ino](/examples/NMEAaverage/NMEAaverage.ino) shows several techniques for performing 2D calculations.

### Distance

To calculate the distance between a pre-determined point and the current fix,

```
NeoGPS::Location_t madrid( 404381311L, -38196229L ); // see https://www.google.com/maps/@40.4381311,-3.8196229,6z
gps_fix fix;

void loop()
{
  while (gps.available( gps_port )) {
    fix = gps.read();
    float dist = fix.location.DistanceKm( madrid );
    // or dist = NeoGPS::Location_t::DistanceKm( fix.location, madrid );
    Serial.print( dist );
    Serial.println( F(" km") );
```

`DistanceMiles` is also available

### Bearing

To calculate the bearing from one point to another (in radians, CW from North),

```
    float bearing = fix.location.BearingToDegrees( madrid );
    // or bearing = NeoGPS::Location_t::BearingToDegrees( fix.location, madrid );
```
Radians is returned by `BearingTo`.

### Offsetting a Location

To move a location by a specified distance, in a specified direction,

```
    float bearing = fix.location.BearingToDegrees( madrid );
    // or bearing = NeoGPS::Location_t::BearingToDegrees( fix.location, madrid );
    
    // Step 10km closer to the destination
    Location_t next_stop( fix.location );
    next_stop.OffsetBy( bearing, 10 / NeoGPS::Location_t::EARTH_RADIUS_KM );
```
Notice that the distance is specified in *radians*.  To convert from km to radians, divide by the Earth's radius in km.  To convert from miles, divide the miles by the Earth's radius in miles.

### NeoGPS namespace
Because the `Location_t` is inside the `NeoGPS` namespace, any time you want to declare your own instance, use any of the constants in that class (anything that requires the `Location_t` name), you must prefix it with `NeoGPS::` (shown above).  As with any C++ namespace, you can relax that requirement by putting this statement anywhere after the NeoGPS includes:

```
using namespace NeoGPS;
```

This technique is used in the **NMEAaverage.ino** sketch.

However, if you have any other libraries that declare their own `Location_t` (not likely), you could not use the `using` statement.  `Time_t` is inside the `NeoGPS` namespace for the same reason: avoiding name collisions.
