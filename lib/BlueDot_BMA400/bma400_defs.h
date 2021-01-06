/**
* Copyright (c) 2020 Bosch Sensortec GmbH. All rights reserved.
*
* BSD-3-Clause
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
*
* 3. Neither the name of the copyright holder nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*
* @file       bma400_defs.h
* @date       2020-06-05
* @version    v1.5.8
*
*/

/*! @cond DOXYGEN_BMA400_DEFS_H_ */

#ifndef BMA400_DEFS_H_
#define BMA400_DEFS_H_

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/kernel.h>
#else
#include <stdint.h>
#include <stddef.h>
#endif

#if !defined(UINT8_C) && !defined(INT8_C)
#define INT8_C(x)                                 S8_C(x)
#define UINT8_C(x)                                U8_C(x)
#endif

#if !defined(UINT16_C) && !defined(INT16_C)
#define INT16_C(x)                                S16_C(x)
#define UINT16_C(x)                               U16_C(x)
#endif

#if !defined(INT32_C) && !defined(UINT32_C)
#define INT32_C(x)                                S32_C(x)
#define UINT32_C(x)                               U32_C(x)
#endif

#if !defined(INT64_C) && !defined(UINT64_C)
#define INT64_C(x)                                S64_C(x)
#define UINT64_C(x)                               U64_C(x)
#endif

/* C standard macros */
#ifndef NULL
#ifdef __cplusplus
#define NULL                                      0
#else
#define NULL                                      ((void *) 0)
#endif
#endif

#ifndef TRUE
#define TRUE                                      UINT8_C(1)
#endif

#ifndef FALSE
#define FALSE                                     UINT8_C(0)
#endif

/*!
 * BMA400_INTF_RET_TYPE is the read/write interface return type which can be overwritten by the build system.
 * The default is set to int8_t.
 */
#ifndef BMA400_INTF_RET_TYPE
#define BMA400_INTF_RET_TYPE                      int8_t
#endif

/*!
 * BMA400_INTF_RET_SUCCESS is the success return value read/write interface return type which can be
 * overwritten by the build system. The default is set to 0.
 */
#ifndef BMA400_INTF_RET_SUCCESS
#define BMA400_INTF_RET_SUCCESS                   INT8_C(0)
#endif

/* API success code */
#define BMA400_OK                                 INT8_C(0)

/* API error codes */
#define BMA400_E_NULL_PTR                         INT8_C(-1)
#define BMA400_E_COM_FAIL                         INT8_C(-2)
#define BMA400_E_DEV_NOT_FOUND                    INT8_C(-3)
#define BMA400_E_INVALID_CONFIG                   INT8_C(-4)

/* API warning codes */
#define BMA400_W_SELF_TEST_FAIL                   INT8_C(1)

/* CHIP ID VALUE */
#define BMA400_CHIP_ID                            UINT8_C(0x90)

/* BMA400 I2C address macros */
#define BMA400_I2C_ADDRESS_SDO_LOW                UINT8_C(0x14)
#define BMA400_I2C_ADDRESS_SDO_HIGH               UINT8_C(0x15)

/* Power mode configurations */
#define BMA400_MODE_NORMAL                        UINT8_C(0x02)
#define BMA400_MODE_SLEEP                         UINT8_C(0x00)
#define BMA400_MODE_LOW_POWER                     UINT8_C(0x01)

/* Enable / Disable macros */
#define BMA400_DISABLE                            UINT8_C(0)
#define BMA400_ENABLE                             UINT8_C(1)

/* Data/sensortime selection macros */
#define BMA400_DATA_ONLY                          UINT8_C(0x00)
#define BMA400_DATA_SENSOR_TIME                   UINT8_C(0x01)

/* ODR configurations  */
#define BMA400_ODR_12_5HZ                         UINT8_C(0x05)
#define BMA400_ODR_25HZ                           UINT8_C(0x06)
#define BMA400_ODR_50HZ                           UINT8_C(0x07)
#define BMA400_ODR_100HZ                          UINT8_C(0x08)
#define BMA400_ODR_200HZ                          UINT8_C(0x09)
#define BMA400_ODR_400HZ                          UINT8_C(0x0A)
#define BMA400_ODR_800HZ                          UINT8_C(0x0B)

/* Accel Range configuration */
#define BMA400_RANGE_2G                           UINT8_C(0x00)
#define BMA400_RANGE_4G                           UINT8_C(0x01)
#define BMA400_RANGE_8G                           UINT8_C(0x02)
#define BMA400_RANGE_16G                          UINT8_C(0x03)

/* Accel Axes selection settings for
 * DATA SAMPLING, WAKEUP, ORIENTATION CHANGE,
 * GEN1, GEN2 , ACTIVITY CHANGE
 */
#define BMA400_AXIS_X_EN                          UINT8_C(0x01)
#define BMA400_AXIS_Y_EN                          UINT8_C(0x02)
#define BMA400_AXIS_Z_EN                          UINT8_C(0x04)
#define BMA400_AXIS_XYZ_EN                        UINT8_C(0x07)

/* Accel filter(data_src_reg) selection settings */
#define BMA400_DATA_SRC_ACCEL_FILT_1              UINT8_C(0x00)
#define BMA400_DATA_SRC_ACCEL_FILT_2              UINT8_C(0x01)
#define BMA400_DATA_SRC_ACCEL_FILT_LP             UINT8_C(0x02)

/* Accel OSR (OSR,OSR_LP) settings */
#define BMA400_ACCEL_OSR_SETTING_0                UINT8_C(0x00)
#define BMA400_ACCEL_OSR_SETTING_1                UINT8_C(0x01)
#define BMA400_ACCEL_OSR_SETTING_2                UINT8_C(0x02)
#define BMA400_ACCEL_OSR_SETTING_3                UINT8_C(0x03)

/* Accel filt1_bw settings */
/* Accel filt1_bw = 0.48 * ODR */
#define BMA400_ACCEL_FILT1_BW_0                   UINT8_C(0x00)

/* Accel filt1_bw = 0.24 * ODR */
#define BMA400_ACCEL_FILT1_BW_1                   UINT8_C(0x01)

/* Auto wake-up timeout value of 10.24s */
#define BMA400_TIMEOUT_MAX_AUTO_WAKEUP            UINT16_C(0x0FFF)

/* Auto low power timeout value of 10.24s */
#define BMA400_TIMEOUT_MAX_AUTO_LP                UINT16_C(0x0FFF)

/* Reference Update macros */
#define BMA400_UPDATE_MANUAL                      UINT8_C(0x00)
#define BMA400_UPDATE_ONE_TIME                    UINT8_C(0x01)
#define BMA400_UPDATE_EVERY_TIME                  UINT8_C(0x02)
#define BMA400_UPDATE_LP_EVERY_TIME               UINT8_C(0x03)

/* Reference Update macros for orient interrupts */
#define BMA400_ORIENT_REFU_ACC_FILT_2             UINT8_C(0x01)
#define BMA400_ORIENT_REFU_ACC_FILT_LP            UINT8_C(0x02)

