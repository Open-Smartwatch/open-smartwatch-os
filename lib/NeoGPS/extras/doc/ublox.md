NeoGPS support for u-blox GPS devices
=================
To use either ublox-specific NMEA messages ($PUBX) or the UBX binary protocol, you must enable the following in `NMEAGPS_cfg.h`:
```
#define NMEAGPS_PARSE_MFR_ID

#define NMEAGPS_DERIVED_TYPES
```

## ublox-specific NMEA messages
**NeoGPS** implements the following additional NMEA messages:

# NMEA 0183 Proprietary Messages
* UBX,00 - Lat/Long Position Data
* UBX,04 - Time of Day and Clock Information

You may want to change the configured PUBX messages in `PUBX_cfg.h`.  It is currently configured to work with the example application, `PUBX.ino`.  `PUBX_cfg.h` has the following configuration items:
```
#define NMEAGPS_PARSE_PUBX_00
#define NMEAGPS_PARSE_PUBX_04
```

## ublox-specific binary protocol

**NeoGPS** implements the following messages in the UBX binary protocol:

# UBX Protocol Messages

<table>
  <tr><td><b>Message</b></td><td><b>Description</b></td><td><b>NEO series</b></td></tr>
  <tr><td>NAV_STATUS</td><td>Receiver Navigation Status</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_TIMEGPS</td><td>GPS Time Solution</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_TIMEUTC</td><td>UTC Time Solution</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_POSLLH</td><td>Geodetic Position Solution</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_DOP</td><td>Dilutions of precision</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_PVT</td><td>Navigation Position Velocity Time Solution</td><td><p align="center">7</p></td></tr>
  <tr><td>NAV_VELNED</td><td>Velocity Solution in NED (North/East/Down)</td><td><p align="center">6</p></td></tr>
  <tr><td>NAV_SVINFO</td><td>Space Vehicle Information</td><td><p align="center">6</p></td></tr>
  <tr><td>HNR_PVT</td><td>High Rate Output of PVT Solution</td><td><p align="center">8</p></td></tr>
</table>

You may want to change the configured UBX messages in `ubx_cfg.h`.  It is currently configured to work with the example application `ublox.ino`.

The configuration file `ubx_cfg.h` has the following configuration items near the top of the file:
```
#define UBLOX_PARSE_STATUS
#define UBLOX_PARSE_TIMEGPS
#define UBLOX_PARSE_TIMEUTC
#define UBLOX_PARSE_POSLLH
//#define UBLOX_PARSE_DOP
#define UBLOX_PARSE_VELNED
//#define UBLOX_PARSE_PVT
#define UBLOX_PARSE_SVINFO
//#define UBLOX_PARSE_HNR_PVT
```

**Note:** Disabling some of the UBX messages may prevent the `ublox.ino` example sketch from working.  That sketch goes through a process of first acquiring the current GPS leap seconds and UTC time so that "time-of-week" milliseconds can be converted to a UTC time.

The POSLLH and VELNED messages use a Time-Of-Week timestamp.  Without the TIMEGPS and TIMEUTC messages, that TOW timestamp cannot be converted to a UTC time.

* If your application does not need the UTC date and/or time, you could disable the TIMEGPS and TIMEUTC messages.

* If your application does not need latitude, longitude or altitude, you could disable the POSLLH message.

* If your application does not need speed or heading, you could disable the VELNED message.

* If your application does not need satellite information, you could disable the SVINFO message.
