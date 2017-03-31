#ifndef _SPISRAM_H_
#define _SPISRAM_H_

#include <stdint.h>
#include "spi.h"

#define  MAX_SIZE_23LC1024_SRAM             131072

/* Read byte from SRAM */
uint8_t spi_sram_read_byte(uint32_t address);

/* Write byte to SRAM */
void spi_sram_write_byte(uint32_t address, uint8_t data);

/* Read array from SRAM */
void spi_sram_read_array(uint32_t address, char *buffer, uint16_t length);

/* Write array to SRAM */
void spi_sram_write_array(uint32_t address, char *buffer, uint16_t length);
#endif
