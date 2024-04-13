#ifndef OSW_EMULATOR
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_HARDWARE_BMP581 == 1
#include <stdexcept>
#include <Wire.h>
#include <OswLogger.h>

#include <devices/bmp581.h>

static int8_t bmp5_i2c_read(uint8_t reg_addr, uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }
    uint8_t bytes_received;

    Wire.beginTransmission(BMP5_I2C_ADDR_PRIM);
    Wire.write(reg_addr);
    if (Wire.endTransmission() == 0) {
        bytes_received = Wire.requestFrom(BMP5_I2C_ADDR_PRIM, len);
        // Optionally, throw an error if bytes_received != len
        for (uint16_t i = 0; i < bytes_received; i++) {
            reg_data[i] = Wire.read();
        }
    } else {
        return -1;
    }

    return 0;
}

static int8_t bmp5_i2c_write(uint8_t reg_addr, const uint8_t* reg_data, uint32_t len, void* intf_ptr) {
    if ((reg_data == NULL) || (len == 0) || (len > 32)) {
        return -1;
    }

    Wire.beginTransmission(BMP5_I2C_ADDR_PRIM);
    Wire.write(reg_addr);
    for (uint16_t i = 0; i < len; i++) {
        Wire.write(reg_data[i]);
    }
    if (Wire.endTransmission() != 0) {
        return -1;
    }

    return 0;
}

static void bmp5_delay_us(uint32_t period, void* intf_ptr) {
    delayMicroseconds(period);
}

void OswDevices::BMP581::setup() {
    int8_t rslt;
    {
        this->bmp5.chip_id = BMP5_I2C_ADDR_PRIM;
        this->bmp5.read = bmp5_i2c_read;
        this->bmp5.write = bmp5_i2c_write;
        this->bmp5.delay_us = bmp5_delay_us;
        this->bmp5.intf = BMP5_I2C_INTF;
        this->bmp5.intf_ptr = nullptr; // we are using the default address anyways

        rslt = bmp5_init(&this->bmp5);
        if(rslt != BMP5_OK)
            throw std::runtime_error("Failed to initialize BMP581!");
    }
    {
        rslt = bmp5_set_power_mode(BMP5_POWERMODE_STANDBY, &this->bmp5);
        if(rslt != BMP5_OK)
            throw std::runtime_error("Failed to reconfigure BMP581 power mode!");

        struct bmp5_iir_config set_iir_cfg;
        rslt = bmp5_get_osr_odr_press_config(&this->osr_odr_press_cfg, &this->bmp5);

        osr_odr_press_cfg.odr = BMP5_ODR_50_HZ;
        osr_odr_press_cfg.press_en = BMP5_ENABLE;
        osr_odr_press_cfg.osr_t = BMP5_OVERSAMPLING_64X;
        osr_odr_press_cfg.osr_p = BMP5_OVERSAMPLING_4X;
        rslt = bmp5_set_osr_odr_press_config(&this->osr_odr_press_cfg, &this->bmp5);

        set_iir_cfg.set_iir_t = BMP5_IIR_FILTER_COEFF_1;
        set_iir_cfg.set_iir_p = BMP5_IIR_FILTER_COEFF_1;
        set_iir_cfg.shdw_set_iir_t = BMP5_ENABLE;
        set_iir_cfg.shdw_set_iir_p = BMP5_ENABLE;
        rslt = bmp5_set_iir_config(&set_iir_cfg, &this->bmp5);

        rslt = bmp5_set_power_mode(BMP5_POWERMODE_NORMAL, &this->bmp5);
        if(rslt != BMP5_OK)
            throw std::runtime_error("Failed to reconfigure BMP581 power mode!");
    }
    // TODO proper sensor reading configuration
}

void OswDevices::BMP581::update() {
    int8_t rslt;
    struct bmp5_sensor_data sensor_data;

    rslt = bmp5_get_sensor_data(&sensor_data, &this->osr_odr_press_cfg, &this->bmp5);

    this->pressure = sensor_data.pressure;
    this->temperature = sensor_data.temperature;
}

float OswDevices::BMP581::getPressure() {
    return this->pressure;
}

float OswDevices::BMP581::getTemperature() {
    return this->temperature;
}
#endif
#endif