/* Number of samples needed for Auto-wakeup interrupt evaluation  */
#define BMA400_SAMPLE_COUNT_1                     UINT8_C(0x00)
#define BMA400_SAMPLE_COUNT_2                     UINT8_C(0x01)
#define BMA400_SAMPLE_COUNT_3                     UINT8_C(0x02)
#define BMA400_SAMPLE_COUNT_4                     UINT8_C(0x03)
#define BMA400_SAMPLE_COUNT_5                     UINT8_C(0x04)
#define BMA400_SAMPLE_COUNT_6                     UINT8_C(0x05)
#define BMA400_SAMPLE_COUNT_7                     UINT8_C(0x06)
#define BMA400_SAMPLE_COUNT_8                     UINT8_C(0x07)

/* Auto low power configurations */
/* Auto low power timeout disabled  */
#define BMA400_AUTO_LP_TIMEOUT_DISABLE            UINT8_C(0x00)

/* Auto low power entered on drdy interrupt */
#define BMA400_AUTO_LP_DRDY_TRIGGER               UINT8_C(0x01)

/* Auto low power entered on GEN1 interrupt */
#define BMA400_AUTO_LP_GEN1_TRIGGER               UINT8_C(0x02)

/* Auto low power entered on timeout of threshold value */
#define BMA400_AUTO_LP_TIMEOUT_EN                 UINT8_C(0x04)

/* Auto low power entered on timeout of threshold value
 * but reset on activity detection
 */
#define BMA400_AUTO_LP_TIME_RESET_EN              UINT8_C(0x08)

/*    TAP INTERRUPT CONFIG MACROS   */
/* Axes select for TAP interrupt */
#define BMA400_TAP_X_AXIS_EN                      UINT8_C(0x02)
#define BMA400_TAP_Y_AXIS_EN                      UINT8_C(0x01)
#define BMA400_TAP_Z_AXIS_EN                      UINT8_C(0x00)

/* TAP tics_th setting */

/* Maximum time between upper and lower peak of a tap, in data samples
 * this time depends on the mechanics of the device tapped onto
 * default = 12 samples
 */

/* Configures 6 data samples for high-low tap signal change time */
#define BMA400_TICS_TH_6_DATA_SAMPLES             UINT8_C(0x00)

/* Configures 9 data samples for high-low tap signal change time */
#define BMA400_TICS_TH_9_DATA_SAMPLES             UINT8_C(0x01)

/* Configures 12 data samples for high-low tap signal change time */
#define BMA400_TICS_TH_12_DATA_SAMPLES            UINT8_C(0x02)

/* Configures 18 data samples for high-low tap signal change time */
#define BMA400_TICS_TH_18_DATA_SAMPLES            UINT8_C(0x03)

/* TAP Sensitivity setting */
/* It modifies the threshold for minimum TAP amplitude */
/* BMA400_TAP_SENSITIVITY_0 correspond to highest sensitivity */
#define BMA400_TAP_SENSITIVITY_0                  UINT8_C(0x00)
#define BMA400_TAP_SENSITIVITY_1                  UINT8_C(0x01)
#define BMA400_TAP_SENSITIVITY_2                  UINT8_C(0x02)
#define BMA400_TAP_SENSITIVITY_3                  UINT8_C(0x03)
#define BMA400_TAP_SENSITIVITY_4                  UINT8_C(0x04)
#define BMA400_TAP_SENSITIVITY_5                  UINT8_C(0x05)
#define BMA400_TAP_SENSITIVITY_6                  UINT8_C(0x06)

/* BMA400_TAP_SENSITIVITY_7 correspond to lowest sensitivity */
#define BMA400_TAP_SENSITIVITY_7                  UINT8_C(0x07)

/*  BMA400 TAP - quiet  settings */

/* Quiet refers to minimum quiet time before and after double tap,
 * in the data samples This time also defines the longest time interval
 * between two taps so that they are considered as double tap
 */

/* Configures 60 data samples quiet time between single or double taps */
#define BMA400_QUIET_60_DATA_SAMPLES              UINT8_C(0x00)

/* Configures 80 data samples quiet time between single or double taps */
#define BMA400_QUIET_80_DATA_SAMPLES              UINT8_C(0x01)

/* Configures 100 data samples quiet time between single or double taps */
#define BMA400_QUIET_100_DATA_SAMPLES             UINT8_C(0x02)

/* Configures 120 data samples quiet time between single or double taps */
#define BMA400_QUIET_120_DATA_SAMPLES             UINT8_C(0x03)

/*  BMA400 TAP - quiet_dt  settings */

/* quiet_dt refers to Minimum time between the two taps of a
 * double tap, in data samples
 */

/* Configures 4 data samples minimum time between double taps */
#define BMA400_QUIET_DT_4_DATA_SAMPLES            UINT8_C(0x00)

/* Configures 8 data samples minimum time between double taps */
#define BMA400_QUIET_DT_8_DATA_SAMPLES            UINT8_C(0x01)

/* Configures 12 data samples minimum time between double taps */
#define BMA400_QUIET_DT_12_DATA_SAMPLES           UINT8_C(0x02)

/* Configures 16 data samples minimum time between double taps */
#define BMA400_QUIET_DT_16_DATA_SAMPLES           UINT8_C(0x03)

/*    ACTIVITY CHANGE CONFIG MACROS   */
/* Data source for activity change detection */
#define BMA400_DATA_SRC_ACC_FILT1                 UINT8_C(0x00)
#define BMA400_DATA_SRC_ACC_FILT2                 UINT8_C(0x01)

/* Number of samples to evaluate for activity change detection */
#define BMA400_ACT_CH_SAMPLE_CNT_32               UINT8_C(0x00)
#define BMA400_ACT_CH_SAMPLE_CNT_64               UINT8_C(0x01)
#define BMA400_ACT_CH_SAMPLE_CNT_128              UINT8_C(0x02)
#define BMA400_ACT_CH_SAMPLE_CNT_256              UINT8_C(0x03)
#define BMA400_ACT_CH_SAMPLE_CNT_512              UINT8_C(0x04)

/* Interrupt pin configuration macros */
#define BMA400_INT_PUSH_PULL_ACTIVE_0             UINT8_C(0x00)
#define BMA400_INT_PUSH_PULL_ACTIVE_1             UINT8_C(0x01)
#define BMA400_INT_OPEN_DRIVE_ACTIVE_0            UINT8_C(0x02)
#define BMA400_INT_OPEN_DRIVE_ACTIVE_1            UINT8_C(0x03)

/* Interrupt Assertion status macros */
#define BMA400_ASSERTED_WAKEUP_INT                UINT16_C(0x0001)
#define BMA400_ASSERTED_ORIENT_CH                 UINT16_C(0x0002)
#define BMA400_ASSERTED_GEN1_INT                  UINT16_C(0x0004)
#define BMA400_ASSERTED_GEN2_INT                  UINT16_C(0x0008)
#define BMA400_ASSERTED_INT_OVERRUN               UINT16_C(0x0010)
#define BMA400_ASSERTED_FIFO_FULL_INT             UINT16_C(0x0020)
#define BMA400_ASSERTED_FIFO_WM_INT               UINT16_C(0x0040)
#define BMA400_ASSERTED_DRDY_INT                  UINT16_C(0x0080)
#define BMA400_ASSERTED_STEP_INT                  UINT16_C(0x0300)
#define BMA400_ASSERTED_S_TAP_INT                 UINT16_C(0x0400)
#define BMA400_ASSERTED_D_TAP_INT                 UINT16_C(0x0800)
#define BMA400_ASSERTED_ACT_CH_X                  UINT16_C(0x2000)
#define BMA400_ASSERTED_ACT_CH_Y                  UINT16_C(0x4000)
#define BMA400_ASSERTED_ACT_CH_Z                  UINT16_C(0x8000)

