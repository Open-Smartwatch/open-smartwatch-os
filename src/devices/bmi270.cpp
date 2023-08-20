#ifndef OSW_EMULATOR
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_BMI270 == 1
#include <stdexcept>
#include <Wire.h>
#include <OswLogger.h>

#include <devices/bmi270.h>

static int8_t bmi2_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }
    uint8_t bytes_received;

    Wire.beginTransmission(BMI2_I2C_PRIM_ADDR);
    Wire.write(reg_addr);
    if (Wire.endTransmission() == 0) {
        bytes_received = Wire.requestFrom(BMI2_I2C_PRIM_ADDR, len);
        // Optionally, throw an error if bytes_received != len
        for (uint16_t i = 0; i < bytes_received; i++) {
            reg_data[i] = Wire.read();
        }
    } else {
        return -1;
    }

    return 0;
}

static int8_t bmi2_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }

    Wire.beginTransmission(BMI2_I2C_PRIM_ADDR);
    Wire.write(reg_addr);
    for (uint16_t i = 0; i < len; i++) {
        Wire.write(reg_data[i]);
    }
    if (Wire.endTransmission() != 0) {
        return -1;
    }

    return 0;
}

static void bmi2_delay_us(uint32_t period, void* intf_ptr) {
    delayMicroseconds(period);
}

