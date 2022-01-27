# What is this?
...

# Elements

## Environment
To globally enable or disable platform support for any environment sensor, set this to `0` or `1`:
```cpp
#define OSW_PLATFORM_ENVIRONMENT 1
```

### Temperature
To globally enable or disable platform support for any temperature sensor, set this to `0` or `1`:
```cpp
#define OSW_PLATFORM_ENVIRONMENT_TEMPERATURE 1
```

#### DS3231MZ / RTC
To enable this sensor as temperature provider:
```cpp
#define OSW_PLATFORM_ENVIRONMENT_TEMPERATURE_DS3231MZ 1
```

#### BMA400
To enable this sensor as temperature provider:
```cpp
#define OSW_PLATFORM_ENVIRONMENT_TEMPERATURE_BMA400 1
```

### Accelerometer
To globally enable or disable platform support for any accelerometer sensor, set this to `0` or `1`:
```cpp
#define OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER 1
```

#### BMA400
To enable this sensor as accelerometer provider:
```cpp
#define OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER_BMA400 1
```

# Pins
## DS3231MZ
## BMA400