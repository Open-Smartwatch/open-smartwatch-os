# Troubleshooting

## Build problems
...as we have some automations setup to always test-build many different configurations, we are quite sure that the code is working out of the box. If you have problems, maybe the following hints can help you. Or you found a bug... please let us know!

### Fixing `fatal error: Arduino_TFT.h: No such file or directory`

You did not clone the repository with the `--recursive-submodules` flag.

!!! note "Tip"
    After changing or updating/pulling a branch, run the commands notes inside the `README.md` to update the submodules and the PlatformIO packages.

### Fixing `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`

On Fedora Silverblue 34 `make` outputs `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`.
This can be fixed by using [this fork from juliagoda](https://github.com/juliagoda/CH341SER).

### Fixing `raise UnidentifiedImageError(msg)`

Open-Smartwatch can automatically embed assets (binary files) into the firmware. If you see this error, you may have forgotten to clone this repository with Git LFS enabled. Please see the `README.md` for instructions on how to clone the repository with Git LFS.

## Upload problems
You compiled the firmware successfully (`Successfully created esp32 image.`), but the upload fails for some reason. Here are some common problems and solutions.

### Always double check the USB-cable!

We have seen a number of "charge-only" or broken cables in the community. Try to upload using a different cable.

### Is the driver working correctly?

On non-Linux machines you may need to install additional drivers, as noted in the [Getting Started](getting_started.md) guide.

!!! note "Checking the connection (Linux only)"
    If you are using Linux, make sure the device `/dev/ttyUSB0` or `/dev/ttyACM0` is present when you plug in the device. If it is not, the kernel is unable to communicate with the USB-to-serial chip on the device.

### Driver is there, but device is busy

On Linux, you may need to remove the `brltty` package to free up the serial port.

```bash
sudo apt-get remove brltty
```

!!! info "What is brltty?"
    `brltty` is a daemon that provides access to the Linux/Unix console (text mode) for a blind person using a refreshable braille display. It is not needed for most users, but is annoyingly always claiming all available serial ports and therefore blocking PlatformIO communication with the device.

### Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst PlatformIO was trying to connect.
