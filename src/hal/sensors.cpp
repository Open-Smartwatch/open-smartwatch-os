#include <Adafruit_Sensor.h>
#include <Wire.h>

// #include "BlueDot_BMA400.h"
// #include "bma400_defs.h"
#include "bma400.h"
#include "osw_hal.h"
#include "osw_pins.h"


/* Earth's gravity in m/s^2 */
#define GRAVITY_EARTH (9.80665f)

/* 39.0625us per tick */
#define SENSOR_TICK_TO_S (0.0000390625f)

#define READ_WRITE_LENGTH UINT8_C(46)
struct bma400_dev bma;
float accelT = 0, accelX = 0, accelY = 0, accelZ = 0;

static uint8_t dev_addr;
uint8_t act_int;
uint32_t step_count = 0;

static float lsb_to_ms2(int16_t accel_data, uint8_t g_range, uint8_t bit_width) {
  float accel_ms2;
  int16_t half_scale;

  half_scale = 1 << (bit_width - 1);
  accel_ms2 = (GRAVITY_EARTH * accel_data * g_range) / half_scale;

  return accel_ms2;
}

void bma400_delay_us(uint32_t period, void *intf_ptr) { delayMicroseconds(period); }

void bma400_check_rslt(const char api_name[], int8_t rslt) {
  if (rslt != BMA400_OK) {
    Serial.print(api_name);
    Serial.print(": ");
  }

  switch (rslt) {
    case BMA400_OK:
      // Serial.print("BMA400 OK");
      /* Do nothing */
      break;
    case BMA400_E_NULL_PTR:
      Serial.print("Error [");
      Serial.print(rslt);
      Serial.println("] : Null pointer");
      break;
    case BMA400_E_COM_FAIL:
      Serial.print("Error [");
      Serial.print(rslt);
      Serial.println("] : Communication failure");
      break;
    case BMA400_E_INVALID_CONFIG:
      Serial.print("Error [");
      Serial.print(rslt);
      Serial.println("] : Invalid configuration");
      break;
    case BMA400_E_DEV_NOT_FOUND:
      Serial.print("Error [");
      Serial.print(rslt);
      Serial.println("] : Device not found");
      break;
    default:
      Serial.print("Error [");
      Serial.print(rslt);
      Serial.println("] : Unknown error code");
      break;
  }
}

BMA400_INTF_RET_TYPE bma400_i2c_read(uint8_t reg_addr, uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  uint8_t dev_addr = *(uint8_t *)intf_ptr;
  Wire.beginTransmission(dev_addr);
  if (!Wire.write(reg_addr)) {
    return 1;
  }
  Wire.endTransmission();
  // TODO: check if Wire.endTransmission(false); is the correct approach
  // if (!) {
  //   return 2;
  // }
  if (!Wire.requestFrom(dev_addr, len)) {
    return 3;
  }
  for (uint32_t i = 0; i < len; i++) {
    int value = Wire.read();
    if (value >= 0) {
      reg_data[i] = value;
    } else {
      return 4;
    }
  }

  return 0;
}

BMA400_INTF_RET_TYPE bma400_i2c_write(uint8_t reg_addr, const uint8_t *reg_data, uint32_t len, void *intf_ptr) {
  uint8_t dev_addr = *(uint8_t *)intf_ptr;
  // TODO: catch errors
  Wire.beginTransmission(dev_addr);
  if (!Wire.write(reg_addr)) {
    return 1;
  }
  for (uint32_t i = 0; i < len; i++) {
    if (!Wire.write(reg_data[i])) {
      return 2;
    }
  }
  Wire.endTransmission();
  // TODO: check if Wire.endTransmission(false); is the correct approach
  // if (!Wire.endTransmission()) {
  //   return 3;
  // }
  return 0;
}

