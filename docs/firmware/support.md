# Hardware Support
Over time, the operating system gained additional hardware support for non open-smartwatch environments. This page lists the supported hardware and how to use it (as well as unwanted features, so called "bugs"). You can also find all these configurations inside the `platformio.ini` file.

## Official Hardware
...based on hardware schematics released by the OSW team and tested frequently.

| Hardware                               | Status       | Reference                                                 |
| -------------------------------------- | ------------ | --------------------------------------------------------- |
| `LIGHT_EDITION_V3_3`                   | Maintained   | See "Hardware"-Documentation section                      |
| `EXPERIMENTAL_LIGHT_EDITION_V4_0`      | Experimental | See "Hardware"-Documentation section                      |
| `EXPERIMENTAL_LIGHT_EDITION_V3_3_LUA`  | Experimental | Same as `LIGHT_EDITION_V3_3`, but with LUA-script-support |
| `EXPERIMENTAL_GPS_EDITION_V3_1`        | Experimental | See "Hardware"-Documentation section                      |
| `EXPERIMENTAL_GPS_EDITION_DEV_ROTATED` | Experimental | See "Hardware"-Documentation section                      |

## 3rd Party Hardware

| Hardware                       | Status       | Reference                         | Known Issues                                                            |
| ------------------------------ | ------------ | --------------------------------- | ----------------------------------------------------------------------- |
| `3RD_PARTY_FLOW3R_C3CAMP_2023` | Experimental | [website](https://flow3r.garden/) | [PR368](https://github.com/Open-Smartwatch/open-smartwatch-os/pull/368) |
