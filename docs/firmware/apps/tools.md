# Tools

## Config App

<img src="/assets/apps/tools/app_config_osw.png" width="256px" style="float:left; margin-right:20px"/>
**The** configuration app. Once connected to the WiFi it starts a webserver that allows you to access and modify your watch's configuration as well as upload new firmware images.
Open the webpage `http://192.168.1.119/` (the IP will change according to your local WiFi network), in your browser. Use the username `admin` as well as the password shown on the watch screen to authenticate.

 * Source: [OswAppWebserver.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/OswAppWebserver.cpp)
 * Authors: [simonmicro](https://github.com/simonmicro), [pauls-3d-things](https://github.com/pauls-3d-things)

<a href="/assets/apps/tools/www_config_app_login.png" target="_blank"><img src="/assets/apps/tools/www_config_app_login.png" width="320px" style="float:left; margin-right:20px"/></a>
<a href="/assets/apps/tools/www_config_app_config.png" target="_blank"><img src="/assets/apps/tools/www_config_app_config.png" width="320px" style="float:left; margin-right:20px"/></a>
<a href="/assets/apps/tools/www_config_app_ota.png" target="_blank"><img src="/assets/apps/tools/www_config_app_ota.png" width="320px" style="float:left; margin-right:20px"/></a>
<a href="/assets/apps/tools/www_config_app_info.png" target="_blank"><img src="/assets/apps/tools/www_config_app_info.png" width="320px" style="float:left; margin-right:20px"/></a>


<div style="clear: both; margin-bottom:20px"></div>

## Manual Time Setting

<img src="/assets/apps/tools/app_config_time_manual_osw.png" width="256px" style="float:left; margin-right:20px"/>
If you have no WiFi connection, you can manually set the time and date with the manual configuration app.

 * Source: [time_config.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/time_config.cpp)
 * Authors: [richtepa](https://github.com/richtepa)

<div style="clear: both; margin-bottom:20px"></div>

## Steps stats

<img src="/assets/apps/tools/app_tool_stepsStats_osw.png" width="256px" style="float:left; margin-right:20px"/>
You can check the steps per day of the week. And you can see the total value for each item and the number for the day.

 * Source: [OswAppStepStats.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/OswAppStepStats.cpp)
 * Authors: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both; margin-bottom:20px"></div>

## Kcal stats

<img src="/assets/apps/tools/app_tool_kcalStats_osw.png" width="256px" style="float:left; margin-right:20px"/>
You can check the calorie consumption of the latest date. And you can see the average value for each item and the number for the day.

 * Source: [OswAppKcalStats.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/OswAppKcalStats.cpp)
 * Authors: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both; margin-bottom:20px"></div>

!!! tip
    You can use the up and down keys to view other dates.

## Fitness stats

<img src="/assets/apps/tools/app_tool_fitnessStats_osw.png" width="256px" style="float:left; margin-right:20px"/>
You can see the steps, distance, calorie consumption, and average achievement of all items during the day.

 * Source: [OswAppFitnessStats.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/OswAppFitnessStats.cpp)
 * Authors: [ruffalolavoisier](https://github.com/RuffaloLavoisier)

<div style="clear: both; margin-bottom:20px"></div>

## Stop Watch

<img src="/assets/apps/tools/app_tool_stopwatch_osw.png" width="256px" style="float:left; margin-right:20px"/>
A stopwatch to count seconds.

 * Source: [stopwatch.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/main/stopwatch.cpp)
 * Authors: [pauls-3d-things](https://github.com/pauls-3d-things)

<div style="clear: both; margin-bottom:20px"></div>

## Water Level

<img src="/assets/apps/tools/app_tool_waterlevel01_osw.png" width="256px" style="float:left; margin-right:20px"/>
A water level, to test the acceleration sensor.

 * Source: [water_level.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/water_level.cpp)
 * Authors: [pauls-3d-things](https://github.com/pauls-3d-things)

<div style="clear: both; margin-bottom:20px"></div>

<img src="/assets/apps/tools/app_tool_waterlevel02_osw.png" width="256px" style="float:left; margin-right:20px"/>

The second screen, axis view.

 * Source: [water_level.cpp](https://github.com/Open-Smartwatch/open-smartwatch-os/blob/master/src/apps/tools/water_level.cpp)
 * Authors:  [GPaddle](https://github.com/GPaddle)

<div style="clear: both; margin-bottom:20px"></div>

## More

Want to add your tool to this project? Fork this repository: [https://github.com/Open-Smartwatch/open-smartwatch.github.io/tree/source](https://github.com/Open-Smartwatch/open-smartwatch.github.io/tree/source) and create a pull request. Join the discord server if you have questions: [https://discord.gg/9DK5JY6](https://discord.gg/9DK5JY6).