/* Generic interrupt criterion_sel configuration macros */
#define BMA400_ACTIVITY_INT                       UINT8_C(0x01)
#define BMA400_INACTIVITY_INT                     UINT8_C(0x00)

/* Generic interrupt axes evaluation logic configuration macros */
#define BMA400_ALL_AXES_INT                       UINT8_C(0x01)
#define BMA400_ANY_AXES_INT                       UINT8_C(0x00)

/* Generic interrupt hysteresis configuration macros */
#define BMA400_HYST_0_MG                          UINT8_C(0x00)
#define BMA400_HYST_24_MG                         UINT8_C(0x01)
#define BMA400_HYST_48_MG                         UINT8_C(0x02)
#define BMA400_HYST_96_MG                         UINT8_C(0x03)

/* BMA400 Register Address */
#define BMA400_REG_CHIP_ID                        UINT8_C(0x00)
#define BMA400_REG_STATUS                         UINT8_C(0x03)
#define BMA400_REG_ACCEL_DATA                     UINT8_C(0x04)
#define BMA400_REG_INT_STAT0                      UINT8_C(0x0E)
#define BMA400_REG_TEMP_DATA                      UINT8_C(0x11)
#define BMA400_REG_FIFO_LENGTH                    UINT8_C(0x12)
#define BMA400_REG_FIFO_DATA                      UINT8_C(0x14)
#define BMA400_REG_STEP_CNT_0                     UINT8_C(0x15)
#define BMA400_REG_STEP_CNT_1                     UINT8_C(0x16)
#define BMA400_REG_STEP_CNT_2                     UINT8_C(0x17)
#define BMA400_REG_STEP_STAT                      UINT8_C(0x18)
#define BMA400_REG_ACCEL_CONFIG_0                 UINT8_C(0x19)
#define BMA400_REG_ACCEL_CONFIG_1                 UINT8_C(0x1A)
#define BMA400_REG_ACCEL_CONFIG_2                 UINT8_C(0x1B)
#define BMA400_REG_INT_CONF_0                     UINT8_C(0x1F)
#define BMA400_REG_INT_CONF_1                     UINT8_C(0x20)
#define BMA400_REG_INT_MAP1                       UINT8_C(0x21)
#define BMA400_REG_INT_MAP2                       UINT8_C(0x22)
#define BMA400_REG_INT_MAP12                      UINT8_C(0x23)
#define BMA400_REG_INT_12_IO_CTRL                 UINT8_C(0x24)
#define BMA400_REG_FIFO_CONFIG_0                  UINT8_C(0x26)
#define BMA400_REG_FIFO_READ_EN                   UINT8_C(0x29)
#define BMA400_REG_AUTO_LOW_POW_0                 UINT8_C(0x2A)
#define BMA400_REG_AUTO_LOW_POW_1                 UINT8_C(0x2B)
#define BMA400_REG_AUTOWAKEUP_0                   UINT8_C(0x2C)
#define BMA400_REG_AUTOWAKEUP_1                   UINT8_C(0x2D)
#define BMA400_REG_WAKEUP_INT_CONF_0              UINT8_C(0x2F)
#define BMA400_REG_ORIENTCH_INT_CONFIG            UINT8_C(0x35)
#define BMA400_REG_GEN1_INT_CONFIG                UINT8_C(0x3F)
#define BMA400_REG_GEN2_INT_CONFIG                UINT8_C(0x4A)
#define BMA400_REG_ACT_CH_CONFIG_0                UINT8_C(0x55)
#define BMA400_REG_TAP_CONFIG                     UINT8_C(0x57)
#define BMA400_REG_SELF_TEST                      UINT8_C(0x7D)
#define BMA400_REG_COMMAND                        UINT8_C(0x7E)

/* BMA400 Command register */
#define BMA400_SOFT_RESET_CMD                     UINT8_C(0xB6)
#define BMA400_FIFO_FLUSH_CMD                     UINT8_C(0xB0)

/* BMA400 Delay definitions */
#define BMA400_DELAY_US_SOFT_RESET                UINT8_C(5000)
#define BMA400_DELAY_US_SELF_TEST                 UINT8_C(7000)
#define BMA400_DELAY_US_SELF_TEST_DATA_READ       UINT8_C(50000)

/* Interface selection macro */
#define BMA400_SPI_WR_MASK                        UINT8_C(0x7F)
#define BMA400_SPI_RD_MASK                        UINT8_C(0x80)

/* UTILITY MACROS */
#define BMA400_SET_LOW_BYTE                       UINT16_C(0x00FF)
#define BMA400_SET_HIGH_BYTE                      UINT16_C(0xFF00)

/* Interrupt mapping selection */
#define BMA400_DATA_READY_INT_MAP                 UINT8_C(0x01)
#define BMA400_FIFO_WM_INT_MAP                    UINT8_C(0x02)
#define BMA400_FIFO_FULL_INT_MAP                  UINT8_C(0x03)
#define BMA400_GEN2_INT_MAP                       UINT8_C(0x04)
#define BMA400_GEN1_INT_MAP                       UINT8_C(0x05)
#define BMA400_ORIENT_CH_INT_MAP                  UINT8_C(0x06)
#define BMA400_WAKEUP_INT_MAP                     UINT8_C(0x07)
#define BMA400_ACT_CH_INT_MAP                     UINT8_C(0x08)
#define BMA400_TAP_INT_MAP                        UINT8_C(0x09)
#define BMA400_STEP_INT_MAP                       UINT8_C(0x0A)
#define BMA400_INT_OVERRUN_MAP                    UINT8_C(0x0B)

/* BMA400 FIFO configurations */
#define BMA400_FIFO_AUTO_FLUSH                    UINT8_C(0x01)
#define BMA400_FIFO_STOP_ON_FULL                  UINT8_C(0x02)
#define BMA400_FIFO_TIME_EN                       UINT8_C(0x04)
#define BMA400_FIFO_DATA_SRC                      UINT8_C(0x08)
#define BMA400_FIFO_8_BIT_EN                      UINT8_C(0x10)
#define BMA400_FIFO_X_EN                          UINT8_C(0x20)
#define BMA400_FIFO_Y_EN                          UINT8_C(0x40)
#define BMA400_FIFO_Z_EN                          UINT8_C(0x80)

/* BMA400 FIFO data configurations */
#define BMA400_FIFO_EN_X                          UINT8_C(0x01)
#define BMA400_FIFO_EN_Y                          UINT8_C(0x02)
#define BMA400_FIFO_EN_Z                          UINT8_C(0x04)
#define BMA400_FIFO_EN_XY                         UINT8_C(0x03)
#define BMA400_FIFO_EN_YZ                         UINT8_C(0x06)
#define BMA400_FIFO_EN_XZ                         UINT8_C(0x05)
#define BMA400_FIFO_EN_XYZ                        UINT8_C(0x07)

