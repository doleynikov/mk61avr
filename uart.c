// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: uart.c
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
#include "usart_module.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include "config.h"
#include "micro_os.h"
#include "mk61types.h"

char   *pBuff_UART;


unsigned char   RxBuffer[16];               // Приемный буфер символов с UART
unsigned char   *pRxPut = &RxBuffer[0];     // Указатель на запись символа в буфер
unsigned char   *pRxGet = &RxBuffer[0];     // Указатель на чтение символа из буфера

const unsigned char BREAK = 0x03;
const unsigned char EOL   = 0x0D;
const unsigned char BS    = 0x08;
const unsigned char SPACE = ' ';
#define START_RX_BUFFER &RxBuffer[0]
#define STOP_RX_BUFFER &RxBuffer[15]

/*
 Событие буфер USART пуст (URDE)
*/
//SIGNAL(SIG_UART_DATA)
SIGNAL (USART_UDRE_vect)
{
    char a = 0;

    pBuff_UART++;
    if(OS.FLAGS.UARTSRC)
    {
        a = __LPM(pBuff_UART);
    }
    else
    {
        a = *pBuff_UART;
    }

    if(a != 0x0)
    {
        UDR = a;
    }
    else
    {
        UDRIE_off();
        OS.FLAGS.UARTBSY = 0;        // UART осовбодился
    }
}


//SIGNAL(SIG_UART_RECV)
SIGNAL (USART_RXC_vect)

{
    register unsigned char CHR = UDR;
    register unsigned char *pLastRxPut;

    // Сначало обработаем BREAK
    if(CHR == BREAK)
    {
        OS_STDIN_BREAK();
        return;
    }

    if(!OS.FLAGS.STDOUT)
    {
        UDR = CHR;
    }

    if(CHR == BS)
    {
        // забой - движение назад по буферу с заменой символа на пробел
        if(pRxPut != pRxGet)
        {
            if(--pRxPut < START_RX_BUFFER)
            {
                pRxPut = STOP_RX_BUFFER;  // назад по буферу
            }
        }

        *pRxPut = SPACE;
    }
    else
    {
        // все остальные символы - движение вперед по буферу с записью символа
        *pRxPut = CHR;
        pLastRxPut = pRxPut;

        if(++pRxPut > STOP_RX_BUFFER)
        {
            pRxPut = START_RX_BUFFER; // вперед по буферу
        }

        if(pRxPut == pRxGet)
        {
            // при совпадении позиции PUT и GET считаем буфер переполненым
            pRxPut = pLastRxPut;
            OS_STDIN_OVERFLOW();
        }
    }
}


/*************************************************************************
Name: RxExtractLine

Input:    unsigned char *buff

Returns:  unsigned char

 Функция выделяет линию из буфера UART если таковая в ней присутствует
  возвращаетя
   NULL     -   если линия текста в буфере не найдена
   не NULL  -   если выделение прошло успешно

   в случае успеха изменяет указатель на чтение из буфера pRxGet
   переводя его в новую позицию чтения за <EOL>

   копирование происходит в приемник *buff
*************************************************************************/
unsigned char RxExtractLine(unsigned char *buff)
{
    register unsigned char *ptr = pRxGet;
    register unsigned char *pDst = buff;

    if(pRxGet == pRxPut)
    {
        return 0; // позиции указателей чтения и записи совпадают - нет символов не то что комманд
    }

    while(*ptr != EOL)
    {
        *pDst++ = *ptr;
        if(ptr == pRxPut)
        {
            return 0;  // указетель чтения совпал с указателем записи
        }

        if(++ptr > STOP_RX_BUFFER)
        {
            ptr = START_RX_BUFFER; // вперед по буферу
        }
    }

    // Выход в эту строку означает что териматор <EOL> найен и ptr указывает на него
    *ptr   =  0x00;       // знак EOL уничтожается и заменяется на 0x00
    *pDst  =  0x00;

    if(++ptr > STOP_RX_BUFFER)
    {
        ptr = START_RX_BUFFER; // вперед по буферу
    }

    pRxGet = ptr;         // позиция чтения из буфера переводится в новую позицию за знак <EOL>
    return -1;
}


/*************************************************************************
Name: WaitUART_Ready

Input:    none

Returns:  none
*************************************************************************/
void WaitUART_Ready(void)
{
    UDRIE_on();
    asm volatile(
    "L_01%=:"                    "\n\t"
    "sei"                        "\n\t"
    "sleep"                      "\n\t"
    "lds __tmp_reg__, OS"        "\n\t"
    "sbrc __tmp_reg__, %[nbit]"  "\n\t"
    "rjmp L_01%="                "\n\t"
    :
    : [nbit] "I" (UARTBSY_BIT)
    );
}


/*************************************************************************
Name: putsrom_UART

Input:    char* PROGMEM pPTR

Returns:  none
*************************************************************************/
void putsrom_UART(char* PROGMEM pPTR)
{
    OS.FLAGS.STDOUT  = 1; // Любой вывод в UART устанавливает занятость STDOUT
    OS.FLAGS.UARTBSY = 1;

    pBuff_UART = pPTR;
    OS.FLAGS.UARTSRC = 1; // строку брать из ROM
    WaitUDRE();
    UDR = __LPM(pPTR);
    WaitUART_Ready();
    WaitUDRE();
}


/*************************************************************************
Name: putsram_UART

Input:    char* pPTR

Returns:  none
*************************************************************************/
void putsram_UART(char* pPTR)
{
    OS.FLAGS.STDOUT  = 1; // Любой вывод в UART устанавливает занятость STDOUT
    OS.FLAGS.UARTBSY = 1;

    pBuff_UART = pPTR;
    OS.FLAGS.UARTSRC = 0; // строку брать из RAM
    WaitUDRE();
    UDR = *pPTR;
    WaitUART_Ready();
    WaitUDRE();
}












