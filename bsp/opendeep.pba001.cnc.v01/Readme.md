# OpenDEEP RP2040/RP2350 Module CNC Board

Board Support Package for the **opendeep.pba001.cnc.v01** board based on RP2040/RP2350 
[Spotpear](https://spotpear.com/) Core modules.

| Parameter | Value |
|---|---|
| Board ID | `opendeep.pba001.cnc.v01` |
| MCU | `RP2040/RP2350 Core module` |
| LCD | `GC9107-based LCD via SPI` |
| LCD Resolution | `128 x 115` |

This BSP defines GPIO mapping and peripheral configuration for:

* GC9107 LCD
* MS5837 pressure sensor
* I2C bus
* UART debug interface

### LCD GPIO Mapping

| Signal   |   GPIO | Notes                         |
| -------- | -----: | ----------------------------- |
| LCD SCK  | GPIO10 | SPI clock                     |
| LCD MOSI | GPIO11 | SPI data                      |
| LCD CS   |  GPIO9 | Chip select                   |
| LCD DC   | GPIO14 | Data / Command                |
| LCD RST  | GPIO12 | Display reset                 |
| LCD BLK  |  GPIO2 | Backlight control, HIGH = ON  |
| LCD TE   | GPIO15 | Optional tearing-effect input |

### LCD SPI Configuration

| Parameter | Value    |
| --------- | -------- |
| SPI port  | `spi1`   |
| SPI speed | `10 MHz` |

The SPI clock is currently set to **10 MHz** as a safe starting point. It can be increased later after display stability and signal integrity are verified.

TE synchronization is available on `GPIO15`, but disabled by default.

```cpp
// #define USE_TE_SYNC 1
```

## Pressure Sensor

The board is intended to use an **MS5837 pressure sensor**.

| Signal       |  GPIO | Notes                        |
| ------------ | ----: | ---------------------------- |
| SENSOR_POWER | GPIO3 | Controls pressure sensor VDD |

The pressure sensor power rail can be controlled by firmware using `SENSOR_POWER`.

## I2C

The pressure sensor is connected through I2C.

### I2C GPIO Mapping

| Signal |  GPIO |
| ------ | ----: |
| SDA    | GPIO4 |
| SCL    | GPIO5 |

### I2C Configuration

| Parameter | Value     |
| --------- | --------- |
| I2C port  | `i2c0`    |
| I2C speed | `400 kHz` |

## UART

UART configuration is inherited from the board CMake configuration.

The BSP selects the UART peripheral based on `PICO_DEFAULT_UART`:

```cpp
#if PICO_DEFAULT_UART == 0
#define BSP_UART_PORT uart0
#elif PICO_DEFAULT_UART == 1
#define BSP_UART_PORT uart1
#else
#error "Unsupported PICO_DEFAULT_UART value <Expected 0 or 1>"
#endif
```

UART TX pin, RX pin, and baudrate are taken from Pico SDK defaults:

```cpp
#define BSP_UART_TX_PIN   PICO_DEFAULT_UART_TX_PIN
#define BSP_UART_RX_PIN   PICO_DEFAULT_UART_RX_PIN
#define BSP_UART_BAUDRATE PICO_DEFAULT_UART_BAUD_RATE
```

These values should be configured in `bsp.cmake`.

## BSP API

The BSP exposes the following initialization functions:

```cpp
void bsp_i2c_init();
void bsp_ps_init();
void bsp_lcd_init();
```

### `bsp_i2c_init()`

Initializes the board I2C bus using the configured SDA, SCL, port, and speed.

### `bsp_ps_init()`

Initializes the pressure sensor power and related GPIO configuration.

### `bsp_lcd_init()`

Initializes the LCD GPIO, SPI interface, and display control pins.

## Pin Summary

| Function        | Signal       |   GPIO |
| --------------- | ------------ | -----: |
| LCD             | SCK          | GPIO10 |
| LCD             | MOSI         | GPIO11 |
| LCD             | CS           |  GPIO9 |
| LCD             | DC           | GPIO14 |
| LCD             | RST          | GPIO12 |
| LCD             | BLK          |  GPIO2 |
| LCD             | TE           | GPIO15 |
| Pressure Sensor | Power Enable |  GPIO3 |
| I2C             | SDA          |  GPIO4 |
| I2C             | SCL          |  GPIO5 |

## Notes

* LCD backlight is controlled by `GPIO2`.
* MS5837 pressure sensor power is controlled by `GPIO3`.
* LCD TE sync is routed to `GPIO15`, but disabled by default.
* UART pins are not hardcoded in the BSP file.
* UART configuration should be provided by `bsp.cmake`.
