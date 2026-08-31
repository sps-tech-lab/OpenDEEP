//
// Created by :: SPS :: on 30/08/2026.
//
// @Brief: Simulated MS5837 dive profile
//
#include "sim_sensor.hpp"
#include <cmath>
#include "fake_ms5837_bus.hpp"
#include "sim_window.hpp"

namespace {

// Factory calibration coefficients
constexpr uint16_t kC1 = 46372;
constexpr uint16_t kC2 = 43981;
constexpr uint16_t kC3 = 29059;
constexpr uint16_t kC4 = 27842;
constexpr uint16_t kC5 = 31553;
constexpr uint16_t kC6 = 28165;

// PresTempSensor's default
constexpr double kSurfacePressureUnits = 101325.0;
constexpr double kWaterDensity = 997.0;
constexpr double kGravity = 9.80665;

constexpr uint32_t kAdcMax = 0x00FFFFFF; // 24-bit ADC result
constexpr double kWaterTemperatureC = 21.8;

bool g_attached = false;

// Invert the MS5837 compensation (datasheet REV A8) to figure out raw ADC codes
void solve_adc(double temperature_c, double pressure_units, uint32_t& d1, uint32_t& d2) {
    const double target_temp = temperature_c * 100.0; // 0.01 'C, as in driver

    // <20'C 2nd-order term subtracts Ti from the reported temperature
    double dt = 0.0;
    double actual_temp = target_temp;
    for( int i = 0; i < 4; i++ ) {
        dt = (actual_temp - 2000.0) * 8388608.0 / static_cast<double>(kC6); // 2^23
        const double ti = (actual_temp < 2000.0) ? (11.0 * dt * dt / 34359738368.0) : 0.0;
        actual_temp = target_temp + ti;
    }

    double off = static_cast<double>(kC2) * 131072.0 + static_cast<double>(kC4) * dt / 64.0;
    double sens = static_cast<double>(kC1) * 65536.0 + static_cast<double>(kC3) * dt / 128.0;

    if( actual_temp < 2000.0 ) {
        const double t = actual_temp - 2000.0;
        off -= 31.0 * t * t / 8.0;
        sens -= 63.0 * t * t / 32.0;
    }

    const double d2_raw = dt + static_cast<double>(kC5) * 256.0;
    const double d1_raw = (pressure_units * 32768.0 + off) * 2097152.0 / sens; // 2^15, 2^21

    d2 = static_cast<uint32_t>(std::fmin(std::fmax(d2_raw, 0.0), static_cast<double>(kAdcMax)));
    d1 = static_cast<uint32_t>(std::fmin(std::fmax(d1_raw, 0.0), static_cast<double>(kAdcMax)));
}

void publish(double depth_m, double temperature_c) {
    const double pressure_units = kSurfacePressureUnits + depth_m * kWaterDensity * kGravity;

    uint32_t d1 = 0;
    uint32_t d2 = 0;
    solve_adc(temperature_c, pressure_units, d1, d2);
    fake_bus_set_adc(d1, d2);
}

} // namespace

void sim_sensor_init() {
    FakeMS5837 device;
    fake_bus_make_valid_prom(device, kC1, kC2, kC3, kC4, kC5, kC6);
    fake_bus_attach(device);
    g_attached = true;

    // Start at the surface
    publish(0.0, kWaterTemperatureC);
}

void sim_sensor_advance(uint32_t elapsed_ms) {
    if( !g_attached ) {
        return;
    }

    double depth_m = static_cast<double>(sim_input_depth_bias());
    if( depth_m < 0.0 ) {
        depth_m = 0.0;
    }

    publish(depth_m, kWaterTemperatureC);
}
