#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "spi.h"
#include "uart.h"
#include "spisram.h"

/* #define USE_SPRINTF */

int main(void)
{
    uint32_t i;
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
    for (i = 0; i < MAX_SIZE_23LC1024_SRAM; i++)
    {
        w_value = 0;
        spi_sram_write_byte(i, w_value);
        r_value = spi_sram_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0x55;
        spi_sram_write_byte(i, w_value);
        r_value = spi_sram_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xAA;
        spi_sram_write_byte(i, w_value);
        r_value = spi_sram_read_byte(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xFF;
        spi_sram_write_byte(i, w_value);
        r_value = spi_sram_read_byte(i);
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

