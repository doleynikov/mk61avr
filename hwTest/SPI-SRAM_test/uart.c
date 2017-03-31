#include "uart.h"

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