int8_t bma400_interface_init(struct bma400_dev *bma400, uint8_t intf) {
  int8_t rslt = BMA400_OK;

  if (bma400 != NULL) {
    // delay(100);

    if (intf == BMA400_I2C_INTF) {
      dev_addr = BMA400_I2C_ADDRESS_SDO_LOW;
      bma400->read = bma400_i2c_read;
      bma400->write = bma400_i2c_write;
      bma400->intf = BMA400_I2C_INTF;
    }
    /* Bus configuration : SPI */
    else if (intf == BMA400_SPI_INTF) {
      // not supported on open smartwatch
      rslt = BMA400_E_NULL_PTR;
    }

    bma400->intf_ptr = &dev_addr;
    bma400->delay_us = bma400_delay_us;
    bma400->read_write_len = READ_WRITE_LENGTH;

    // delay(200);
  } else {
    rslt = BMA400_E_NULL_PTR;
  }

  return rslt;
}

void setupTiltToWake() {
  int8_t rslt = 0;

  // get current state of 0x1F register
  uint8_t regSet = 0;
  rslt = bma400_get_regs(0x1f, &regSet, 1, &bma);
  bma400_check_rslt("bma400_get_regs 0x1f", rslt);

  if (rslt != BMA400_OK) {
    // we were NOT successful getting the 0x1f register
    return;
  }

  // set orientation change used for interrupt
  uint8_t data = BMA400_AXIS_X_EN & BMA400_DATA_SRC_ACCEL_FILT_LP;
  rslt = bma400_set_regs(0x35, &data, 1, &bma);

  // set the threshold for the twist
  // todo: test different values here to see if this changes anything. not sure if "data" is the value that needs to be
  // adjusted to set threshold.
  data = OswConfigAllKeys::raiseToWakeSensitivity.get();
  rslt = bma400_set_regs(0x36, &data, 1, &bma);
  bma400_check_rslt("bma400_set_regs 0x36", rslt);

  // set stable time in 50ths of a second
  data = 0x19;
  rslt = bma400_set_regs(0x38, &data, 1, &bma);
  bma400_check_rslt("bma400_set_regs 0x38", rslt);

  // add orientation change to current interrupt settings
  regSet = regSet & BMA400_AXIS_X_EN;
  rslt = bma400_set_regs(0x1f, &regSet, 1, &bma);
  bma400_check_rslt("bms400_set_regs 0x1f", rslt);

  // get the current setting for 0x21 (int1 map)
  rslt = bma400_get_regs(0x21, &regSet, 1, &bma);
  bma400_check_rslt("bma400_get_regs 0x21", rslt);

  // add orientch to int1 map
  if (rslt == BMA400_OK) {
    // in this case we could read existing data
    if (OswConfigAllKeys::raiseToWakeEnabled.get()) {
      regSet = regSet | 0b00000010;  // OR 1 to enable
    } else {
      regSet = regSet & 0b11111101;  // AND 0 to disable
    }
    rslt = bma400_set_regs(0x21, &regSet, 1, &bma);
  } else {
    // in this case we could not read existing data,
    // and disable everything (or just enable our specific interrupt)
    if (OswConfigAllKeys::raiseToWakeEnabled.get()) {
      data = 0x02;
    } else {
      data = 0x00;
    }
    rslt = bma400_set_regs(0x21, &data, 1, &bma);
  }
}

// BlueDot_BMA400 bma400 = BlueDot_BMA400();
void IRAM_ATTR isrStep() { Serial.println("Step"); }
void IRAM_ATTR isrTap() {
  // check which interrupt fired
  // TODO: read INT_STAT0,INT_STAT1,INT_STAT2

  Serial.println("Tap/Tilt");
}