void OswDevices::BMI270::setup() {
    int8_t rslt;
    {
        this->bmi2.chip_id = BMI2_I2C_PRIM_ADDR;
        this->bmi2.read = bmi2_i2c_read;
        this->bmi2.write = bmi2_i2c_write;
        this->bmi2.delay_us = bmi2_delay_us;
        this->bmi2.intf = BMI2_I2C_INTF;
        this->bmi2.intf_ptr = nullptr; // we are using the default address anyways
        this->bmi2.read_write_len = 30; // Bosch mentioned limitations of the "Wire library"
        this->bmi2.config_file_ptr = nullptr; // we are not using a config file

        rslt = bmi270_init(&this->bmi2);
        if(rslt != BMI2_OK)
            throw std::runtime_error("Failed to initialize BMI270!");
    }

    {
        struct bmi2_int_pin_config int_pin_cfg;
        int_pin_cfg.pin_type = BMI2_INT1;
        int_pin_cfg.int_latch = BMI2_INT_NON_LATCH;
        int_pin_cfg.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
        int_pin_cfg.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
        int_pin_cfg.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
        int_pin_cfg.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;

        struct bmi2_sens_config sens_cfg[2];
        sens_cfg[0].type = BMI2_ACCEL;
        sens_cfg[0].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
        sens_cfg[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
        sens_cfg[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;
        sens_cfg[0].cfg.acc.range = BMI2_ACC_RANGE_4G;
        sens_cfg[1].type = BMI2_GYRO;
        sens_cfg[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
        sens_cfg[1].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
        sens_cfg[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
        sens_cfg[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;
        sens_cfg[1].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

        rslt = bmi2_set_int_pin_config(&int_pin_cfg, &this->bmi2);
        if (rslt != BMI2_OK)
            throw std::runtime_error("Failed configure pins of BMI270!");

        rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, &this->bmi2);
        if (rslt != BMI2_OK)
            throw std::runtime_error("Failed to map data interrupt of BMI270!");

        rslt = bmi2_set_sensor_config(sens_cfg, 2, &this->bmi2);
        if (rslt != BMI2_OK)
            throw std::runtime_error("Failed to configure sensors of BMI270!");

        uint8_t cmndSetFeaturePage = 6;
        if(bmi2_set_regs(0x2F, &cmndSetFeaturePage, sizeof(cmndSetFeaturePage), &this->bmi2) < 0)
            throw std::runtime_error("Failed to set configuration page of BMI270!");

        uint8_t cmnd[2] = { 0x00, 0x10 | 0x20 }; // ...; enable counter, enable activity
        if(bmi2_set_regs(0x32, cmnd, sizeof(cmnd), &this->bmi2) < 0)
            throw std::runtime_error("Failed to configure step counter of BMI270!");

        uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };
        rslt = bmi2_sensor_enable(sens_list, 2, &this->bmi2);
        if (rslt != BMI2_OK)
            throw std::runtime_error("Failed to enable sensors of BMI270!");
    }

    // We are NOT initializing the IMM150, because there is no hardware with it ;)
}

void OswDevices::BMI270::update() {
    this->updateAcceleration();
    this->updateSteps();
    this->updateTemperature();
    this->updateActivityMode();
}

void OswDevices::BMI270::updateAcceleration() {
    int8_t rslt;
    struct bmi2_sens_data sensor_data;

    rslt = bmi2_get_sensor_data(&sensor_data, &this->bmi2);
    if (rslt != BMI2_OK) {
        OSW_LOG_E("BMI270 Acceleration read error (SPI error)");
        return;
    }

    this->accX = (float) sensor_data.acc.x / 1000;
    this->accY = (float) sensor_data.acc.y / 1000;
    this->accZ = (float) sensor_data.acc.z / 1000;
}

void OswDevices::BMI270::updateSteps() {
    uint8_t data[2];
    if (bmi2_get_regs(0x1E, data, sizeof(data), &this->bmi2) < 0) {
        OSW_LOG_E("BMI270 Steps read error");
        return;
    }

    this->step_count = (data[1] << 8) | data[0];
}

void OswDevices::BMI270::updateTemperature() {
    uint8_t data[2];
    if (bmi2_get_regs(0x22, data, sizeof(data), &this->bmi2) < 0) {
        OSW_LOG_E("BMI270 Temperature read error");
        return;
    }

    int16_t temp = (data[1] << 8) | data[0];

    if(temp == 0x8000) {
        OSW_LOG_E("BMI270 Temperature read error (value error)");
        return;
    }

    this->temperature = temp / 100.0f;
}

void OswDevices::BMI270::updateActivityMode() {
    uint8_t data;
    if (bmi2_get_regs(0x20, &data, sizeof(data), &this->bmi2) < 0) {
        OSW_LOG_E("BMI270 Activity mode read error");
        return;
    }
    data = data >> 2; // drop the first two bytes by shifting to the right
    data = data & 0x03; // drop the last 6 bytes by masking

    switch(data) {
    case 0x00:
        this->activityMode = OswAccelerationProvider::ActivityMode::STILL;
        break;
    case 0x01:
        this->activityMode = OswAccelerationProvider::ActivityMode::WALK;
        break;
    case 0x02:
        this->activityMode = OswAccelerationProvider::ActivityMode::RUN;
        break;
    default:
        this->activityMode = OswAccelerationProvider::ActivityMode::UNKNOWN; // also known as 0x03
        break;
    }
}

float OswDevices::BMI270::getAccelerationX() {
    return this->accX;
}

float OswDevices::BMI270::getAccelerationY() {
    return this->accY;
}

float OswDevices::BMI270::getAccelerationZ() {
    return this->accZ;
}

uint32_t OswDevices::BMI270::getStepCount() {
    return this->step_count;
}

OswAccelerationProvider::ActivityMode OswDevices::BMI270::getActivityMode() {
    return this->activityMode;
}

void OswDevices::BMI270::resetStepCount() {
    uint8_t cmndSetFeaturePage = 6;
    if(bmi2_set_regs(0x2F, &cmndSetFeaturePage, sizeof(cmndSetFeaturePage), &this->bmi2) < 0)
        throw std::runtime_error("Failed to set configuration page of BMI270!");

    uint8_t cmnd[2] = { 0x00, 0x4 | 0x10 | 0x20 }; // ...; reset steps, enable counter, enable activity
    if(bmi2_set_regs(0x32, cmnd, sizeof(cmnd), &this->bmi2) < 0)
        throw std::runtime_error("Failed to configure step counter of BMI270!");

    this->step_count = 0;
}

float OswDevices::BMI270::getTemperature() {
    return this->temperature;
}
#endif
#endif