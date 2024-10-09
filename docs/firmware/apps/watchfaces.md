# Watchfaces

## Default

### Analog

<img src="/assets/apps/watchfaces/watchface_analog_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default analog watchface.

Steps are shown in the blue circle, a full rotation is 10800 steps.

 * Source: [OswAppWatchface.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchface.cpp)
 * Author: [pauls-3d-things](https://github.com/pauls-3d-things)

<div style="clear: both; margin-bottom:20px"></div>
!!! tip
    When applying dual time mode `Display Dual-Time Hour Tick` for analog clocks, the analog clocks display dark-colored hour hands.
    
### Digital

<img src="/assets/apps/watchfaces/watchface_digital_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default digital watchface.

Steps are shown as a number at the bottom of the screen.

 * Source: [OswAppWatchfaceDigital.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceDigital.cpp)
 * Author: [rawsignal](https://github.com/rawsignal)

<div style="clear: both"></div>

### Mix

<img src="/assets/apps/watchfaces/watchface_mix_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default mix watchface.

Steps are shown as a step-history frame at the bottom of the screen.

 * Source: [OswAppWatchfaceMix.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceMix.cpp)
 * Author: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both"></div>

### Dual-Time

<img src="/assets/apps/watchfaces/watchface_dualtime_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default digital dual-time watchface.

Based on the animation bar representing the common time in seconds, the top is local and the bottom is dual time.

 * Source: [OswAppWatchfaceDual.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceDual.cpp)
 * Author: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both"></div>

### Fitness-Tracking

<img src="/assets/apps/watchfaces/watchface_fitnesstracking_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default fitness-tracking watchface.

It shows steps, calories, and distance during the day.

 * Source: [OswAppWatchfaceFitness.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceFitness.cpp)
 * Author: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both"></div>

### Binary

<img src="/assets/apps/watchfaces/watchface_binary_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default binary watchface.

Steps are shown as a hexadecimal number at side of the screen.

 * Source: [OswAppWatchfaceBinary.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceBinary.cpp)
 * Author: [xasz](https://github.com/xasz)

<div style="clear: both"></div>

### Monotimer

<img src="/assets/apps/watchfaces/watchface_monotimer_osw.png" width="256px" style="float:left; margin-right:20px"/>
The default Monotimer watchface.

Since a monotimer-clock has only a single hand, you read both the hours and minutes from the position of it. First, you read the hours by finding the nearest hour from the left (clockwise) of the hand. Then you read the Minutes by looking at the ticks between the hour tick and the hand. The smallest tick stands for a 5 minute interval, the next bigger one for 15 minutes and the biggest one for 30 minutes (see image). For example, the clock from the image shows the time between 9:25 and 9:30.

The steps are displayed in the center of the clock. If the daily step goal is reached, the text color changes from InfoColor to SucessColor.

 * Source: [OswAppWatchfaceMonotimer.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/watchfaces/OswAppWatchfaceMonotimer.cpp)
 * Author: [David Schramm](https://github.com/daschr)

<div style="clear: both"></div>

## More

Want to add your watch face to this project? Fork this repository: [https://github.com/Open-Smartwatch/open-smartwatch.github.io/tree/source](https://github.com/Open-Smartwatch/open-smartwatch.github.io/tree/source) and create a pull request. Join the discord server if you have questions: [https://discord.gg/9DK5JY6](https://discord.gg/9DK5JY6).
