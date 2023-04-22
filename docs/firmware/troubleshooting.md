# Troubleshooting

## Always double check the cable

We have seen a number of "charge only" cables in the community. Try to upload using a different cable.

## Check the driver

Check the driver insertion.

## Arduino_TFT.h: No such file or directory

You did not clone the repository with the `--recursive-submodules` flag.

!!! note "Tip"
    After changing the branch, follow the command :
    `git submodule update`

## Failed to connect to ESP32: Timed out waiting for packet header

You did not hold down BTN1(FLASH) and then tap the RESET button on the watch whilst platform.io was trying to connect.

## Fixing `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`

On Fedora Silverblue 34 `make` outputs `error: unknown type name ‘wait_queue_t’; did you mean ‘wait_event’?`.
This can be fixed by using [this fork from juliagoda](https://github.com/juliagoda/CH341SER).
