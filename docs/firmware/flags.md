# Feature Flags
The table below list all currently available features of the OSW-OS. These features can be manually enabled (or disabled) by modifying the `platformio.ini` and adding (or removing) their `-D`-Define lines.

| Flag                         | Description                                                                          | Requirements                       |
| ---------------------------- | ------------------------------------------------------------------------------------ | ---------------------------------- |
| `OSW_FEATURE_STATS_STEPS`    | Enable step history (displayed on the watchfaces)                                    | -                                  |
| `OSW_FEATURE_WIFI`           | Enable all wifi related functions (services, webinterface)                           | -                                  |
| `OSW_FEATURE_WIFI_ONBOOT`    | Allow the user to enable the wifi on boot                                            | `OSW_FEATURE_WIFI`                 |
| `OSW_FEATURE_BLE_MEDIA_CTRL` | Enables media control via BLE                                                        | -                                  |
| `OSW_FEATURE_LUA`            | Enable LUA scripting support for apps                                                | `LUA_C89_NUMBERS`                  |
| `SERVICE_BLE_COMPANION=1`    | Enables the BLE Companion Service (unstable, requires custom smartphone application) | -                                  |
| `DEBUG=1`                    | Enables debug logging to the console & additional utilities                          | -                                  |
| `GPS_EDITION`                | Configure the build for use with GPS (including apps, api, sensors)                  | `PROGMEM_TILES`, `BOARD_HAS_PSRAM` |
| `GPS_EDITION_ROTATED`        | Replacement for `GPS_EDITION` to work with flipped boards                            | -                                  |

## Example Flags

You want to know how to use some example code or see it in action? These flags enable various features that are not enabled by default - just search inside the source code for the flag to see how it works and what it does.

| Flag                  | Description                                                           | Requirements |
| --------------------- | --------------------------------------------------------------------- | ------------ |
| `OSW_SERVICE_EXAMPLE` | Enable the example code to demonstrate how to write on services.      | -            |
| `OSW_APPS_EXAMPLES`   | Enable the example code to demonstrate how to write own apps (v1/v2). | -            |

## Experimental Flags

These flags should be available on all models. Because they are experimental, they are not enabled by default any may won't work or even compile.

| Flag                         | Description                                                                                                                                                                                                                                             | Requirements       |
| ---------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ------------------ |
| `RAW_SCREEN_SERVER`          | Capture the watchface and save it as a `*.png` file.                                                                                                                                                                                                    | `OSW_FEATURE_WIFI` |
| `ANIMATION`                  | Animation can be used as the background of the watchface.                                                                                                                                                                                               | -                  |
| `OSW_FEATURE_BLE_MEDIA_CTRL` | See `OswAppBLEMediaCtrl.cpp` a tech demo to use the OSW as an external keyboard. OSW Light `v3.x` has insufficient memory, <br>`OswHal::getInstance()->disableDisplayBuffer()` is called to free memory <br>but slows down redraw speeds significantly. | -                  |
| `OSW_FEATURE_WEATHER`        | You can monitor the weather through an OpenWeatherAPI.                                                                                                                                                                                                  | `OSW_FEATURE_WIFI` |
| `GIF_BG`                     | Enable GIF support for the background of some watchfaces.                                                                                                                                                                                               | -                  |

## Supported Flags per Device
The table below lists which features are available in which version of the OS by default. It is always our goal to also support older hardware revisions, but not all features can run properly using the old schematics.

| Flag                      | `LIGHT_EDITION_V4_0` | `LIGHT_EDITION_V3_3` | `LIGHT_EDITION_DEV_LUA` | `GPS_EDITION_V3_1` | `GPS_EDITION_DEV_ROTATED` |
| ------------------------- | -------------------- | -------------------- | ----------------------- | ------------------ | ------------------------- |
| `OSW_FEATURE_STATS_STEPS` | ✓                    | ✓                    | ❌                       | ✓                  | ✓                         |
| `OSW_FEATURE_WIFI`        | ✓                    | ✓                    | ❌                       | ✓                  | ✓                         |
| `OSW_FEATURE_WIFI_APST`   | ❌                    | ❌                    | ❌                       | ✓                  | ✓                         |
| `OSW_FEATURE_WIFI_ONBOOT` | ✓                    | ❌                    | ❌                       | ✓                  | ✓                         |
| `OSW_FEATURE_LUA`         | ❌                    | ❌                    | ✓                       | ❌                  | ❌                         |
