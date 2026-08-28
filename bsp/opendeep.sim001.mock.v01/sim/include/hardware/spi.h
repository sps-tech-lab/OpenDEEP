//
// Created by :: SPS :: on 27/08/2026.
//
// @Board:   Virtual (host simulator)
// @Details: <hardware/spi.h>  for host simulator
//           decodes byte stream as the real controller
//
#ifndef OPENDEEP_SIM_HARDWARE_SPI_H
#define OPENDEEP_SIM_HARDWARE_SPI_H

#include <cstddef>
#include <cstdint>

typedef struct spi_inst spi_inst_t;

extern spi_inst_t* const spi0;
extern spi_inst_t* const spi1;

typedef enum { SPI_CPHA_0 = 0, SPI_CPHA_1 = 1 } spi_cpha_t;
typedef enum { SPI_CPOL_0 = 0, SPI_CPOL_1 = 1 } spi_cpol_t;
typedef enum { SPI_LSB_FIRST = 0, SPI_MSB_FIRST = 1 } spi_order_t;

uint32_t spi_init(spi_inst_t* spi, uint32_t baudrate);
void spi_set_format(spi_inst_t* spi, uint32_t data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order);
int spi_write_blocking(spi_inst_t* spi, const uint8_t* src, size_t len);
int spi_write16_blocking(spi_inst_t* spi, const uint16_t* src, size_t len);

#endif // OPENDEEP_SIM_HARDWARE_SPI_H