void OswHal::setupSensors() {
  struct bma400_sensor_conf accel_setting[3] = {{}};
  struct bma400_int_enable int_en[3];
  int8_t rslt = 0;

  Wire.begin(SDA, SCL, 100000L);

  rslt = bma400_interface_init(&bma, BMA400_I2C_INTF);
  bma400_check_rslt("bma400_interface_init", rslt);

  // rslt = bma400_soft_reset(&bma);
  // bma400_check_rslt("bma400_soft_reset", rslt);

  rslt = bma400_init(&bma);
  bma400_check_rslt("bma400_init", rslt);

  accel_setting[0].type = BMA400_STEP_COUNTER_INT;
  accel_setting[1].type = BMA400_ACCEL;
  accel_setting[2].type = BMA400_TAP_INT;

  rslt = bma400_get_sensor_conf(accel_setting, 3, &bma);
  bma400_check_rslt("bma400_get_sensor_conf", rslt);

  setupTiltToWake();  // registers tilt on INT_CHANNEL_1

  // registers steps on INT_CHANNEL_2
  accel_setting[0].param.step_cnt.int_chan = BMA400_INT_CHANNEL_2;

  // settings required for tap detection to work
  accel_setting[1].param.accel.odr = BMA400_ODR_200HZ;
  accel_setting[1].param.accel.range = BMA400_RANGE_16G;
  accel_setting[1].param.accel.data_src = BMA400_DATA_SRC_ACCEL_FILT_1;
  accel_setting[1].param.accel.filt1_bw = BMA400_ACCEL_FILT1_BW_1;

  // registers taps on INT_CHANNEL_1
  accel_setting[2].param.tap.int_chan = BMA400_INT_CHANNEL_1;
  accel_setting[2].param.tap.axes_sel = BMA400_TAP_Z_AXIS_EN;  // BMA400_TAP_X_AXIS_EN | BMA400_TAP_Y_AXIS_EN |
  accel_setting[2].param.tap.sensitivity = BMA400_TAP_SENSITIVITY_5;
  accel_setting[2].param.tap.tics_th = BMA400_TICS_TH_6_DATA_SAMPLES;
  accel_setting[2].param.tap.quiet = BMA400_QUIET_60_DATA_SAMPLES;
  accel_setting[2].param.tap.quiet_dt = BMA400_QUIET_DT_4_DATA_SAMPLES;

  rslt = bma400_set_sensor_conf(accel_setting, 3, &bma);
  bma400_check_rslt("bma400_set_sensor_conf", rslt);

  rslt = bma400_set_power_mode(BMA400_MODE_NORMAL, &bma);
  bma400_check_rslt("bma400_set_power_mode", rslt);

  int_en[0].type = BMA400_STEP_COUNTER_INT_EN;
  int_en[0].conf = BMA400_ENABLE;
  int_en[1].type = BMA400_LATCH_INT_EN;
  int_en[1].conf = BMA400_DISABLE;
  int_en[2].type = BMA400_SINGLE_TAP_INT_EN;
  int_en[2].conf = OswConfigAllKeys::tapToWakeEnabled.get() ? BMA400_ENABLE : BMA400_DISABLE;

  rslt = bma400_enable_interrupt(int_en, 3, &bma);

  bma400_check_rslt("bma400_enable_interrupt", rslt);

  pinMode(BMA_INT_1, INPUT);
  pinMode(BMA_INT_2, INPUT);

  attachInterrupt(BMA_INT_1, isrTap, FALLING);
  attachInterrupt(BMA_INT_2, isrStep, FALLING);
}

bool OswHal::hasBMA400(void) { return _hasBMA400; }

void OswHal::updateAccelerometer(void) {
  int8_t rslt = BMA400_OK;
  struct bma400_sensor_data data;

  rslt = bma400_get_steps_counted(&step_count, &act_int, &bma);
  bma400_check_rslt("bma400_get_steps_counted", rslt);

  rslt = bma400_get_accel_data(BMA400_DATA_SENSOR_TIME, &data, &bma);
  bma400_check_rslt("bma400_get_accel_data", rslt);

  /* 12-bit accelerometer at range 2G */
  accelX = lsb_to_ms2(data.x, 2, 12);
  accelY = lsb_to_ms2(data.y, 2, 12);
  accelZ = lsb_to_ms2(data.z, 2, 12);

  if (!_hasBMA400 && accelX != 0) {
    _hasBMA400 = true;
  }
  // TODO: add getter
  accelT = (float)data.sensortime * SENSOR_TICK_TO_S;
}
float OswHal::getAccelerationX(void) {
#if defined(GPS_EDITION)
  return accelX;
#else
  return accelY;
#endif
};
float OswHal::getAccelerationY(void) {
#if defined(GPS_EDITION)
  return -accelY;
#else
  return accelX;
#endif
};
float OswHal::getAccelerationZ(void) { return accelZ; };

uint32_t OswHal::getStepCount(void) { return step_count; };

uint8_t OswHal::getActivityMode(void) { return act_int; };
