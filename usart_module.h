// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: uart_module.h
//
// Module description:
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
// MA  02110-1301, USA.
//
// ***********************************************************
//
#ifndef _USART_MODULE_H_
#define _USART_MODULE_H_

#include <avr/pgmspace.h>
#include <avr/interrupt.h>

// прерывания обслуживаемые в модуле
SIGNAL (SIG_UART_DATA);
SIGNAL (SIG_UART_RECV);

// процедуры модуля
extern void putsrom_UART(char* PROGMEM pPTR);
extern void putsram_UART(char* pPTR);
extern unsigned char *GetLine_UART(void);
extern unsigned char RxExtractLine(unsigned char *buff);

extern char *pBuff_UART;

#define UDRIE_on()                        \
__asm__ __volatile__(                     \
  "sbi %[ucsrb], %[udrie]" "\n\t"         \
  :                                       \
  : [ucsrb] "I" (_SFR_IO_ADDR(UCSRB)),    \
    [udrie] "I" (UDRIE)                   \
)

#define UDRIE_off()                       \
__asm__ __volatile__(                     \
  "cbi %[ucsrb], %[udrie]" "\n\t"         \
  :                                       \
  : [ucsrb] "I" (_SFR_IO_ADDR(UCSRB)),    \
    [udrie] "I" (UDRIE)                   \
)

#define isUDRE(value)                     \
__asm__ __volatile__(                     \
  "clr %[rval]"             "\n\t"        \
  "sbic %[ucsra], %[udre]"  "\n\t"        \
  "ser %[rval]"             "\n\t"        \
  : [rval]  "=r" (value)                  \
  : [ucsra] "I" (_SFR_IO_ADDR(UCSRA)),    \
    [udre]  "I" (UDRE)                    \
)

#define WaitUDRE(value)                   \
__asm__ __volatile__(                     \
  "wait_udre%=:"           "\n\t"        \
  "sbis %[ucsra], %[udre]"  "\n\t"        \
  "rjmp wait_udre%="       "\n\t"        \
  :                                       \
  : [ucsra] "I" (_SFR_IO_ADDR(UCSRA)),    \
    [udre]  "I" (UDRE)                    \
)

#define RxUART_off()                      \
__asm__ __volatile__(                     \
  "cbi %[ucsrb], %[rxen]"     "\n\t"      \
  :                                       \
  : [ucsrb] "I" (_SFR_IO_ADDR(UCSRB)),    \
    [rxen] "I" (RXEN)                     \
)

#define RxUART_on()                       \
__asm__ __volatile__(                     \
  "sbi %[ucsrb], %[rxen]"     "\n\t"      \
  :                                       \
  : [ucsrb] "I" (_SFR_IO_ADDR(UCSRB)),    \
    [rxen] "I" (RXEN)                     \
)

#endif // _USART_MODULE_H_
