//
// Created by :: SPS :: on 03.02.2026.
//

#ifndef OPENDEEP_MS5837_HPP
#define OPENDEEP_MS5837_HPP

#include <cstdint>

/**
 * @brief OverSampling Ratio of ADC conversion
 */
enum class MS5837osr : uint16_t {
    osr256 = 256,
    osr512 = 512,
    osr1024 = 1024,
    osr2048 = 2048,
    osr4096 = 4096,
    osr8192 = 8192
};

/**
 * @brief   Pressure/Temperature Sensor class
 * @details Basic Pressure & Temperature Sensor methods
 */
class PresTempSensor {
public:
    PresTempSensor();
    ~PresTempSensor();

    /**
     * @brief Sensor initialization
     * @return Success of initialization
     */
    bool init();

    /**
     * @brief Perform temperature/pressure measurements and calculations
     * @return Success of measurements and calculations
     */
    bool measure();

    /**
     * @brief Get calculated temperature
     * @details Could be 2nd order compensated in some cases
     * @return temperature in ['C]
     */
    float temperature() const;

    /**
     * @brief Get calculated temperature compensated pressure
     * @return pressure in [mbar]
     */
    float pressure() const;

    /**
     * @brief Get calculated depth
     * @return depth
     */
    float depth() const;

    /**
     * @brief    Set water density in kg/m^3
     * @details  fresh water = 997 / seawater = 1029
     */
    void set_water_density(uint16_t density);

private:
    static uint8_t crc4(uint16_t n_prom[]);
    uint16_t* get_calibration_data();
    bool read_pressure(MS5837osr osr);
    bool read_temperature(MS5837osr osr);
    void calculate();

    uint8_t readbuf[4] = {0};
    uint16_t calibration[8] = {0};
    uint16_t water_density{997};
    uint32_t raw_pressure{0};
    uint32_t raw_temperature{0};
    int32_t compensated_pressure{0};
    int32_t compensated_temperature{0};
    int32_t atmospheric_pressure{0};
    // //Ref.: MS5837-02BA Datasheet REV A8 12/2019
    // int32_t         diff_temperature{0};            // dT
    // int64_t         off_temperature{0};             // OFF
    // int64_t         sens_on_temperature{0};         // SENS
    // int32_t         OFFi = 0;
    // int32_t         SENSi = 0;
    //
    // int32_t Ti = 0;
    // int64_t OFF2 = 0;
    // int64_t SENS2 = 0;
};

#endif // OPENDEEP_MS5837_HPP
