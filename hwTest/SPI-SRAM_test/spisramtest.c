#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* #define USE_SPRINTF */

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#define USART_BAUDRATE 19200
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* SRAM opcodes */
#define RDSR        5
#define WRSR        1
#define READ        3
#define WRITE       2

/* ---- 23LC1024 SPI wiring -----
    Arduino -- 23LC1024
    D13 <------> SCK
    D12 <------> MISO
    D11 <------> MOSI
    D10 <------> CS
    5V  <------> VCC
    5V  <------> HOLD
    5V  <-10K-> CS       Resistor
    GND <------> Vss */

/* Initialize SPI Master Device */
void spi_init_master (void)
{
    DDRB |= (1 << PB3) | (1 << PB5) | (1 << PB2);       /* Set MOSI, SCK and SS as output */
    PORTB |= (1 << PB2);                                /* Set SS to high */
    /* SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  Enable master SPI at clock rate Fck/16 */
    SPCR = (1 << SPE) | (1 << MSTR) | (0 << SPR0);      /* Fck/4 */
    SPSR = (1<<SPI2X);                                  /* Gain fck/2 instead of fck/4 */
}

/* Send and receive data over SPI */
uint8_t spi_tranceiver (uint8_t data)
{
    SPDR = data;                                        /* Load data into the buffer */
    while(!(SPSR & (1 << SPIF)));                       /* Wait until transmission complete */
    return(SPDR);                                       /* Return received data */
}

/* Initialize UART Device */
void uart_init(uint16_t ubrr)
{
   UBRR0H = (uint8_t)(ubrr >> 8);                       /* Set baud rate */
   UBRR0L = (uint8_t)ubrr;
   UCSR0B = (1 << RXEN0) | (1 << TXEN0);                /* Enable receiver and transmitter */
   UCSR0C = (1 << USBS0) | (3 << UCSZ00);               /* Set frame format: 8 bits data, 2 stop bits */
}

/* Send byte over UART */
void uart_send_byte(uint8_t data)
{
    while(!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

/* Receive byte over UART */
uint8_t uart_receive_byte(void)
{
   return UDR0;
}

/* Send string over UART */
void uart_send_string(char *string_ptr)
{
    while(*string_ptr != 0x00)
    {
        uart_send_byte(*string_ptr);
        string_ptr++;
    }
}

/* Read byte from 23LC1024 SRAM */
uint8_t spi_23LC1024_read_byte(uint32_t address)
{
    uint8_t  data;

    PORTB &= ~(1 << PB2);                           /* Set SPI_SS low */
    spi_tranceiver(READ);
    spi_tranceiver((uint8_t)((address >> 16) & 0xff));
    spi_tranceiver((uint8_t)((address >> 8) & 0xff));
    spi_tranceiver((uint8_t)address & 0xff);
    data = spi_tranceiver(0x00);
    PORTB |= (1<<PB2);                              /* Set SS to high */
    return data;
}

/* Write byte to 23LC1024 SRAM */
void spi_23LC1024_write_byte(uint32_t address, uint8_t data)
{
    PORTB &= ~(1 << PB2);
    spi_tranceiver(WRITE);
    spi_tranceiver((uint8_t)(address >> 16) & 0xff);
    spi_tranceiver((uint8_t)(address >> 8) & 0xff);
    spi_tranceiver((uint8_t)address);
    spi_tranceiver(data);
    PORTB |= (1 << PB2);
}

/* Read array from 23LC1024 SRAM */
void spi_23LC1024_read_array(uint32_t address, char *buffer, uint16_t length)
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

/* Write array to 23LC1024 SRAM */
void spi_23LC1024_write_array(uint32_t address, char *buffer, uint16_t length)
{
    uint16_t i;

    PORTB &= ~(1 << PORTB2);
    spi_tranceiver(WRITE);
    spi_tranceiver((uint8_t)(address >> 16) & 0xff);
    spi_tranceiver((uint8_t)(address >> 8) & 0xff);
    spi_tranceiver((uint8_t)address);
    for (i = 0; i < length; i++)
        spi_tranceiver(buffer[i]);

    PORTB |= (1 << PORTB2);
}


/*************************************************
 * Main function
 *************************************************/
int main(void)
{
    uint32_t i;
    uint32_t maxram_size = 131072;
    uint8_t r_value, w_value;
#ifdef USE_SPRINTF
    char str_buf[48];
#else
    char str_buf[8];
#endif
    uint8_t ram_ok = 1;

    spi_init_master();
    uart_init(BAUD_PRESCALE);
    DDRD |= 0x01;                                       /* PD0 as Output */
    _delay_ms(200);

    uart_send_string("\n\n23LC1024 RAM test started\n");

#if 1
    for (i = 0; i < maxram_size; i++)
    {
        w_value = 0;
        spi_23LC1024_write_byte(i, w_value);
        r_value = spi_23LC1024_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0x55;
        spi_23LC1024_write_byte(i, w_value);
        r_value = spi_23LC1024_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xAA;
        spi_23LC1024_write_byte(i, w_value);
        r_value = spi_23LC1024_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xFF;
        spi_23LC1024_write_byte(i, w_value);
        r_value = spi_23LC1024_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        if((i % 4096) == 0)
        {
#ifdef USE_SPRINTF
            sprintf(str_buf, "Passed bytes: %lu\n", i);
            uart_send_string(str_buf);
#else
            uart_send_string("\nPassed bytes: ");
            ltoa(i, str_buf, 10);
            uart_send_string(str_buf);
#endif
        }
    }

    if(ram_ok == 0)
    {
#ifdef USE_SPRINTF
        sprintf(str_buf, "RAM error at %lu: %d != %d\n", i, w_value, r_value);
        uart_send_string(str_buf);
#else
        uart_send_string("\nRAM error at: ");
        ltoa(i, str_buf, 10);
        uart_send_string(" ");
        itoa(w_value, str_buf, 10);
        uart_send_string(" != ");
        itoa(r_value, str_buf, 10);
#endif
    }
    else
    {
        uart_send_string("\nRAM test passed\n");
    }
#endif

   return 0;
}
s