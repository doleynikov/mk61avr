#include "spi.h"

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