/* BMA400 Self test configurations */
#define BMA400_SELF_TEST_DISABLE                  UINT8_C(0x00)
#define BMA400_SELF_TEST_ENABLE_POSITIVE          UINT8_C(0x07)
#define BMA400_SELF_TEST_ENABLE_NEGATIVE          UINT8_C(0x0F)

/* BMA400 FIFO data masks */
#define BMA400_FIFO_HEADER_MASK                   UINT8_C(0x3E)
#define BMA400_FIFO_BYTES_OVERREAD                UINT8_C(25)
#define BMA400_AWIDTH_MASK                        UINT8_C(0xEF)
#define BMA400_FIFO_DATA_EN_MASK                  UINT8_C(0x0E)

/* BMA400 Step status field - Activity status */
#define BMA400_STILL_ACT                          UINT8_C(0x00)
#define BMA400_WALK_ACT                           UINT8_C(0x01)
#define BMA400_RUN_ACT                            UINT8_C(0x02)

/* It is inserted when FIFO_CONFIG0.fifo_data_src
 * is changed during the FIFO read
 */
#define BMA400_FIFO_CONF0_CHANGE                  UINT8_C(0x01)

/* It is inserted when ACC_CONFIG0.filt1_bw
 * is changed during the FIFO read
 */
#define BMA400_ACCEL_CONF0_CHANGE                 UINT8_C(0x02)

/* It is inserted when ACC_CONFIG1.acc_range
 * acc_odr or osr is changed during the FIFO read
 */
#define BMA400_ACCEL_CONF1_CHANGE                 UINT8_C(0x04)

/* Accel width setting either 12/8 bit mode */
#define BMA400_12_BIT_FIFO_DATA                   UINT8_C(0x01)
#define BMA400_8_BIT_FIFO_DATA                    UINT8_C(0x00)

/* BMA400 FIFO header configurations */
#define BMA400_FIFO_SENSOR_TIME                   UINT8_C(0xA0)
#define BMA400_FIFO_EMPTY_FRAME                   UINT8_C(0x80)
#define BMA400_FIFO_CONTROL_FRAME                 UINT8_C(0x48)
#define BMA400_FIFO_XYZ_ENABLE                    UINT8_C(0x8E)
#define BMA400_FIFO_X_ENABLE                      UINT8_C(0x82)
#define BMA400_FIFO_Y_ENABLE                      UINT8_C(0x84)
#define BMA400_FIFO_Z_ENABLE                      UINT8_C(0x88)
#define BMA400_FIFO_XY_ENABLE                     UINT8_C(0x86)
#define BMA400_FIFO_YZ_ENABLE                     UINT8_C(0x8C)
#define BMA400_FIFO_XZ_ENABLE                     UINT8_C(0x8A)

/* BMA400 bit mask definitions */
#define BMA400_POWER_MODE_STATUS_MSK              UINT8_C(0x06)
#define BMA400_POWER_MODE_STATUS_POS              UINT8_C(1)

#define BMA400_POWER_MODE_MSK                     UINT8_C(0x03)

#define BMA400_ACCEL_ODR_MSK                      UINT8_C(0x0F)

#define BMA400_ACCEL_RANGE_MSK                    UINT8_C(0xC0)
#define BMA400_ACCEL_RANGE_POS                    UINT8_C(6)

#define BMA400_DATA_FILTER_MSK                    UINT8_C(0x0C)
#define BMA400_DATA_FILTER_POS                    UINT8_C(2)

#define BMA400_OSR_MSK                            UINT8_C(0x30)
#define BMA400_OSR_POS                            UINT8_C(4)

#define BMA400_OSR_LP_MSK                         UINT8_C(0x60)
#define BMA400_OSR_LP_POS                         UINT8_C(5)

#define BMA400_FILT_1_BW_MSK                      UINT8_C(0x80)
#define BMA400_FILT_1_BW_POS                      UINT8_C(7)

#define BMA400_WAKEUP_TIMEOUT_MSK                 UINT8_C(0x04)
#define BMA400_WAKEUP_TIMEOUT_POS                 UINT8_C(2)

#define BMA400_WAKEUP_THRES_LSB_MSK               UINT16_C(0x000F)

#define BMA400_WAKEUP_THRES_MSB_MSK               UINT16_C(0x0FF0)
#define BMA400_WAKEUP_THRES_MSB_POS               UINT8_C(4)

#define BMA400_WAKEUP_TIMEOUT_THRES_MSK           UINT8_C(0xF0)
#define BMA400_WAKEUP_TIMEOUT_THRES_POS           UINT8_C(4)

#define BMA400_WAKEUP_INTERRUPT_MSK               UINT8_C(0x02)
#define BMA400_WAKEUP_INTERRUPT_POS               UINT8_C(1)

#define BMA400_AUTO_LOW_POW_MSK                   UINT8_C(0x0F)

#define BMA400_AUTO_LP_THRES_MSK                  UINT16_C(0x0FF0)
#define BMA400_AUTO_LP_THRES_POS                  UINT8_C(4)

#define BMA400_AUTO_LP_THRES_LSB_MSK              UINT16_C(0x000F)

#define BMA400_WKUP_REF_UPDATE_MSK                UINT8_C(0x03)

#define BMA400_AUTO_LP_TIMEOUT_LSB_MSK            UINT8_C(0xF0)
#define BMA400_AUTO_LP_TIMEOUT_LSB_POS            UINT8_C(4)

#define BMA400_SAMPLE_COUNT_MSK                   UINT8_C(0x1C)
#define BMA400_SAMPLE_COUNT_POS                   UINT8_C(2)

#define BMA400_WAKEUP_EN_AXES_MSK                 UINT8_C(0xE0)
#define BMA400_WAKEUP_EN_AXES_POS                 UINT8_C(5)

#define BMA400_TAP_AXES_EN_MSK                    UINT8_C(0x18)
#define BMA400_TAP_AXES_EN_POS                    UINT8_C(3)

#define BMA400_TAP_QUIET_DT_MSK                   UINT8_C(0x30)
#define BMA400_TAP_QUIET_DT_POS                   UINT8_C(4)

#define BMA400_TAP_QUIET_MSK                      UINT8_C(0x0C)
#define BMA400_TAP_QUIET_POS                      UINT8_C(2)

#define BMA400_TAP_TICS_TH_MSK                    UINT8_C(0x03)

#define BMA400_TAP_SENSITIVITY_MSK                UINT8_C(0X07)

#define BMA400_ACT_CH_AXES_EN_MSK                 UINT8_C(0xE0)
#define BMA400_ACT_CH_AXES_EN_POS                 UINT8_C(5)

#define BMA400_ACT_CH_DATA_SRC_MSK                UINT8_C(0x10)
#define BMA400_ACT_CH_DATA_SRC_POS                UINT8_C(4)

#define BMA400_ACT_CH_NPTS_MSK                    UINT8_C(0x0F)

#define BMA400_INT_AXES_EN_MSK                    UINT8_C(0xE0)
#define BMA400_INT_AXES_EN_POS                    UINT8_C(5)

#define BMA400_INT_DATA_SRC_MSK                   UINT8_C(0x10)
#define BMA400_INT_DATA_SRC_POS                   UINT8_C(4)

