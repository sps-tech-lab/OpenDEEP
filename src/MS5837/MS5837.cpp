//
// Created by :: SPS :: on 03.02.2026.
//
#include "MS5837.hpp"
#include <cstdio>
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

// Port config TODO: move to bsp
#define I2C_PORT i2c1

// MS5837 basic commands
// clang-format off
constexpr uint8_t MS5837_ADDRESS      = 0x76;
constexpr uint8_t MS5837_RESET        = 0x1E;
constexpr uint8_t MS5837_DP_CONV_256  = 0x40;
constexpr uint8_t MS5837_DP_CONV_512  = 0x42;
constexpr uint8_t MS5837_DP_CONV_1024 = 0x44;
constexpr uint8_t MS5837_DP_CONV_2048 = 0x46;
constexpr uint8_t MS5837_DP_CONV_4096 = 0x48;
constexpr uint8_t MS5837_DP_CONV_8192 = 0x4A;
constexpr uint8_t MS5837_DT_CONV_256  = 0x50;
constexpr uint8_t MS5837_DT_CONV_512  = 0x52;
constexpr uint8_t MS5837_DT_CONV_1024 = 0x54;
constexpr uint8_t MS5837_DT_CONV_2048 = 0x56;
constexpr uint8_t MS5837_DT_CONV_4096 = 0x58;
constexpr uint8_t MS5837_DT_CONV_8192 = 0x5A;
constexpr uint8_t MS5837_ADC_READ     = 0x00;
constexpr uint8_t MS5837_READ_PROM    = 0xA0;
// clang-format on

// MS5837 calibration data
#define MS5837_CALIB_DATA_LEN (7)

PresTempSensor::PresTempSensor() = default;

bool PresTempSensor::init() {
    gpio_init(PIN_FAKE3V3_MS5);
    gpio_set_dir(PIN_FAKE3V3_MS5, GPIO_OUT);
    gpio_put(PIN_FAKE3V3_MS5, true);

    sleep_ms(10);

    uint8_t cmd = MS5837_RESET;
    auto ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, false));
    printf("PresTempSensor reset %s\n", (ret >= PICO_OK) ? ("OK") : ("ERROR"));

    sleep_ms(10);

    // Read calibration and crc
    for( uint8_t i = 0; i < MS5837_CALIB_DATA_LEN; i++ ) {
        cmd = (MS5837_READ_PROM + i * 2);
        ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, true));
        if( ret < PICO_OK ) {
            printf("Calibration C%u requested ERROR[%d]\n", i, ret);
            return false;
        }
        ret = static_cast<int32_t>(i2c_read_blocking(I2C_PORT, MS5837_ADDRESS, this->readbuf, 2, false));
        this->calibration[i] = static_cast<uint16_t>(this->readbuf[0] << 8 | this->readbuf[1]);
        if( ret < PICO_OK ) {
            printf("Calibration C%u read ERROR[%d]\n", i, ret);
            return false;
        }
        printf("Calibration C%u value:  %u\n", i, this->calibration[i]);
    }

    const uint8_t read_crc = this->calibration[0] >> 12;
    const uint8_t calc_crc = crc4(this->calibration);

    if( read_crc != calc_crc ) {
        printf("Calibration CRC Fail!\n");
        return false;
    }
    printf("Calibration CRC OK\n");

    if( measure() == false ) {
        printf("PresTempSensor measure ERROR\n");
    }

    // Remember first measured pressure like atmospheric_pressure for depth compensation
    this->atmospheric_pressure = this->compensated_pressure;

    return true;
}

bool PresTempSensor::measure() {
    if( read_pressure(MS5837osr::osr8192) == false ) {
        printf("PresTempSensor read pressure ERROR\n");
        return false;
    }
    if( read_temperature(MS5837osr::osr8192) == false ) {
        printf("PresTempSensor read temperature ERROR\n");
        return false;
    }
    calculate();

    return true;
}

void PresTempSensor::calculate() {
    // Ref.: MS5837-02BA Datasheet REV A8 12/2019
    int32_t diff_temp{0};     // dT
    int32_t actual_temp{0};   // TEMP
    int64_t off_temp{0};      // OFF
    int64_t sens_temp{0};     // SENS
    int64_t temp_low{0};      // Ti
    int64_t off_temp_low{0};  // OFFi
    int64_t sens_temp_low{0}; // SENSi
    int64_t off_temp_2nd{0};  // OFF2
    int64_t sens_temp_2nd{0}; // SENS2

    // Calculation and compensation
    diff_temp = static_cast<int32_t>(this->raw_temperature - (static_cast<uint32_t>(this->calibration[5]) * 256l));

    actual_temp =
        2000l + static_cast<int32_t>((static_cast<int64_t>(diff_temp) * static_cast<uint32_t>(this->calibration[6])) >>
                                     23); // 2^23

    off_temp = static_cast<int64_t>(this->calibration[2]) * 131072l +
               (static_cast<int64_t>(this->calibration[4]) * diff_temp) / 64l;

    sens_temp = static_cast<int64_t>(this->calibration[1]) * 65536l +
                (static_cast<int64_t>(this->calibration[3]) * diff_temp) / 128l;

    // Temperature lower than 20'C -> compensate it
    if( actual_temp < 2000 ) {
        const auto dt = static_cast<int64_t>(diff_temp);
        const auto t = static_cast<int64_t>(static_cast<int64_t>(actual_temp) - 2000LL);

        temp_low = (11LL * dt * dt) / 34359738368LL;
        off_temp_low = (31LL * t * t) / 8LL;
        sens_temp_low = (63LL * t * t) / 32LL;
    }

    off_temp_2nd = off_temp - off_temp_low;
    sens_temp_2nd = sens_temp - sens_temp_low;
    this->compensated_temperature = static_cast<int32_t>(actual_temp - temp_low);
    this->compensated_pressure =
        static_cast<int32_t>((this->raw_pressure * sens_temp_2nd / 2097152l - off_temp_2nd) / 32768l);
}

