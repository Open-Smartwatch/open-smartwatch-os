# Getting Started

This page describes which software you'll need and how to manually flash the firmware.

!!! note "Tip"
    In case you won't or can't compile the (latest) firmware yourself, you can download the latest pre-compiled firmware from the [releases page](https://github.com/Open-Smartwatch/open-smartwatch-os/releases) (in case we did not forget to upload it there). In case you want to take a look into what is coming with the next release, checkout the [GitHub Actions](https://github.com/Open-Smartwatch/open-smartwatch-os/actions) (you have to login to download anything). To then flash the firmware to the watch use something like the [ESPHome Flasher](https://github.com/esphome/esphome-flasher), be aware that this flasher will also erase the watches memory every time! An alternative (although not tested) would be the [NodeMCU Flasher](https://github.com/marcelstoer/nodemcu-pyflasher).

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

!!! note "Tip"
    Most Linux distributions already have the drivers shipped with the kernel, so you don't need to install them manually. In newer Ubuntu versions you may need to uninstall the `brltty` package to get the serial port working.

The Open-Smartwatch uses a `WCH340E` USB to serial controller. If your device is not detected as `COM?` or `/dev/cu.usbserial-?` or similar, please install the drivers from the manufacturer: [http://www.wch-ic.com/downloads/CH341SER_ZIP.html](http://www.wch-ic.com/downloads/CH341SER_ZIP.html)

Please also see the trouble shooting section below if your device is not connected.

## Cloning the Repository

Open git bash in your desired folder and clone the source code repository **recursively**:

    git clone --recurse-submodules https://github.com/Open-Smartwatch/open-smartwatch-os.git

If you have cloned the repo without the recurse option, run `git submodule update --init`.

!!! note "Tip"
    After changing or updating/pulling a branch, run the commands notes inside the `README.md` to update the submodules and the PlatformIO packages.

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
