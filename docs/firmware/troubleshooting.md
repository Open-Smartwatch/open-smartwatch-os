# Troubleshooting

## Always double check the cable

We have seen a number of "charge only" cables in the community. Try to upload using a different cable.

## Check the driver

On non-Linux machines you may need to install additional drivers, as noted in the [Getting Started](getting_started.md) guide.

### Driver is there, but device is busy

On Linux, you may need to remove the `brltty` package to free up the serial port.

```bash
sudo apt-get remove brltty
```

!!! info "What is brltty?"
    `brltty` is a daemon that provides access to the Linux/Unix console (text mode) for a blind person using a refreshable braille display. It is not needed for most users, but is annoyingly always claiming all available serial ports.

## Arduino_TFT.h: No such file or directory

You did not clone the repository with the `--recursive-submodules` flag.

!!! note "Tip"
    After changing or updating/pulling a branch, run the commands notes inside the `README.md` to update the submodules and the PlatformIO packages.

## Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst PlatformIO was trying to connect.

## Fixing `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`

On Fedora Silverblue 34 `make` outputs `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`.
This can be fixed by using [this fork from juliagoda](https://github.com/juliagoda/CH341SER).
