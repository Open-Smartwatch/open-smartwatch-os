# HowTo Run

**Please read the documentation on the [website](https://open-smartwatch.github.io/4_flashing/)!**
## prerequirements

* install [Platformio Core](https://docs.platformio.org/en/latest/core/installation.html) or [Platformio IDE](https://docs.platformio.org/en/latest/integration/ide/vscode.html#ide-vscode) (which installes Pio core automatically)
* install [SWIG](http://www.swig.org/Doc4.0/SWIGDocumentation.html#Preface_installation) (also available in most package managers, e.g. `brew install swig`)
* Then clone this repository

```
git clone --recurse-submodules https://github.com/Open-Smartwatch/open-smartwatch-os.git
```

## build (Visual Studio Code)

Open the cloned repo in VSCode

```
code open-smartwatch-os
```

and rename file `include/config.h.example` to `include/config.h` and adapt the values according to your requirements and press f5 to build it.


## build (CLI)

You can instead go to the repo folder with your terminal and run 

```
pio run -e pico32_LIGHT_EDITION
```

respectively

```
pio run -e pico32_GPS_EDITION
```

depending on the watch model.
