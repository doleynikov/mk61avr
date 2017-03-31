#ifndef _UART_H_
#define _UART_H_

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define USART_BAUDRATE 19200 
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

/* Initialize UART Device */
void uart_init(uint16_t ubrr);

/* Send byte over UART */
void uart_send_byte(uint8_t data);

/* Receive byte over UART */
uint8_t uart_receive_byte(void);

/* Send string over UART */
void uart_send_string(char *string_ptr);
#endif
