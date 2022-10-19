# OSW-OS

This page describes which software you'll need and how to manually flash the firmware.

## Required Software

- [GIT](https://git-scm.com)
- [Visual Studio Code](https://code.visualstudio.com)
- [VSC PlatformIO Extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
- [WCH340 Serial Drivers](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

#### Git

Install `git` from [https://git-scm.com](https://git-scm.com).

Make sure to enable the "Add the Git Bash profile to Windows terminal" during the GIT installation process.

The following website will guide you through this step: [https://linuxhint.com/add-git-bash-windows-terminal/](https://linuxhint.com/add-git-bash-windows-terminal/)

#### Visual Studio Code

To flash the latest software to the open-smartwatch you will need to install VScode with PlatformIO.

The following Youtube tutorial will guide you through this step: [https://youtu.be/JmvMvIphMnY](https://youtu.be/JmvMvIphMnY).

#### USB Serial Drivers for WCH340E

The Open-Smartwatch uses a `WCH340E` USB to serial controller. If your device is not detected as `COM?` or `/dev/cu.usbserial-?` or similar, please install the drivers from the manufacturer: [http://www.wch-ic.com/downloads/CH341SER_ZIP.html](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

Please also see the trouble shooting section below if your device is not connected.

## Cloning the Repository

Open git bash in your desired folder and clone the source code repository **recursively**:

    git clone --recurse-submodules https://github.com/Open-Smartwatch/open-smartwatch-os.git

If you have cloned the repo without the recurse option, run `git submodule update`.

!!! note "Tip"
    After changing or updating/pulling a branch, run the command again to also update dependencies:
    `git submodule update`

Then, open the directory with Visual Studio Code.

    code open-smartwatch-os

## Seting up config.h

Got to the directory `include/` and take a look into the `config.h.example` - all instructions are noted there.
You will have to copy the `config.h.example` to `config.h`. Then you can modify the parameters to preconfigure your settings:

- `CONFIG_WIFI_SSID` to set your WiFi SSID
- `CONFIG_WIFI_PASS` to set your WiFi password
- `LOCALE` to configure your locale, see `include/locales` for options
- `DEVICE_NAME` to set your device name (BT name, hostname)
- `CONFIG_TIMEZONE` to set your timezone
- `CONFIG_DAYLIGHTOFFSET` to set your daylight saving time offset
- `CONFIG_DATE_FORMAT` to set your calendar format, options: `mm/dd/yyyy`, `dd.mm.yyyy`, `yy.mm/dd`

## Uploading


To set the watch into upload mode, you need to hold the **lower left** button and then click the reset button (**top left**). This enables flash mode, the display will turn dark.
Press the reset button after uploading to reboot into the new firmware.

The following video shows you how it's done: [https://www.youtube.com/watch?v=p-mDOIUr-rQ](https://www.youtube.com/watch?v=p-mDOIUr-rQ)

## GPIO

Generally, you won't need to worry about GPIO assignment and initialisation as this is done by the OSW-OS. For reference see the image below.

**Keep in mind the PCB is flipped when inserted into the watch, EN/Reset is at the top left in this case:**

<img src="/assets/uploading.jpg" width="512px"/>


## More Developer Infos

Also check the latest readme file within the software repository for more developer options: [https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/Readme.md](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/Readme.md)

### Feature Flags
The table below list all currently available features of the OSW-OS. These features can be manually enabled (or disabled) by modifying the `platformio.ini` and adding (or removing) their `-D`-Define lines.

Flag | Description | Requirements
----------- | ----------- | -----------
`OSW_FEATURE_STATS_STEPS` | Enable step history (displayed on the watchfaces) | -
`OSW_FEATURE_WIFI` | Enable all wifi related functions (services, webinterface) | -
`OSW_FEATURE_WIFI_APST` | Allow the watch to enable wifi client and station simultaneously | `OSW_FEATURE_WIFI`
`OSW_FEATURE_WIFI_ONBOOT` | Allow the user to enable the wifi on boot | `OSW_FEATURE_WIFI`
`OSW_FEATURE_LUA` | Enable LUA scripting support for apps | `LUA_C89_NUMBERS`
`LUA_C89_NUMBERS` | Needed when compiling with LUA support | -
`DEBUG=1` | Enables debug logging to the console & additional utilities | -
`GPS_EDITION` | Configure the build for use with GPS (including apps, api, sensors) | `PROGMEM_TILES`, `BOARD_HAS_PSRAM`
`PROGMEM_TILES` | Needed when compiling with GPS support | -
`BOARD_HAS_PSRAM` | Needed when compiling with GPS support | -
`GPS_EDITION_ROTATED` | Replacement for `GPS_EDITION` to work with flipped boards | -

#### Experimental Flags

This flag is available on all models.

Flag | Description | Requirements
----------- | ----------- | -----------
- `RAW_SCREEN_SERVER` | Capture the watchface and save it as a `*.png` file. | `OSW_FEATURE_WIFI`
- `ANIMATION` | Animation can be used as the background of the watchface. | -
- `OSW_FEATURE_BLE_MEDIA_CTRL` | See `OswAppBLEMediaCtrl.cpp` a tech demo to use the OSW as an external keyboard. | OSW Light `v3.x` has insufficient memory, <br>`OswHal::getInstance()->disableDisplayBuffer()` is called to free memory <br>but slows down redraw speeds significantly.
- `OSW_FEATURE_WEATHER` | You can monitor the weather through an OpenWeatherAPI. | `OSW_FEATURE_WIFI`

#### Supported Flags per Device
The table below lists which features are available in which version of the OS by default. It is always our goal to also support older hardware revisions, but not all features can run properly using the old schematics.

Flag | `LIGHT_EDITION_V4_0` | `LIGHT_EDITION_V3_3` | `LIGHT_EDITION_V3_2` | `LIGHT_EDITION_DEV_LUA` | `GPS_EDITION_V3_1` | `GPS_EDITION_DEV_ROTATED`
----------- | ----------- | ----------- | ----------- | ----------- | ----------- | -----------
`OSW_FEATURE_STATS_STEPS` | ✓ | ✓ | ✓ | ❌ | ✓ | ✓
`OSW_FEATURE_WIFI` | ✓ | ✓ | ✓ | ❌ | ✓ | ✓
`OSW_FEATURE_WIFI_APST` | ❌ | ❌ | ❌ | ❌ | ✓ | ✓
`OSW_FEATURE_WIFI_ONBOOT` | ✓ | ❌ | ❌ | ❌ | ✓ | ✓
`OSW_FEATURE_LUA` | ❌ | ❌ | ❌ | ✓ | ❌ | ❌

## Troubleshooting

#### Always double check the cable

We have seen a number of "charge only" cables in the community. Try to upload using a different cable.

#### Check the driver

Check the driver insertion.

#### Arduino_TFT.h: No such file or directory

You did not clone the repository with the `--recursive-submodules` flag.

!!! note "Tip"
    After changing the branch, follow the command :
    `git submodule update`

#### 'LANG_STW_START' was not declared in this scope

You did not rename `include/config.h.example`

#### Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst platform.io was trying to connect.

#### Fixing `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`

On Fedora Silverblue 34 `make` outputs `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`.
This can be fixed by using [this fork from juliagoda](https://github.com/juliagoda/CH341SER).
