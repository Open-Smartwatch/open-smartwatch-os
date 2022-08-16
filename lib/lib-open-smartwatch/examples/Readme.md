# Requirements

The test code, to run the stuff on a desktop is a bit messy, but it works.

## Getting Started

### Dependencies

Install *cmake* and all required libs (in case you are on Debian/Linux):
```bash
apt-get install -y cmake libsdl2-dev libsdl2-image-dev
```

### Running all examples (e.g. DoomFire)
```bash
mkdir build
cd build
cmake ..
make -j $(nproc)
./DoomFire
```

### Windows + WSL way to work

On windows, a very Unix friendly way to run the 2D Emulator is to use [WSL](https://en.wikipedia.org/wiki/Windows_Subsystem_for_Linux) that way, you can use the same commands as described by the first part of the tutorial. Your default disks and working directories are available (at least on Ubuntu) on `/mnt/<diskName>`, `/mnt/c`, `/mnt/d`, ...

-------

<summary>
⚠ CMake issues
<details>
You will probably get some issues with CMake, I personally needed to rebuild CMake because the latest available version was not the latest version. It could be done by getting the sources from web.

> source https://askubuntu.com/questions/829310/how-to-upgrade-cmake-in-ubuntu

    Download the latest version on: https://cmake.org/download/ uncompress it then :

```
sudo apt-get install libssl-dev
cd $CMAKE_DOWNLOAD_PATH
./configure
make
sudo make install
```

</details>
</summary>

-------

By default, WSL doesn't integrate a XServer that is needed to run Graphical interfaces. That's why we need to add it, my choice was to use GWSL, it is available [on their website](https://opticos.github.io/gwsl/).

One you installed it, you will use the `shortcut creator`, add a label (Something like `OSW 2D`), and use the absolute path to the file in your WSL format (For example, my project is in the D directory and I want to run the watch-simple : `/mnt/d/openSmartWatch/lib-open-smartwatch/examples/watch-simple/build/<Compiled name often : Example>`), validate the modal by clicking `Add to start menu` then you can get an app by looking on the windows search menu `OSW 2D on <Distro name>`.

You can try the configuration before creating the app by clicking `Test Configuration`

After that instead of running ./Example or ./DoomFire, you just need to start the app.

To rebuild the app the only thing that is needed is to use `make` from the build directory

### Warning :
Because my environment is up, I probably missed some part, if you find some errors or missing parts in this tutorial don't hesitate to make a Pull Request 
