# Waveshare RP2040 PiZero board

Board Support Package for the **RP2040 PiZero** board.

## Board

| Parameter      | Value                      |
| -------------- | -------------------------- |
| Board          | `RP2040 PiZero`            |
| MCU            | `RP2040`                   |
| LCD            | `GC9107-based LCD via SPI` |
| LCD Resolution | `128 x 115`                |


This BSP defines GPIO mapping and peripheral configuration for:

* GC9107 LCD
* MS5837 pressure sensor
* I2C bus

### LCD GPIO Mapping

| Signal      |   GPIO | Notes                               |
| ----------- | -----: | ----------------------------------- |
| LCD SCK     | GPIO18 | SPI clock                           |
| LCD MOSI    | GPIO19 | SPI data                            |
| LCD CS      | GPIO17 | Chip select                         |
| LCD DC      | GPIO20 | LCD Data / Command                  |
| LCD RST     | GPIO21 | LCD Reset                           |
| LCD BLK     | GPIO13 | Backlight control, HIGH = ON        |
| LCD TE      | GPIO15 | TE input, tearing effect            |
| FAKE3V3_LCD |  GPIO2 | Temporary LCD VDD, low-current only |

### LCD SPI Configuration

| Parameter | Value    |
| --------- | -------- |
| SPI port  | `spi0`   |
| SPI speed | `10 MHz` |

The SPI clock is currently set to **10 MHz** as a safe starting point. It can be increased later after display stability and signal integrity are verified.

## Pressure Sensor

The board is intended to use an **MS5837 pressure sensor**.

| Signal       |  GPIO | Notes                                  |
| ------------ | ----: | -------------------------------------- |
| SENSOR_POWER | GPIO3 | Temporary MS5837 VDD, low-current only |

The pressure sensor power rail can be controlled by firmware using `SENSOR_POWER`.

## I2C

I2C pins are inherited from the default Pico SDK board configuration.

| Signal | Value                      |
| ------ | -------------------------- |
| SDA    | `PICO_DEFAULT_I2C_SDA_PIN` |
| SCL    | `PICO_DEFAULT_I2C_SCL_PIN` |

## Optional TE Synchronization

TE synchronization is available on `GPIO15`, but disabled by default.

```cpp
// #define USE_TE_SYNC 1
```

## BSP API

The BSP exposes the following initialization functions:

```cpp
void bsp_i2c_init();
void bsp_ps_init();
void bsp_lcd_init();
```

### `bsp_i2c_init()`

Initializes the board I2C bus using the default RP2040 PiZero I2C pinout.

### `bsp_ps_init()`

Initializes the pressure sensor power and related GPIO configuration.

### `bsp_lcd_init()`

Initializes the LCD GPIO, SPI interface, and display control pins.

## Pin Summary

| Function        | Signal        |                       GPIO |
| --------------- | ------------- | -------------------------: |
| LCD             | SCK           |                     GPIO18 |
| LCD             | MOSI          |                     GPIO19 |
| LCD             | CS            |                     GPIO17 |
| LCD             | DC            |                     GPIO20 |
| LCD             | RST           |                     GPIO21 |
| LCD             | BLK           |                     GPIO13 |
| LCD             | TE            |                     GPIO15 |
| LCD             | Temporary VDD |                      GPIO2 |
| Pressure Sensor | Temporary VDD |                      GPIO3 |
| I2C             | SDA           | `PICO_DEFAULT_I2C_SDA_PIN` |
| I2C             | SCL           | `PICO_DEFAULT_I2C_SCL_PIN` |

## Notes

* LCD backlight is controlled by `GPIO13`.
* LCD temporary VDD is controlled by `GPIO2`.
* MS5837 pressure sensor temporary VDD is controlled by `GPIO3`.
* `PIN_FAKE3V3_LCD` and `SENSOR_POWER` are marked as **low-current only** temporary power outputs.
* LCD TE sync is routed to `GPIO15`, but disabled by default.
* I2C pins are taken from Pico SDK default board definitions.
