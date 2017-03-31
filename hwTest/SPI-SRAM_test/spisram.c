#include "spisram.h"

/* Read byte from SRAM */
uint8_t spi_sram_read_byte(uint32_t address)
{
    uint8_t  data;

    PORTB &= ~(1 << PB2);                               /* Set SPI_SS low */
    spi_tranceiver(READ);
    spi_tranceiver((uint8_t)((address >> 16) & 0xff));
    spi_tranceiver((uint8_t)((address >> 8) & 0xff));
    spi_tranceiver((uint8_t)address & 0xff);
    data = spi_tranceiver(0x00);
    PORTB |= (1 << PB2);                                /* Set SS to high */
    return data;
}

/* Write byte to SRAM */
void spi_sram_write_byte(uint32_t address, uint8_t data)
{
    PORTB &= ~(1 << PB2);
    spi_tranceiver(WRITE);
    spi_tranceiver((uint8_t)(address >> 16) & 0xff);
    spi_tranceiver((uint8_t)(address >> 8) & 0xff);
    spi_tranceiver((uint8_t)address);
    spi_tranceiver(data);
    PORTB |= (1 << PB2);
}

/* Read array from SRAM */
void spi_sram_read_array(uint32_t address, char *buffer, uint16_t length)
{
    uint16_t i;

    PORTB &= ~(1 << PB2);
    spi_tranceiver(READ);
    spi_tranceiver((uint8_t)(address >> 16) & 0xff);
    spi_tranceiver((uint8_t)(address >> 8) & 0xff);
    spi_tranceiver((uint8_t)address);
    for (i = 0; i < length; i++)
        buffer[i] = spi_tranceiver(0x00);

    PORTB |= (1 << PB2);
}

/* Write array to SRAM */
void spi_sram_write_array(uint32_t address, char *buffer, uint16_t length)
{
    uint16_t i;

    PORTB &= ~(1 << PB2); 
    spi_tranceiver(WRITE);
    spi_tranceiver((uint8_t)(address >> 16) & 0xff);
    spi_tranceiver((uint8_t)(address >> 8) & 0xff);
    spi_tranceiver((uint8_t)address);
    for (i = 0; i < length; i++)
        spi_tranceiver(buffer[i]);

    PORTB |= (1 << PB2);             
}

