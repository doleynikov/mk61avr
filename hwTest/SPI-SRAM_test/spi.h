#ifndef _SPI_H_
#define _SPI_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

/* SRAM opcodes */
#define RDSR        5
#define WRSR        1
#define READ        3
#define WRITE       2

/* 
---- 23LC1024 SPI wiring -----
    Arduino -- 23LC1024
    D13 <------> SCK
    D12 <------> MISO
    D11 <------> MOSI
    D10 <------> CS
    5V  <------> VCC
    5V  <------> HOLD
    5V  <-10K-> CS       Resistor
    GND <------> Vss
*/


/* Initialize SPI Master Device */
void spi_init_master (void);

/* Send and receive data over SPI */
uint8_t spi_tranceiver (uint8_t data);

#endif
