//
// Created by SPS on 19/07/2026.
//
// MS5837 pressure/temperature sensor driver unit-tests
//

// Includes
#include "test_ms5837.hpp"
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include "MS5837.hpp"
#include "fake_ms5837_bus.hpp"

using Catch::Approx;

namespace {

// Factory calibration coefficients (emulate MS5837-02BA PROM)
constexpr uint16_t kC1 = 46372;
constexpr uint16_t kC2 = 43981;
constexpr uint16_t kC3 = 29059;
constexpr uint16_t kC4 = 27842;
constexpr uint16_t kC5 = 31553;
constexpr uint16_t kC6 = 28165;

// Raw ADC results giving ~20C/~1100mbar
constexpr uint32_t kD1Warm = 6465444;
constexpr uint32_t kD2Warm = 8077636;

FakeMS5837 make_warm_device() {
    FakeMS5837 dev;
    fake_bus_make_valid_prom(dev, kC1, kC2, kC3, kC4, kC5, kC6);
    dev.d1_pressure = kD1Warm;
    dev.d2_temperature = kD2Warm;
    return dev;
}

// Compensation, in double precision, including the <20C 2nd order flow
void reference_temp_pressure(uint32_t d1, uint32_t d2, double& temperature_c, double& pressure_mbar) {
    const double dT = static_cast<double>(d2) - static_cast<double>(kC5) * 256.0;
    const double temp = 2000.0 + dT * static_cast<double>(kC6) / 8388608.0;    // /2^23
    const double off = static_cast<double>(kC2) * 131072.0 + kC4 * dT / 64.0;  // 2^17, /2^6
    const double sens = static_cast<double>(kC1) * 65536.0 + kC3 * dT / 128.0; // 2^16, /2^7

    double ti = 0.0;
    double offi = 0.0;
    double sensi = 0.0;
    if( temp < 2000.0 ) {
        ti = 11.0 * dT * dT / 34359738368.0; // /2^35
        const double t = temp - 2000.0;
        offi = 31.0 * t * t / 8.0;
        sensi = 63.0 * t * t / 32.0;
    }

    const double off2 = off - offi;
    const double sens2 = sens - sensi;
    const double pressure = (static_cast<double>(d1) * sens2 / 2097152.0 - off2) / 32768.0; // /2^21, /2^15

    temperature_c = (temp - ti) / 100.0;
    pressure_mbar = pressure / 100.0;
}

} // namespace

TEST_CASE("MS5837 init succeeds with a CRC-valid PROM") {
    const FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());
}

TEST_CASE("MS5837 init rejects a PROM whose CRC does not match") {
    FakeMS5837 dev = make_warm_device();
    dev.prom[3] ^= 0x0001; // corrupt a coefficient without fixing stored CRC
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE_FALSE(sensor.init());
}

TEST_CASE("MS5837 init fails when the bus reports an I2C error") {
    const FakeMS5837 dev = make_warm_device();

    SECTION("failure while requesting a calibration word") {
        fake_bus_attach(dev);
        fake_bus_fail_on_transfer(1); // write first READ_PROM command
        PresTempSensor sensor;
        REQUIRE_FALSE(sensor.init());
    }

    SECTION("failure while reading a calibration word") {
        fake_bus_attach(dev);
        fake_bus_fail_on_transfer(2); // read first PROM word
        PresTempSensor sensor;
        REQUIRE_FALSE(sensor.init());
    }
}

TEST_CASE("MS5837 reports temperature and pressure matching the reference (>= 20 C)") {
    const FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());

    double ref_temp = 0.0;
    double ref_pressure = 0.0;
    reference_temp_pressure(kD1Warm, kD2Warm, ref_temp, ref_pressure);

    CHECK(sensor.temperature() == Approx(ref_temp).margin(0.05));
    CHECK(sensor.pressure() == Approx(ref_pressure).margin(0.05));

    // near room temperature and roughly atmospheric
    CHECK(sensor.temperature() == Approx(20.0).margin(0.1));
    CHECK(sensor.pressure() > 900.0);
    CHECK(sensor.pressure() < 1300.0);
}

TEST_CASE("MS5837 applies second-order compensation below 20 C") {
    FakeMS5837 dev = make_warm_device();
    dev.d2_temperature = 7000000; // strongly negative dT -> temperature below 20C
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());

    double ref_temp = 0.0;
    double ref_pressure = 0.0;
    reference_temp_pressure(kD1Warm, 7000000, ref_temp, ref_pressure);

    REQUIRE(ref_temp < 0.0); // guard: this vector exercises cold flow
    CHECK(sensor.temperature() == Approx(ref_temp).margin(0.05));
    CHECK(sensor.pressure() == Approx(ref_pressure).margin(0.05));
}

TEST_CASE("MS5837 depth is zero at the surface baseline") {
    const FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init()); // init captures atmospheric baseline

    // no pressure change since init -> depth must be exactly zero!
    CHECK(sensor.depth() == Approx(0.0).margin(1e-9));
}

TEST_CASE("MS5837 depth grows with pressure above the baseline") {
    FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());
    const double surface_pressure = sensor.pressure();

    // submerge -> raise raw pressure and measure
    dev.d1_pressure = 7200000;
    fake_bus_attach(dev);
    REQUIRE(sensor.measure());

    const double submerged_pressure = sensor.pressure();
    REQUIRE(submerged_pressure > surface_pressure);

    const double depth = sensor.depth();
    CHECK(depth > 0.0);

    // independent depth check for fresh water (default density 997 kg/m^3)
    // depth = (P - P_atm) [Pa] / (rho * g)
    // 1mbar == 100Pa
    const double expected = (submerged_pressure - surface_pressure) * 100.0 / (997.0 * 9.80665);
    CHECK(depth == Approx(expected).margin(0.01));
}

TEST_CASE("MS5837 depth clamps to zero when pressure drops below the baseline") {
    FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());

    dev.d1_pressure = 5000000; // lower than baseline pressure
    fake_bus_attach(dev);
    REQUIRE(sensor.measure());

    REQUIRE(sensor.pressure() < 1100.0);
    CHECK(sensor.depth() == Approx(0.0).margin(1e-9));
}

TEST_CASE("MS5837 water density scales depth inversely") {
    FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());

    dev.d1_pressure = 7200000;
    fake_bus_attach(dev);
    REQUIRE(sensor.measure());

    sensor.set_water_density(997); // fresh water
    const double depth_fresh = sensor.depth();

    sensor.set_water_density(1029); // seawater
    const double depth_sea = sensor.depth();

    REQUIRE(depth_fresh > 0.0);
    REQUIRE(depth_sea > 0.0);
    // denser water -> shallower depth for same pressure
    CHECK(depth_sea < depth_fresh);
    CHECK(depth_fresh / depth_sea == Approx(1029.0 / 997.0).margin(1e-4));
}

TEST_CASE("MS5837 measure fails when a conversion read errors") {
    const FakeMS5837 dev = make_warm_device();
    fake_bus_attach(dev);

    PresTempSensor sensor;
    REQUIRE(sensor.init());

    // fail the very next transfer, first write of measure()
    fake_bus_fail_on_transfer(fake_bus_transfer_count());
    CHECK_FALSE(sensor.measure());
}
