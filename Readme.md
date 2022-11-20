# HowTo Run

**Please read the documentation on the [website](https://open-smartwatch.github.io/resources/firmware/)!**

## Prerequirements

1. Install [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) or [PlatformIO IDE](https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode) (which installs PlatformIO core automatically)
2. **For improved LUA Script support, see env:pico32_LIGHT_EDITION_PREBUILD_LUA**: install [SWIG](http://www.swig.org/Doc4.0/SWIGDocumentation.html#Preface_installation) (also available in most package managers, e.g. `brew install swig`)
3. Then clone this repository:
    ```bash
    $ git clone --recurse-submodules https://github.com/Open-Smartwatch/open-smartwatch-os.git
    ```
    **As this repository contains binary data (e.g. schematics or images), make sure to have `git-lfs` installed!** Cloning this repository without `git-lfs` will result in missing or broken files.

## Build
<p align="center">
  <a href="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-OSW.yml"><img alt="OSW-OS-test" src="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-OSW.yml/badge.svg"></a>
  <a href="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-emulator.yml"><img alt="OSW-EMULATOR-test" src="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-emulator.yml/badge.svg"></a>
  <a href="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-FEATURE.yml"><img alt="OSW-FEATURE-test" src="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/test-FEATURE.yml/badge.svg"></a>
  <a href="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/astyle.yml"><img alt="auto-format" src="https://github.com/Open-Smartwatch/open-smartwatch-os/actions/workflows/astyle.yml/badge.svg"></a>
</p>

The `master` branch is a stable version and the `develop` branch is a beta version. Recommended that you upload the `master` branch.

### Visual Studio Code

Open the cloned repo in VSCode:

```
$ code open-smartwatch-os
```

And rename file `include/config.h.example` to `include/config.h` and adapt the values according to your requirements and press `F5` to build it.

### CLI

You can instead go to the repo folder with your terminal and run

```
$ pio run -e pico32_LIGHT_EDITION -t upload
```

respectively

```
$ pio run -e pico32_GPS_EDITION -t upload
```

Depending on the watch model.

## Debugging (CLI)

If you want to print out the log for debugging, following command:

```
$ pio device monitor
```

## Creating Screenshots of your Apps

<p align="center">
    <img src="./screenshots/watchface_analog_osw.png" width="40%">
    <img src="./screenshots/watchface_digital_osw.png" width="40%">
</p>

* Wi-Fi needs to be able to connect for this to work.
* you will need bash and ImageMagick for the helper scripts to work

The raw screenserver runs in the background and prints via serial:

```
Started Raw ScreenServer
http://<IP_OF_WATCH>/api/screenserver
```

### HowTo

 * add `-D RAW_SCREEN_SERVER` to your build flags in `platformio.ini`
 * build + flash + reset watch
 * wait for the server to be started (see msg above)
 * run `bash fetchScreen.sh <IP_OF_WATCH> screenshot.png`
 * run `bash composeScreen.sh screenshot.png screenshot_composed.png`

If there is no `curl`, it must be installed.
```
$ apt install curl -y
```
The `fetchScreen.sh` downloads the raw image buffer from the running screen server, and converts the image to png. The `composeScreen.sh` creates the image with a surrounding smartwatch (light edition).

#### The fast way (recommended)

Run the following inside the `open-smartwatch-os` directory:

```
$ cd scripts/screen_capture/
$ ./createScreenshot.sh <IP_OF_WATCH> <SCREENSHOT>
```
* The captured file can be found in the `screenshot/` folder inside the `open-smartwatch-os` directory.
## Troubleshooting

For more information on troubleshooting, see [Wiki](https://open-smartwatch.github.io/resources/firmware/#troubleshooting).

### Arduino_TFT.h: No such file or directory

You did not clone the repository with the `--recursive-submodules` flag.

### 'LANG_STW_START' was not declared in this scope

You did not rename `include/config.h.example`

### Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst platform.io was trying to connect.

## OSW Emulator
![emulator](screenshots/emulator_demo.png)

The OS itself can be executed as a regular program on your machine. This saves you time compiling for the watch and flashing the OS, every time you make a minor change - e.g. while developing the UI or a game, which not explicitly depend on the hardware of the watch.

This also implies some limitations what you can do with the emulator, as we had to hack and reimplement some of the Arduino-specific libraries and their (conflicting) simplifications. This also means, that it maybe necessary to extend those extensions down the road as we (likely) missed that one specific function you try to use... :wink:

### Build (cmake)
The emulator can be build using the `CMakeLists.txt` file - you may need to install additional libraries to be able to use it.

Here is a small example running on "Ubuntu 22.04 LTS":
```bash
$ sudo apt install libsdl2-dev libsdl2-image-dev g++ gcc cmake make build-essential
$ mkdir build && cd build
$ cmake ..
$ make -j $(nproc)
$ ./emulator.run
```

You also may extend the `cmake`-command with `-DCMAKE_BUILD_TYPE=Release` to get an even faster and smaller binary.

### With Docker
If a library is unavailable, you can still use the emulator using docker (e.g. on Ubuntu 20.04 SDL2 is too old). Proceed with a typical docker installation. Showing an application running in docker requires some additional steps:

#### Host PC
*Tested on Ubuntu 20.04.*
```bash
$ xhost +
$ xauth list # Copy the result of the command. 
$ docker run --net=host -e DISPLAY -v /tmp/.X11-unix -d --name OSW -p 22:22 -it --privileged ubuntu:22.04
```

#### Docker
```bash
$ xauth add <'xauth list' command result>
```

## License
Everything in this repository is under a GPL-3.0 license, see [here](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/LICENSE) for more details.