#define BMA400_INT_REFU_MSK                       UINT8_C(0x0C)
#define BMA400_INT_REFU_POS                       UINT8_C(2)

#define BMA400_INT_HYST_MSK                       UINT8_C(0x03)

#define BMA400_GEN_INT_COMB_MSK                   UINT8_C(0x01)

#define BMA400_GEN_INT_CRITERION_MSK              UINT8_C(0x02)
#define BMA400_GEN_INT_CRITERION_POS              UINT8_C(0x01)

#define BMA400_INT_PIN1_CONF_MSK                  UINT8_C(0x06)
#define BMA400_INT_PIN1_CONF_POS                  UINT8_C(1)

#define BMA400_INT_PIN2_CONF_MSK                  UINT8_C(0x60)
#define BMA400_INT_PIN2_CONF_POS                  UINT8_C(5)

#define BMA400_INT_STATUS_MSK                     UINT8_C(0xE0)
#define BMA400_INT_STATUS_POS                     UINT8_C(5)

#define BMA400_EN_DRDY_MSK                        UINT8_C(0x80)
#define BMA400_EN_DRDY_POS                        UINT8_C(7)

#define BMA400_EN_FIFO_WM_MSK                     UINT8_C(0x40)
#define BMA400_EN_FIFO_WM_POS                     UINT8_C(6)

#define BMA400_EN_FIFO_FULL_MSK                   UINT8_C(0x20)
#define BMA400_EN_FIFO_FULL_POS                   UINT8_C(5)

#define BMA400_EN_INT_OVERRUN_MSK                 UINT8_C(0x10)
#define BMA400_EN_INT_OVERRUN_POS                 UINT8_C(4)

#define BMA400_EN_GEN2_MSK                        UINT8_C(0x08)
#define BMA400_EN_GEN2_POS                        UINT8_C(3)

#define BMA400_EN_GEN1_MSK                        UINT8_C(0x04)
#define BMA400_EN_GEN1_POS                        UINT8_C(2)

#define BMA400_EN_ORIENT_CH_MSK                   UINT8_C(0x02)
#define BMA400_EN_ORIENT_CH_POS                   UINT8_C(1)

#define BMA400_EN_LATCH_MSK                       UINT8_C(0x80)
#define BMA400_EN_LATCH_POS                       UINT8_C(7)

#define BMA400_EN_ACTCH_MSK                       UINT8_C(0x10)
#define BMA400_EN_ACTCH_POS                       UINT8_C(4)

#define BMA400_EN_D_TAP_MSK                       UINT8_C(0x08)
#define BMA400_EN_D_TAP_POS                       UINT8_C(3)

#define BMA400_EN_S_TAP_MSK                       UINT8_C(0x04)
#define BMA400_EN_S_TAP_POS                       UINT8_C(2)

#define BMA400_EN_STEP_INT_MSK                    UINT8_C(0x01)

#define BMA400_STEP_MAP_INT2_MSK                  UINT8_C(0x10)
#define BMA400_STEP_MAP_INT2_POS                  UINT8_C(4)

#define BMA400_EN_WAKEUP_INT_MSK                  UINT8_C(0x01)

#define BMA400_TAP_MAP_INT1_MSK                   UINT8_C(0x04)
#define BMA400_TAP_MAP_INT1_POS                   UINT8_C(2)

#define BMA400_TAP_MAP_INT2_MSK                   UINT8_C(0x40)
#define BMA400_TAP_MAP_INT2_POS                   UINT8_C(6)

#define BMA400_ACTCH_MAP_INT1_MSK                 UINT8_C(0x08)
#define BMA400_ACTCH_MAP_INT1_POS                 UINT8_C(3)

#define BMA400_ACTCH_MAP_INT2_MSK                 UINT8_C(0x80)
#define BMA400_ACTCH_MAP_INT2_POS                 UINT8_C(7)

#define BMA400_FIFO_BYTES_CNT_MSK                 UINT8_C(0x07)

#define BMA400_FIFO_TIME_EN_MSK                   UINT8_C(0x04)
#define BMA400_FIFO_TIME_EN_POS                   UINT8_C(2)

#define BMA400_FIFO_AXES_EN_MSK                   UINT8_C(0xE0)
#define BMA400_FIFO_AXES_EN_POS                   UINT8_C(5)

#define BMA400_FIFO_8_BIT_EN_MSK                  UINT8_C(0x10)
#define BMA400_FIFO_8_BIT_EN_POS                  UINT8_C(4)

