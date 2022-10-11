# What is this?

These are the platform-specifc configurations, which specify e.g. which sensors are available and on what pin is which device / sensor / whatever. These files are also used to specify e.g. workarounds on problematic hardware-releases. These headers are intended to only reflect hardware-specific rules, which should not be overriden by the normal user using the `platformio.ini` files, as they commonly also require modification of the hardware itself or describe board-specific stuff.

If you wish to extend the `define`-s in those files, please make sure to port your changes to all the other files (platforms) as well.

If you want to port the OSW-OS to an other platform, these files are probably the first thing you want to look at. :)