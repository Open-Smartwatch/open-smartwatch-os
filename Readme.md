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
pio run -e pico32_LIGHT_EDITION -t upload
```

respectively

```
pio run -e pico32_GPS_EDITION -t upload
```

depending on the watch model.

## Lauchner (PLauncher)

You can toggle faces with Btn1 and Btn3
To trigger the face action1 short press Btn2
To trigger the face action2 long press Btn2
...
Holding Btn1 and Btn3 sends you to the appmenu.
You can toggle apps with Btn1 and Btn3 and select one with btn2.
...
Holding Btn2 and Btn3 always sends you back to the launcher/current face and shutdown whatever app runs.




## Face 

A face is a small Widget which is not allowed to take control over any buttons.
It can have 2 actions which are called by the launcher
Faces can be watches or small widgets like infomerations or companion faces for apps.
...

## Apps

A app can be started from the launcher which starts at fully standalone app and can take control over all the buttons. 
The app has to have a possiblity to close it and go back to the launcher via calling backToFace() on the launcher.
If the app does not have this you can always quit it with Btn2 and Btn3 holding - See Launcher.

## Making a Watchface

To make your first custom watchface just inhert from OswBasicWatch which provides all basic needs for the watch.
The only thing you have todo is to override the draw method like this:
```
void MyCustomwatch::draw(Graphics2D* gfx2d) {
    // fill background
    gfx2d->fill(rgb565(0, 0, 0));

    // hours
    gfx2d->drawThickTick(120, 120, 0, 60, 360.0 / 12.0 * (1.0 * hour + minute / 60.0), 1, rgb565(255, 255, 255));
    
    // minutes
    gfx2d->drawThickTick(120, 120, 0, 105, 360.0 / 60.0 * (1.0 * minute + second / 60.0), 1, rgb565(255, 255, 255));
    
    // seconds
    gfx2d->drawThickTick(120, 120, 110, 16, 360.0 / 60.0 * second, 1, rgb565(255, 0, 0));
}

```
Then register it in the Launcher:
```
registerFace(new OswStopwatchFace());
```

That's it!

For more advanced faces you can always inhert using OswFace directly.

