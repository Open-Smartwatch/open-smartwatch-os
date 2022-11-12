# OSW WEATHER

- [How to install](#how-to-install)
- [Getting the API key](#getting-the-api-key)
- [Configuration](#configuration)
  * [Web UI](#web-ui)
  * [Without web UI](#without-web-ui)
- [Usage](#usage)
  * [Overview](#overview)
  * [Units](#units)
  * [Weather conditions](#weather-conditions)

## How to install
- Add the flag OSW_FEATURE_WEATHER to the file platformio.ini  
![](../../assets/apps/OswWeather/example_ini.png)

## Getting the API key
The weather updates are provided by openweathermap.org, so you need a valid API key. 
- Create an account [here](https://openweathermap.org/)
- Select a free [plan](https://openweathermap.org/price)
- From your account select "my API keys" and generate a key.
![](../../assets/apps/OswWeather/key.png)
## Configuration
It's possible to configure the app from both the web UI and the configuration file `config_defaults.h`
### Web UI
- Paste your API key in the field `API key for Openweathermap.org`
- Write the name of the location of your interest in the field `City name`
- Write the two letters iso code of the country associated with your location in the field `Country code`. You can search the two letters code of a given country [here](https://www.iso.org/obp/ui/#home) selecting `country codes`.
- Save
### Without web UI
If you want to configure the app without using the web UI you can set the followings values in config_defaults.h:
- `OPENWEATHERMAP_APIKEY`, the api key
- `OPENWEATHERMAP_CITY`, city name
- `OPENWEATHERMAP__STATE_CODE`, the two letter iso code of the country of your interest    



If you are not sure about the geocoding you can perform a simple test.
Type in the address bar of your browser: "https://api.openweathermap.org/data/2.5/forecast?q=CITY,STATE&appid=APIKEY&cnt=24 "
substituting `CITY`, `STATE` and `APIKEY` with: city name, two letters code and api key. Beware not to past additional characters, the country code must be separated by a comma from the city name.
If you get an answer like this: `{"cod":"200" ... ` both api key and location are valid.

## Usage
### Overview
![screen](../../assets/apps/OswWeather/sync.png)   
After the installation you will notice that there are no updates to show, in order to get the updates, press the top right button `update`. A pop-up should appear, the operation will take a few seconds.  

The bottom left button allows the user to select:  

current update  
![](../../assets/apps/OswWeather/h.png)

the current day  
![](../../assets/apps/OswWeather/day.png)

The / \  and \ /  arrows are used to decrease/increase the current selection ( day or update ) .
### Units
- Temperature: °C
- Humidity: relative [humidity](https://en.wikipedia.org/wiki/Humidity#Relative_humidity)
- Pressure: [hPa](https://en.wikipedia.org/wiki/Pascal_(unit)#Multiples_and_submultiples)
### Weather conditions 
Visit [this link](https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2) for a more detailed description of each weather condition.
| Icon | # | Description | Openweathermap code |
|------|---|-------------|------------------|
|![](../../assets/apps/OswWeather/sun.png) | 0 | Clear |800  |
|![](../../assets/apps/OswWeather/cl1.png) | 1 | Clouds min. |801  |
|![](../../assets/apps/OswWeather/cl2.png) | 2 | Clouds med. |802  |
|![](../../assets/apps/OswWeather/cl3.png) | 3 | Clouds heavy |803, 804 |
|![](../../assets/apps/OswWeather/mist.png) | 4 | Mist |701  |
|![](../../assets/apps/OswWeather/fog.png) | 5 | Fog |741  |
|![](../../assets/apps/OswWeather/snow1.png) | 6 | Snow min. |611, 612, 615, 616  |
|![](../../assets/apps/OswWeather/snow2.png) | 7 | Snow med. |600, 613, 601, 620|
|![](../../assets/apps/OswWeather/snow3.png) | 8 | Snow heavy |602, 621, 622|
|![](../../assets/apps/OswWeather/rain1.png) | 9 | Rain min. |500, 300, 301, 302, 310, 311, 312, 313, 314, 321|
|![](../../assets/apps/OswWeather/rain2.png) | 10 | Rain med. |501, 502|
|![](../../assets/apps/OswWeather/rain3.png) | 11 | Rain heavy |503, 504, 511, 520, 521, 522, 531|
|![](../../assets/apps/OswWeather/th1.png) | 12 | Thunderstorm  |200, 201, 210, 211, 231, 230|
|![](../../assets/apps/OswWeather/th2.png) | 13 | Thunderstorm heavy  |202, 212, 221, 232|
| ! | 14 | Squall/tornado  |771, 781|
| ? | 15 | Unknown  ||

