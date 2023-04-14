# OSW-OS

This page describes which software you'll need and how to manually flash the firmware.

## Required Software

- [GIT](https://git-scm.com)
- [Visual Studio Code](https://code.visualstudio.com)
- [VSC PlatformIO Extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
- [WCH340 Serial Drivers](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

### Git

Install `git` from [https://git-scm.com](https://git-scm.com).

Make sure to enable the "Add the Git Bash profile to Windows terminal" during the GIT installation process.

The following website will guide you through this step: [https://linuxhint.com/add-git-bash-windows-terminal/](https://linuxhint.com/add-git-bash-windows-terminal/)

### Visual Studio Code

To flash the latest software to the open-smartwatch you will need to install VScode with PlatformIO.

The following Youtube tutorial will guide you through this step: [https://youtu.be/JmvMvIphMnY](https://youtu.be/JmvMvIphMnY).

### USB Serial Drivers for WCH340E

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

## Setting up config.h overrides

By default you are *not required anymore* to create your own `config.h` file. The default configuration is sufficient for most users. If you still want to modify the initial device configuration, you can create a `config.h` file (just rename the `config.h.example`) inside the `include` directory. This file will be included in the build process and override the default configuration. If you are not sure which settings are supported with which values, take a look into the `config_defaults.h` file.

## Uploading

To set the watch into upload mode, you need to hold the **lower left** button and then click the reset button (**top left**). This enables flash mode, the display will turn dark.
Press the reset button after uploading to reboot into the new firmware.

The following video shows you how it's done: [https://www.youtube.com/watch?v=p-mDOIUr-rQ](https://www.youtube.com/watch?v=p-mDOIUr-rQ)

## GPIO

Generally, you won't need to worry about GPIO assignment and initialisation as this is done by the OSW-OS. For reference see the image below.

**Keep in mind the PCB is flipped when inserted into the watch, EN/Reset is at the top left in this case:**

<img src="/assets/uploading.jpg" width="512px"/>

## Feature Flags
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

### Experimental Flags

These flags should be available on all models. Because they are experimental, they are not enabled by default any may won't work or even compile.

Flag | Description | Requirements
----------- | ----------- | -----------
- `RAW_SCREEN_SERVER` | Capture the watchface and save it as a `*.png` file. | `OSW_FEATURE_WIFI`
- `ANIMATION` | Animation can be used as the background of the watchface. | -
- `OSW_FEATURE_BLE_MEDIA_CTRL` | See `OswAppBLEMediaCtrl.cpp` a tech demo to use the OSW as an external keyboard. | OSW Light `v3.x` has insufficient memory, <br>`OswHal::getInstance()->disableDisplayBuffer()` is called to free memory <br>but slows down redraw speeds significantly.
- `OSW_FEATURE_WEATHER` | You can monitor the weather through an OpenWeatherAPI. | `OSW_FEATURE_WIFI`
- `GIF_BG` | Enable GIF support for the background of some watchfaces. | -

### Supported Flags per Device
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

#### Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst platform.io was trying to connect.

#### Fixing `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`

On Fedora Silverblue 34 `make` outputs `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`.
This can be fixed by using [this fork from juliagoda](https://github.com/juliagoda/CH341SER).