float PresTempSensor::pressure() const {
    return static_cast<float>(this->compensated_pressure) / 100.0f;
}

float PresTempSensor::temperature() const {
    return static_cast<float>(this->compensated_temperature) / 100.0f;
}

float PresTempSensor::depth() const {
    const float temp_depth = (static_cast<float>(this->compensated_pressure - this->atmospheric_pressure) /
                              (static_cast<float>(this->water_density) * 9.80665f));

    return (temp_depth > 0.0f) ? temp_depth : 0.0f;
}

void PresTempSensor::set_water_density(const uint16_t density) {
    this->water_density = density;
}

bool PresTempSensor::read_pressure(MS5837osr osr) {
    uint8_t cmd = 0;
    switch( osr ) {
    case MS5837osr::osr256:
        cmd = MS5837_DP_CONV_256;
        break;
    case MS5837osr::osr512:
        cmd = MS5837_DP_CONV_512;
        break;
    case MS5837osr::osr1024:
        cmd = MS5837_DP_CONV_1024;
        break;
    case MS5837osr::osr2048:
        cmd = MS5837_DP_CONV_2048;
        break;
    case MS5837osr::osr4096:
        cmd = MS5837_DP_CONV_4096;
        break;
    case MS5837osr::osr8192:
        cmd = MS5837_DP_CONV_8192;
        break;
    default:
        return false;
    }

    auto ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, false));
    if( ret < PICO_OK ) {
        printf("PresTempSensor OSR setting ERROR[%d]\n", ret);
        return false;
    }

    sleep_ms(20);

    cmd = MS5837_ADC_READ;
    ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, true));
    if( ret < PICO_OK ) {
        printf("PresTempSensor request pressure ERROR[%d]\n", ret);
        return false;
    }
    ret = static_cast<int32_t>(i2c_read_blocking(I2C_PORT, MS5837_ADDRESS, this->readbuf, 3, false));
    if( ret < PICO_OK ) {
        printf("PresTempSensor read pressure ERROR[%d]\n", ret);
        return false;
    }

    this->raw_pressure = (static_cast<uint32_t>(this->readbuf[0]) << 16) |
                         (static_cast<uint32_t>(this->readbuf[1]) << 8) | static_cast<uint32_t>(this->readbuf[2]);

    // printf("PresTempSensor digital pressure is %u\n", this->raw_pressure);
    return true;
}

bool PresTempSensor::read_temperature(MS5837osr osr) {
    uint8_t cmd = 0;
    switch( osr ) {
    case MS5837osr::osr256:
        cmd = MS5837_DT_CONV_256;
        break;
    case MS5837osr::osr512:
        cmd = MS5837_DT_CONV_512;
        break;
    case MS5837osr::osr1024:
        cmd = MS5837_DT_CONV_1024;
        break;
    case MS5837osr::osr2048:
        cmd = MS5837_DT_CONV_2048;
        break;
    case MS5837osr::osr4096:
        cmd = MS5837_DT_CONV_4096;
        break;
    case MS5837osr::osr8192:
        cmd = MS5837_DT_CONV_8192;
        break;
    default:
        return false;
    }

    auto ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, false));
    if( ret < PICO_OK ) {
        printf("PresTempSensor OSR setting ERROR[%d]\n", ret);
        return false;
    }

    sleep_ms(20);

    cmd = MS5837_ADC_READ;
    ret = static_cast<int32_t>(i2c_write_blocking(I2C_PORT, MS5837_ADDRESS, &cmd, 1, true));
    if( ret < PICO_OK ) {
        printf("PresTempSensor request temperature ERROR[%d]\n", ret);
        return false;
    }
    ret = static_cast<int32_t>(i2c_read_blocking(I2C_PORT, MS5837_ADDRESS, this->readbuf, 3, false));
    if( ret < PICO_OK ) {
        printf("PresTempSensor read temperature ERROR[%d]\n", ret);
        return false;
    }

    this->raw_temperature = (static_cast<uint32_t>(this->readbuf[0]) << 16) |
                            (static_cast<uint32_t>(this->readbuf[1]) << 8) | static_cast<uint32_t>(this->readbuf[2]);

    printf("PresTempSensor digital temperature is %u\n", this->raw_temperature);
    return true;
}

// Ref.: MS5837-02BA Datasheet REV A8 12/2019
uint8_t PresTempSensor::crc4(uint16_t n_prom[]) {
    uint16_t n_rem = 0;

    n_prom[0] = ((n_prom[0]) & 0x0FFF);
    n_prom[7] = 0;

    for( uint8_t cnt = 0; cnt < 16; cnt++ ) {
        if( cnt % 2 == 1 ) {
            n_rem ^= static_cast<uint16_t>((n_prom[cnt >> 1]) & 0x00FF);
        } else {
            n_rem ^= static_cast<uint16_t>(n_prom[cnt >> 1] >> 8);
        }
        for( uint8_t n_bit = 8; n_bit > 0; n_bit-- ) {
            if( n_rem & 0x8000 ) {
                n_rem = (n_rem << 1) ^ 0x3000;
            } else {
                n_rem = (n_rem << 1);
            }
        }
    }

    n_rem = ((n_rem >> 12) & 0x000F);

    return n_rem ^ 0x00;
}