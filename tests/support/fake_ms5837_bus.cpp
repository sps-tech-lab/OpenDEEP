//
// Created by SPS on 19/07/2026.
//
// Fake I2C bus implementation for MS5837 unit-tests
//
#include "fake_ms5837_bus.hpp"
#include "bsp.hpp"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

namespace {

// MS5837 command bytes (kept private to the fake; mirror the datasheet).
constexpr uint8_t kReset = 0x1E;
constexpr uint8_t kAdcRead = 0x00;
constexpr uint8_t kReadProm = 0xA0;
constexpr uint8_t kD1ConvBase = 0x40; // pressure conversion commands 0x40..0x4A
constexpr uint8_t kD2ConvBase = 0x50; // temperature conversion commands 0x50..0x5A

enum class PendingRead { None, Prom, Adc };
enum class LastConversion { None, Pressure, Temperature };

struct FakeBusState {
    bool attached = false;
    FakeMS5837 device{};

    PendingRead pending = PendingRead::None;
    uint8_t prom_index = 0;
    LastConversion conversion = LastConversion::None;

    int transfer_count = 0;
    int fail_at = -1;
};

FakeBusState g_state;

// Returns true if this transfer should be reported as a failure
bool should_fail() {
    const int index = g_state.transfer_count;
    ++g_state.transfer_count;
    return (g_state.fail_at >= 0) && (index == g_state.fail_at);
}

} // namespace

void fake_bus_reset() {
    g_state = FakeBusState{};
}

void fake_bus_attach(const FakeMS5837& device) {
    fake_bus_reset();
    g_state.attached = true;
    g_state.device = device;
}

void fake_bus_fail_on_transfer(int transfer_index) {
    g_state.fail_at = transfer_index;
}

int fake_bus_transfer_count() {
    return g_state.transfer_count;
}

// Independent transcription of the MS5837 CRC-4 (datasheet REV A8)
static uint8_t reference_crc4(const uint16_t prom[8]) {
    uint16_t work[8];
    for( int i = 0; i < 8; i++ ) {
        work[i] = prom[i];
    }
    work[0] = work[0] & 0x0FFF; // stored part of CRC
    work[7] = 0;

    uint16_t remainder = 0;
    for( int byte = 0; byte < 16; byte++ ) {
        const uint16_t word = work[byte / 2];
        const uint8_t data = (byte & 1) ? static_cast<uint8_t>(word & 0x00FF) : static_cast<uint8_t>(word >> 8);
        remainder ^= data;
        for( int bit = 0; bit < 8; bit++ ) {
            if( remainder & 0x8000 ) {
                remainder = static_cast<uint16_t>((remainder << 1) ^ 0x3000);
            } else {
                remainder = static_cast<uint16_t>(remainder << 1);
            }
        }
    }
    return static_cast<uint8_t>((remainder >> 12) & 0x000F);
}

void fake_bus_make_valid_prom(FakeMS5837& device, uint16_t c1, uint16_t c2, uint16_t c3, uint16_t c4, uint16_t c5,
                              uint16_t c6) {
    device.prom[0] = 0; // low 12 bits (factory data) unused by the driver
    device.prom[1] = c1;
    device.prom[2] = c2;
    device.prom[3] = c3;
    device.prom[4] = c4;
    device.prom[5] = c5;
    device.prom[6] = c6;
    device.prom[7] = 0;
    const uint8_t crc = reference_crc4(device.prom);
    device.prom[0] = static_cast<uint16_t>(crc << 12);
}

// ---- Pico SDK host stubs -------------------------------------------------

void sleep_ms(uint32_t /*ms*/) {
    // No delay on the host
}

void bsp_ps_init() {
    // Nothing to power up on the host
}

// ---- Blocking I2C implementation ----------------------------------------

int i2c_write_blocking(i2c_inst_t* /*i2c*/, uint8_t /*addr*/, const uint8_t* src, size_t len, bool /*nostop*/) {
    if( should_fail() ) {
        return PICO_ERROR_GENERIC;
    }
    if( !g_state.attached || src == nullptr || len == 0 ) {
        return PICO_ERROR_GENERIC;
    }

    const uint8_t cmd = src[0];

    if( cmd == kReset ) {
        g_state.pending = PendingRead::None;
    } else if( cmd >= kReadProm && cmd <= kReadProm + 2 * 7 ) {
        g_state.pending = PendingRead::Prom;
        g_state.prom_index = static_cast<uint8_t>((cmd - kReadProm) / 2);
    } else if( cmd >= kD1ConvBase && cmd <= kD1ConvBase + 0x0A ) {
        g_state.conversion = LastConversion::Pressure;
    } else if( cmd >= kD2ConvBase && cmd <= kD2ConvBase + 0x0A ) {
        g_state.conversion = LastConversion::Temperature;
    } else if( cmd == kAdcRead ) {
        g_state.pending = PendingRead::Adc;
    }

    return static_cast<int>(len);
}

int i2c_read_blocking(i2c_inst_t* /*i2c*/, uint8_t /*addr*/, uint8_t* dst, size_t len, bool /*nostop*/) {
    if( should_fail() ) {
        return PICO_ERROR_GENERIC;
    }
    if( !g_state.attached || dst == nullptr ) {
        return PICO_ERROR_GENERIC;
    }

    if( g_state.pending == PendingRead::Prom && len >= 2 ) {
        const uint16_t word = g_state.device.prom[g_state.prom_index & 0x07];
        dst[0] = static_cast<uint8_t>(word >> 8);
        dst[1] = static_cast<uint8_t>(word & 0xFF);
        return static_cast<int>(len);
    }

    if( g_state.pending == PendingRead::Adc && len >= 3 ) {
        const uint32_t value = (g_state.conversion == LastConversion::Pressure) ? g_state.device.d1_pressure
                                                                                : g_state.device.d2_temperature;
        dst[0] = static_cast<uint8_t>((value >> 16) & 0xFF);
        dst[1] = static_cast<uint8_t>((value >> 8) & 0xFF);
        dst[2] = static_cast<uint8_t>(value & 0xFF);
        return static_cast<int>(len);
    }

    return PICO_ERROR_GENERIC;
}
