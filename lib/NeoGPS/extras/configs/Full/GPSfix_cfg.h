#ifndef GPS_FIX_CFG
#define GPS_FIX_CFG

/**
 * Enable/disable the storage for the members of a fix.
 *
 * Disabling a member prevents it from being parsed from a received message.
 * The disabled member cannot be accessed or stored, and its validity flag 
 * would not be available.  It will not be declared, and code that uses that
 * member will not compile.
 *
 * DATE and TIME are somewhat coupled in that they share a single `time_t`,
 * but they have separate validity flags.
 *
 * See also note regarding the DOP members, below.
 *
 */

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

#endif
