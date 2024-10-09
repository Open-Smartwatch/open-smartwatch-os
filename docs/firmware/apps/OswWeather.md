
# OSW WEATHER
![](/assets/apps/OswWeather/h.png)
Author: [@LorenzoSciacca](https://github.com/Lorenzosciacca)
## How to install
Add the flag `OSW_FEATURE_WEATHER` to the file `platformio.ini`:
```ini
; Example code block (make sure to use the one from the platformio config!)
[env:LIGHT_EDITION_V3_3]
build_flags =
	-D OSW_TARGET_PLATFORM_HEADER='"platform/LIGHT_EDITION_V3_3.h"'
	-D OSW_FEATURE_STATS_STEPS
	-D OSW_FEATURE_WIFI ; ADD THIS LINE
build_type = debug
```

## Getting the API key
The weather updates are provided by `openweathermap.org`, so you need a valid API key. 
1. Create an account [here](https://openweathermap.org/)
2. Select a free [plan](https://openweathermap.org/price)
3. From your account select "my API keys" and generate a key.

![](/assets/apps/OswWeather/key.png)

## Configuration
It's possible to configure the app from both the web UI and the configuration file `config_defaults.h`.

### Web UI
1. Paste your API key in the field `API key for Openweathermap.org`
2. Write the name of the location of your interest in the field `City name`
3. Write the two letters iso code of the country associated with your location in the field `Country code`. You can search the two letters code of a given country [here](https://www.iso.org/obp/ui/#home) selecting `country codes`.
4. Save

### Without web UI
If you want to configure the app without using the web UI you can set the following values in config_defaults.h:
- `OPENWEATHERMAP_APIKEY`, the api key
- `OPENWEATHERMAP_CITY`, city name
- `OPENWEATHERMAP__STATE_CODE`, the two letter iso code of the country of your interest

If you are not sure about the geocoding you can perform a simple test.
Type in the address bar of your browser: `https://api.openweathermap.org/data/2.5/forecast?q=CITY,STATE&appid=APIKEY&cnt=24` substituting `CITY`, `STATE` and `APIKEY` with: city name, two letters code and api key. Beware to not past additional characters, the country code must be separated by a comma from the city name. If you get an answer like this: `{"code":"200" ... ` both api key and location are valid.

## Usage
### Overview
![screen](/assets/apps/OswWeather/sync.png)   
After the installation you will notice that there are no updates to show, in order to get the updates, press the top right button `update`. A pop-up should appear, the operation will take a few seconds.  

The bottom left button allows the user to select:  

* current update  
![](/assets/apps/OswWeather/h.png)

* the current day  
![](/assets/apps/OswWeather/day.png)

The / \  and \ /  arrows are used to decrease/increase the current selection (day or update).

### Units
- Temperature: °C
- Humidity: [relative humidity](https://en.wikipedia.org/wiki/Humidity#Relative_humidity)
- Pressure: [hPa](https://en.wikipedia.org/wiki/Pascal_(unit)#Multiples_and_submultiples)

### Weather conditions

Visit [this link](https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2) for a more detailed description of each weather condition.

<table>
    <tr>
        <td>Icon</td>
        <td>#</td>
        <td>Description</td>
        <td>Openweathermap code</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/sun.png"></td>
        <td>0</td>
        <td>Clear</td>
        <td>800</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/cl1.png"></td>
        <td>1</td>
        <td>Clouds min.</td>
        <td>801</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/cl2.png"></td>
        <td>2</td>
        <td>Clouds med.</td>
        <td>802</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/cl3.png"></td>
        <td>3</td>
        <td>Clouds heavy</td>
        <td>803, 804</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/mist.png"></td>
        <td>4</td>
        <td>Mist</td>
        <td>701</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/fog.png"></td>
        <td>5</td>
        <td>Fog</td>
        <td>741</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/snow1.png"></td>
        <td>6</td>
        <td>Snow min.</td>
        <td>611, 612, 615, 616</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/snow2.png"></td>
        <td>7</td>
        <td>Snow med.</td>
        <td>600, 613, 601, 620</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/snow3.png"></td>
        <td>8</td>
        <td>Snow heavy</td>
        <td>602, 621, 622</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/rain1.png"></td>
        <td>9</td>
        <td>Rain min.</td>
        <td>500, 300, 301, 302, 310, 311, 312, 313, 314, 321</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/rain2.png"></td>
        <td>10</td>
        <td>Rain med.</td>
        <td>501, 502</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/rain3.png"></td>
        <td>11</td>
        <td>Rain heavy</td>
        <td>503, 504, 511, 520, 521, 522, 531</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/th1.png"></td>
        <td>12</td>
        <td>Thunderstorm</td>
        <td>200, 201, 210, 211, 231, 230</td>
    </tr>
    <tr>
        <td><img src="/assets/apps/OswWeather/th2.png"></td>
        <td>13</td>
        <td>Thunderstorm heavy</td>
        <td>202, 212, 221, 232</td>
    </tr>
    <tr>
        <td>!</td>
        <td>14</td>
        <td>Squall/tornado</td>
        <td>771, 781</td>
    </tr>
    <tr>
        <td>?</td>
        <td>15</td>
        <td>Unknown</td>
        <td></td>
    </tr>
</table>

## Using the emulator
To use this app while using the OSW emulator, in order to retrieve the data, it is necessary to perform the API request using the browser and to save the response in `file_weather.json` in the  `/build` folder.

 