/* Macro to SET and GET BITS of a register */
#define BMA400_SET_BITS(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) |   \
     ((data << bitname##_POS) & bitname##_MSK))

#define BMA400_GET_BITS(reg_data, bitname)        ((reg_data & (bitname##_MSK)) >> \
                                                   (bitname##_POS))

#define BMA400_SET_BITS_POS_0(reg_data, bitname, data) \
    ((reg_data & ~(bitname##_MSK)) |         \
     (data & bitname##_MSK))

#define BMA400_GET_BITS_POS_0(reg_data, bitname)  (reg_data & (bitname##_MSK))

#define BMA400_SET_BIT_VAL_0(reg_data, bitname)   (reg_data & ~(bitname##_MSK))

#define BMA400_GET_LSB(var)                       (uint8_t)(var & BMA400_SET_LOW_BYTE)
#define BMA400_GET_MSB(var)                       (uint8_t)((var & BMA400_SET_HIGH_BYTE) >> 8)

/* Macros used for Self test */

/*
 * Derivation of values obtained by :
 * Signal_Diff = ( (LSB/g value based on Accel Range) * (Minimum difference signal value) ) / 1000
 */

/* Self-test: Resulting minimum difference signal for BMA400 with Range 4G */

#define BMA400_ST_ACC_X_AXIS_SIGNAL_DIFF          UINT16_C(768)
#define BMA400_ST_ACC_Y_AXIS_SIGNAL_DIFF          UINT16_C(614)
#define BMA400_ST_ACC_Z_AXIS_SIGNAL_DIFF          UINT16_C(128)

/*
 * Interface selection enums
 */
enum bma400_intf {
    /* SPI interface */
    BMA400_SPI_INTF,
    /* I2C interface */
    BMA400_I2C_INTF
};

/********************************************************* */
/*!               Function Pointers                       */
/********************************************************* */

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific read functions of the user
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data from the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 */
typedef BMA400_INTF_RET_TYPE (*bma400_read_fptr_t)(uint8_t reg_addr, uint8_t *reg_data, uint32_t length,
                                                   void *intf_ptr);

/*!
 * @brief Bus communication function pointer which should be mapped to
 * the platform specific write functions of the user
 *
 * @param[in]     reg_addr : 8bit register address of the sensor
 * @param[out]    reg_data : Data to the specified address
 * @param[in]     length   : Length of the reg_data array
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 * @retval 0 for Success
 * @retval Non-zero for Failure
 *
 */
typedef BMA400_INTF_RET_TYPE (*bma400_write_fptr_t)(uint8_t reg_addr, const uint8_t *reg_data, uint32_t length,
                                                    void *intf_ptr);

/*!
 * @brief Delay function pointer which should be mapped to
 * delay function of the user
 *
 * @param period - The time period in microseconds
 * @param[in,out] intf_ptr : Void pointer that can enable the linking of descriptors
 *                           for interface related callbacks
 */
typedef void (*bma400_delay_us_fptr_t)(uint32_t period, void *intf_ptr);

/*
 * Sensor selection enums
 */
enum bma400_sensor {
    BMA400_ACCEL,
    BMA400_TAP_INT,
    BMA400_ACTIVITY_CHANGE_INT,
    BMA400_GEN1_INT,
    BMA400_GEN2_INT,
    BMA400_ORIENT_CHANGE_INT,
    BMA400_STEP_COUNTER_INT
};

/*
 * Interrupt channel selection enums
 */
enum bma400_int_chan {
    BMA400_UNMAP_INT_PIN,
    BMA400_INT_CHANNEL_1,
    BMA400_INT_CHANNEL_2,
    BMA400_MAP_BOTH_INT_PINS
};

/*
 * Interrupt pin hardware configurations
 */
struct bma400_int_pin_conf
{
    /* Interrupt channel selection enums */
    enum bma400_int_chan int_chan;

    /* Interrupt pin configuration
     * Assignable Macros :
     *  - BMA400_INT_PUSH_PULL_ACTIVE_0
     *  - BMA400_INT_PUSH_PULL_ACTIVE_1
     *  - BMA400_INT_OPEN_DRIVE_ACTIVE_0
     *  - BMA400_INT_OPEN_DRIVE_ACTIVE_1
     */
    uint8_t pin_conf;
};

/*
 * Accel basic configuration
 */
struct bma400_acc_conf
{
    /* Output data rate
     * Assignable macros :
     *  - BMA400_ODR_12_5HZ  - BMA400_ODR_25HZ   - BMA400_ODR_50HZ
     *  - BMA400_ODR_100HZ   - BMA400_ODR_200HZ  - BMA400_ODR_400HZ
     *  - BMA400_ODR_800HZ
     */
    uint8_t odr;

    /* Range of sensor
     * Assignable macros :
     *  - BMA400_2G_RANGE   - BMA400_8G_RANGE
     *  - BMA400_4G_RANGE   - BMA400_16G_RANGE
     */
    uint8_t range;

    /* Filter setting for data source
     * Assignable Macros :
     * - BMA400_DATA_SRC_ACCEL_FILT_1
     * - BMA400_DATA_SRC_ACCEL_FILT_2
     * - BMA400_DATA_SRC_ACCEL_FILT_LP
     */
    uint8_t data_src;

    /* Assignable Macros for osr and osr_lp:
     * - BMA400_ACCEL_OSR_SETTING_0     - BMA400_ACCEL_OSR_SETTING_2
     * - BMA400_ACCEL_OSR_SETTING_1     - BMA400_ACCEL_OSR_SETTING_3
     */

    /* OSR setting for data source */
    uint8_t osr;

    /* OSR setting for low power mode */
    uint8_t osr_lp;

    /* Filter 1 Bandwidth
     * Assignable macros :
     *  - BMA400_ACCEL_FILT1_BW_0
     *  - BMA400_ACCEL_FILT1_BW_1
     */
    uint8_t filt1_bw;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * Tap interrupt configurations
 */
struct bma400_tap_conf
{
    /* Axes enabled to sense tap setting
     * Assignable macros :
     *   - BMA400_X_AXIS_EN_TAP
     *   - BMA400_Y_AXIS_EN_TAP
     *   - BMA400_Z_AXIS_EN_TAP
     */
    uint8_t axes_sel;

    /* TAP sensitivity settings modifies the threshold
     *  for minimum TAP amplitude
     * Assignable macros :
     *   - BMA400_TAP_SENSITIVITY_0  - BMA400_TAP_SENSITIVITY_4
     *   - BMA400_TAP_SENSITIVITY_1  - BMA400_TAP_SENSITIVITY_5
     *   - BMA400_TAP_SENSITIVITY_2  - BMA400_TAP_SENSITIVITY_6
     *   - BMA400_TAP_SENSITIVITY_3  - BMA400_TAP_SENSITIVITY_7
     *
     * @note :
     *  - BMA400_TAP_SENSITIVITY_0 correspond to highest sensitivity
     *  - BMA400_TAP_SENSITIVITY_7 correspond to lowest sensitivity
     */
    uint8_t sensitivity;

    /* TAP tics_th setting is the maximum time between upper and lower
     * peak of a tap, in data samples, This time depends on the
     * mechanics of the device tapped onto  default = 12 samples
     * Assignable macros :
     *   - BMA400_TICS_TH_6_DATA_SAMPLES
     *   - BMA400_TICS_TH_9_DATA_SAMPLES
     *   - BMA400_TICS_TH_12_DATA_SAMPLES
     *   - BMA400_TICS_TH_18_DATA_SAMPLES
     */
    uint8_t tics_th;

    /* BMA400 TAP - quiet  settings to configure minimum quiet time
     *  before and after double tap, in the data samples.
     * This time also defines the longest time interval between two
     * taps so that they are considered as double tap
     * Assignable macros :
     *   - BMA400_QUIET_60_DATA_SAMPLES
     *   - BMA400_QUIET_80_DATA_SAMPLES
     *   - BMA400_QUIET_100_DATA_SAMPLES
     *   - BMA400_QUIET_120_DATA_SAMPLES
     */
    uint8_t quiet;

    /* BMA400 TAP - quiet_dt  settings
     * quiet_dt refers to Minimum time between the two taps of a
     * double tap, in data samples
     * Assignable macros :
     *   - BMA400_QUIET_DT_4_DATA_SAMPLES
     *   - BMA400_QUIET_DT_8_DATA_SAMPLES
     *   - BMA400_QUIET_DT_12_DATA_SAMPLES
     *   - BMA400_QUIET_DT_16_DATA_SAMPLES
     */
    uint8_t quiet_dt;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * Activity change interrupt configurations
 */
struct bma400_act_ch_conf
{
    /* Threshold for activity change (8 mg/LSB) */
    uint8_t act_ch_thres;

    /* Axes enabled to sense activity change
     * Assignable macros :
     *   - BMA400_X_AXIS_EN
     *   - BMA400_Y_AXIS_EN
     *   - BMA400_Z_AXIS_EN
     *   - BMA400_XYZ_AXIS_EN
     */
    uint8_t axes_sel;

    /* Data Source for activity change
     * Assignable macros :
     *    - BMA400_DATA_SRC_ACC_FILT1
     *    - BMA400_DATA_SRC_ACC_FILT2
     */
    uint8_t data_source;

    /* Sample count for sensing act_ch
     * Assignable macros :
     *  - BMA400_ACT_CH_SAMPLE_CNT_32
     *  - BMA400_ACT_CH_SAMPLE_CNT_64
     *  - BMA400_ACT_CH_SAMPLE_CNT_128
     *  - BMA400_ACT_CH_SAMPLE_CNT_256
     *  - BMA400_ACT_CH_SAMPLE_CNT_512
     */
    uint8_t act_ch_ntps;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * Generic interrupt configurations
 */
struct bma400_gen_int_conf
{
    /* Threshold for the gen1 interrupt (1 LSB = 8mg)
     * if gen_int_thres = 10, then threshold = 10 * 8 = 80mg
     */
    uint8_t gen_int_thres;

    /* Duration for which the condition has to persist until
     *  interrupt can be triggered
     *  duration is measured in data samples of selected data source
     */
    uint16_t gen_int_dur;

    /* Enable axes to sense for the gen1 interrupt
     * Assignable macros :
     *  - BMA400_X_AXIS_EN
     *  - BMA400_Y_AXIS_EN
     *  - BMA400_Z_AXIS_EN
     *  - BMA400_XYZ_AXIS_EN
     */
    uint8_t axes_sel;

    /* Data source to sense for the gen1 interrupt
     * Assignable macros :
     *  - BMA400_DATA_SRC_ACC_FILT1
     *  - BMA400_DATA_SRC_ACC_FILT2
     */
    uint8_t data_src;

    /* Activity/Inactivity selection macros
     * Assignable macros :
     *  - BMA400_ACTIVITY_INT
     *  - BMA400_INACTIVITY_INT
     */
    uint8_t criterion_sel;

    /* Axes selection logic macros
     * Assignable macros :
     *  - BMA400_ALL_AXES_INT
     *  - BMA400_ANY_AXES_INT
     */
    uint8_t evaluate_axes;

    /* Reference x,y,z values updates
     * Assignable macros :
     *  - BMA400_MANUAL_UPDATE
     *  - BMA400_ONE_TIME_UPDATE
     *  - BMA400_EVERY_TIME_UPDATE
     *  - BMA400_LP_EVERY_TIME_UPDATE
     */
    uint8_t ref_update;

    /* Hysteresis value
     * Higher the hysteresis value, Lower the value of noise
     * Assignable macros :
     *  - BMA400_HYST_0_MG
     *  - BMA400_HYST_24_MG
     *  - BMA400_HYST_48_MG
     *  - BMA400_HYST_96_MG
     */
    uint8_t hysteresis;

    /* Threshold value for x axes */
    uint16_t int_thres_ref_x;

    /* Threshold value for y axes */
    uint16_t int_thres_ref_y;

    /* Threshold value for z axes */
    uint16_t int_thres_ref_z;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * Orient interrupt configurations
 */
struct bma400_orient_int_conf
{
    /* Enable axes to sense for the gen1 interrupt
     * Assignable macros :
     *  - BMA400_X_AXIS_EN
     *  - BMA400_Y_AXIS_EN
     *  - BMA400_Z_AXIS_EN
     *  - BMA400_XYZ_AXIS_EN
     */
    uint8_t axes_sel;

    /* Data source to sense for the gen1 interrupt
     * Assignable macros :
     *  - BMA400_DATA_SRC_ACC_FILT1
     *  - BMA400_DATA_SRC_ACC_FILT2
     */
    uint8_t data_src;

    /* Reference x,y,z values updates
     * Assignable macros :
     *  - BMA400_MANUAL_UPDATE
     *  - BMA400_ORIENT_REFU_ACC_FILT_2
     *  - BMA400_ORIENT_REFU_ACC_FILT_LP
     */
    uint8_t ref_update;

    /* Threshold for the orient interrupt (1 LSB = 8mg)
     * if orient_thres = 10, then threshold = 10 * 8 = 80mg
     */
    uint8_t orient_thres;

    /* Threshold to check for stability (1 LSB = 8mg)
     * if stability_thres = 10, then threshold = 10 * 8 = 80mg
     */
    uint8_t stability_thres;

    /* orient_int_dur duration in which orient interrupt
     * should occur, It is 8bit value configurable at 10ms/LSB.
     */
    uint8_t orient_int_dur;

    /* Reference value for x axes */
    uint16_t orient_ref_x;

    /* Reference value for y axes */
    uint16_t orient_ref_y;

    /* Reference value for z axes */
    uint16_t orient_ref_z;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/* Step counter configurations */
struct bma400_step_int_conf
{
    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * Union of sensor Configurations
 */
union bma400_set_param
{
    /* Accel configurations */
    struct bma400_acc_conf accel;

    /* TAP configurations */
    struct bma400_tap_conf tap;

    /* Activity change configurations */
    struct bma400_act_ch_conf act_ch;

    /* Generic interrupt configurations */
    struct bma400_gen_int_conf gen_int;

    /* Orient configurations */
    struct bma400_orient_int_conf orient;

    /* Step counter configurations */
    struct bma400_step_int_conf step_cnt;
};

/*
 * Sensor selection and their configurations
 */
struct bma400_sensor_conf
{
    /* Sensor selection */
    enum bma400_sensor type;

    /* Sensor configuration */
    union bma400_set_param param;
};

/*
 * enum to select device settings
 */
enum bma400_device {
    BMA400_AUTOWAKEUP_TIMEOUT,
    BMA400_AUTOWAKEUP_INT,
    BMA400_AUTO_LOW_POWER,
    BMA400_INT_PIN_CONF,
    BMA400_INT_OVERRUN_CONF,
    BMA400_FIFO_CONF
};

/*
 * BMA400 auto-wakeup configurations
 */
struct bma400_auto_wakeup_conf
{
    /* Enable auto wake-up by using timeout threshold
     * Assignable Macros :
     *   - BMA400_ENABLE    - BMA400_DISABLE
     */
    uint8_t wakeup_timeout;

    /* Timeout threshold after which auto wake-up occurs
     * It is 12bit value configurable at 2.5ms/LSB
     * Maximum timeout is 10.24s (4096 * 2.5) for
     * which the assignable macro is :
     *      - BMA400_AUTO_WAKEUP_TIMEOUT_MAX
     */
    uint16_t timeout_thres;
};

/*
 * BMA400 wakeup configurations
 */
struct bma400_wakeup_conf
{
    /* Wakeup reference update
     *  Assignable macros:
     *   - BMA400_MANUAL_UPDATE
     *   - BMA400_ONE_TIME_UPDATE
     *   - BMA400_EVERY_TIME_UPDATE
     */
    uint8_t wakeup_ref_update;

    /* Number of samples for interrupt condition evaluation
     * Assignable Macros :
     *  - BMA400_SAMPLE_COUNT_1  - BMA400_SAMPLE_COUNT_5
     *  - BMA400_SAMPLE_COUNT_2  - BMA400_SAMPLE_COUNT_6
     *  - BMA400_SAMPLE_COUNT_3  - BMA400_SAMPLE_COUNT_7
     *  - BMA400_SAMPLE_COUNT_4  - BMA400_SAMPLE_COUNT_8
     */
    uint8_t sample_count;

    /* Enable low power wake-up interrupt for X(BIT 0), Y(BIT 1), Z(BIT 2)
     * axes  0 - not active; 1 - active
     * Assignable macros :
     *  - BMA400_X_AXIS_EN
     *  - BMA400_Y_AXIS_EN
     *  - BMA400_Z_AXIS_EN
     *  - BMA400_XYZ_AXIS_EN
     */
    uint8_t wakeup_axes_en;

    /* Interrupt threshold configuration  */
    uint8_t int_wkup_threshold;

    /* Reference acceleration x-axis for the wake-up interrupt */
    uint8_t int_wkup_ref_x;

    /* Reference acceleration y-axis for the wake-up interrupt */
    uint8_t int_wkup_ref_y;

    /* Reference acceleration z-axis for the wake-up interrupt */
    uint8_t int_wkup_ref_z;

    /* Interrupt channel to be mapped */
    enum bma400_int_chan int_chan;
};

/*
 * BMA400 auto-low power configurations
 */
struct bma400_auto_lp_conf
{
    /* Enable auto low power mode using  data ready interrupt /
     * Genric interrupt1 / timeout counter value
     * Assignable macros :
     * - BMA400_AUTO_LP_DRDY_TRIGGER
     * - BMA400_AUTO_LP_GEN1_TRIGGER
     * - BMA400_AUTO_LP_TIMEOUT_EN
     * - BMA400_AUTO_LP_TIME_RESET_EN
     * - BMA400_AUTO_LP_TIMEOUT_DISABLE
     */
    uint8_t auto_low_power_trigger;

    /* Timeout threshold after which auto wake-up occurs
     * It is 12bit value configurable at 2.5ms/LSB
     * Maximum timeout is 10.24s (4096 * 2.5) for
     *  which the assignable macro is :
     *  - BMA400_AUTO_LP_TIMEOUT_MAX
     */
    uint16_t auto_lp_timeout_threshold;
};

/*
 * FIFO configurations
 */
struct bma400_fifo_conf
{
    /* Select FIFO configurations to enable/disable
     * Assignable Macros :
     *   - BMA400_FIFO_AUTO_FLUSH
     *   - BMA400_FIFO_STOP_ON_FULL
     *   - BMA400_FIFO_TIME_EN
     *   - BMA400_FIFO_DATA_SRC
     *   - BMA400_FIFO_8_BIT_EN
     *   - BMA400_FIFO_X_EN
     *   - BMA400_FIFO_Y_EN
     *   - BMA400_FIFO_Z_EN
     */
    uint8_t conf_regs;

    /* Enable/ disable selected FIFO configurations
     * Assignable Macros :
     *   - BMA400_ENABLE
     *   - BMA400_DISABLE
     */
    uint8_t conf_status;

    /* Value to set the water-mark */
    uint16_t fifo_watermark;

    /* Interrupt pin mapping for FIFO full interrupt */
    enum bma400_int_chan fifo_full_channel;

    /* Interrupt pin mapping for FIFO water-mark interrupt */
    enum bma400_int_chan fifo_wm_channel;
};

/*
 * Interrupt overrun configurations
 */
struct bma400_int_overrun
{
    /* Interrupt pin mapping for interrupt overrun */
    enum bma400_int_chan int_chan;
};

/*
 * Union of device configuration parameters
 */
union bma400_device_params
{
    /* Auto wakeup configurations */
    struct bma400_auto_wakeup_conf auto_wakeup;

    /* Wakeup interrupt configurations */
    struct bma400_wakeup_conf wakeup;

    /* Auto Low power configurations */
    struct bma400_auto_lp_conf auto_lp;

    /* Interrupt pin configurations */
    struct bma400_int_pin_conf int_conf;

    /* FIFO configuration */
    struct bma400_fifo_conf fifo_conf;

    /* Interrupt overrun configuration */
    struct bma400_int_overrun overrun_int;
};

/*
 * BMA400 device configuration
 */
struct bma400_device_conf
{
    /* Device feature selection */
    enum bma400_device type;

    /* Device feature configuration */
    union bma400_device_params param;
};

/*
 * BMA400 sensor data
 */
struct bma400_sensor_data
{
    /* X-axis sensor data */
    int16_t x;

    /* Y-axis sensor data */
    int16_t y;

    /* Z-axis sensor data */
    int16_t z;

    /* sensor time */
    uint32_t sensortime;
};

/*
 * BMA400 interrupt selection
 */
enum bma400_int_type {
    /* DRDY interrupt */
    BMA400_DRDY_INT_EN,
    /* FIFO watermark interrupt */
    BMA400_FIFO_WM_INT_EN,
    /* FIFO full interrupt */
    BMA400_FIFO_FULL_INT_EN,
    /* Generic interrupt 2 */
    BMA400_GEN2_INT_EN,
    /* Generic interrupt 1 */
    BMA400_GEN1_INT_EN,
    /* Orient change interrupt */
    BMA400_ORIENT_CHANGE_INT_EN,
    /* Latch interrupt */
    BMA400_LATCH_INT_EN,
    /* Activity change interrupt */
    BMA400_ACTIVITY_CHANGE_INT_EN,
    /* Double tap interrupt */
    BMA400_DOUBLE_TAP_INT_EN,
    /* Single tap interrupt */
    BMA400_SINGLE_TAP_INT_EN,
    /* Step interrupt */
    BMA400_STEP_COUNTER_INT_EN,
    /* Auto wakeup interrupt */
    BMA400_AUTO_WAKEUP_EN
};

/*
 * Interrupt enable/disable configurations
 */
struct bma400_int_enable
{
    /* Enum to choose the interrupt to be enabled */
    enum bma400_int_type type;

    /* Enable/ disable selected interrupts
     * Assignable Macros :
     *   - BMA400_ENABLE
     *   - BMA400_DISABLE
     */
    uint8_t conf;
};
struct bma400_fifo_data
{
    /* Data buffer of user defined length is to be mapped here */
    uint8_t *data;

    /* While calling the API  "bma400_get_fifo_data" , length stores
     *  number of bytes in FIFO to be read (specified by user as input)
     *  and after execution of the API ,number of FIFO data bytes
     *  available is provided as an output to user
     */
    uint16_t length;

    /* FIFO time enable */
    uint8_t fifo_time_enable;

    /* FIFO 8bit mode enable */
    uint8_t fifo_8_bit_en;

    /* Streaming of the Accelerometer data for selected x,y,z axes
     *   - BMA400_FIFO_X_EN
     *   - BMA400_FIFO_Y_EN
     *   - BMA400_FIFO_Z_EN
     */
    uint8_t fifo_data_enable;

    /* Will be equal to length when no more frames are there to parse */
    uint16_t accel_byte_start_idx;

    /* It stores the value of configuration changes
     * in sensor during FIFO read
     */
    uint8_t conf_change;

    /* Value of FIFO sensor time time */
    uint32_t fifo_sensor_time;
};

/*
 * bma400 device structure
 */
struct bma400_dev
{
    /* Chip Id */
    uint8_t chip_id;

    /* SPI/I2C Interface selection */
    enum bma400_intf intf;

    /*!
     * The interface pointer is used to enable the user
     * to link their interface descriptors for reference during the
     * implementation of the read and write interfaces to the
     * hardware.
     */
    void *intf_ptr;

    /* Decide SPI or I2C read mechanism */
    uint8_t dummy_byte;

    /* Bus read function pointer */
    bma400_read_fptr_t read;

    /* Bus write function pointer */
    bma400_write_fptr_t write;

    /* delay(in us) function pointer */
    bma400_delay_us_fptr_t delay_us;

    /* Resolution for FOC */
    uint8_t resolution;

    /* User set read/write length */
    uint16_t read_write_len;

    /*! To store interface pointer error */
    BMA400_INTF_RET_TYPE intf_rslt;
};

#endif /* BMA400_DEFS_H_ */
/*! @endcond */
