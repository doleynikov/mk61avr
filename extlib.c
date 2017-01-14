// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: extlib.c
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
#include <stdio.h>
#include <avr/pgmspace.h>
#include "config.h"
#include "extlib.h"

/*************************************************************************
Name: HEX (inline function)

Input:    unsigned char val


Returns:  char
*************************************************************************/
char HEX(unsigned char val)
{
    if((val += '0') > '9')
    {
        val += ('A' - '0' - 0x0A);
    }

    return val;
}

/*************************************************************************
Name: atoc

Input:    unsigned char *pStr

Returns:  unsigned char
*************************************************************************/
unsigned char atoc(unsigned char *pStr)
{
    register unsigned char retv;
    asm volatile(
    "clr __tmp_reg__"           "\n\t"
    "loop%=:"                   "\n\t"
    "ld   %[rv], X+"            "\n\t"
    "subi %[rv], '0'"           "\n\t"
    "cpi  %[rv], 10"            "\n\t"
    "brcs digit%="              "\n\t"
    "subi %[rv], 'A'-'0'-10"    "\n\t"
    "digit%=:"                  "\n\t"
    "swap __tmp_reg__"          "\n\t"
    "or   __tmp_reg__, %[rv]"   "\n\t"
    "com  __zero_reg__"         "\n\t"
    "brne loop%="               "\n\t"
    "mov  %[rv], __tmp_reg__"   "\n\t"
    : [rv]   "=r" (retv)
    : [pStr] "x"  (pStr)
    );
    return retv;
}


/*************************************************************************
Name: BlockWrEEPROM

Input:    unsigned char *eeprom
          unsigned char *ram
          unsigned char count

Returns:  none
*************************************************************************/
void BlockWrEEPROM(unsigned char *eeprom, unsigned char *ram, unsigned char count)
{
    asm volatile(
    "cli"                       "\n\t"
    "wait_eeprom%=:"            "\n\t"
    "sbic %[eecr],%[eewe]"      "\n\t"
    "rjmp wait_eeprom%="        "\n\t"
    "out %[eearh], %B[eeptr]"   "\n\t"
    "out %[eearl], %A[eeptr]"   "\n\t"
    "adiw %A[eeptr], 1"         "\n\t"
    "ld  __tmp_reg__, X+"       "\n\t"
    "out %[eedr], __tmp_reg__"  "\n\t"
    "sbi %[eecr], %[eemwe]"     "\n\t"
    "sbi %[eecr], %[eewe]"      "\n\t"
    "dec %[cnt]"                "\n\t"
    "brne wait_eeprom%="        "\n\t"
    "sei"                       "\n\t"
    :
    : [eecr]  "I" (_SFR_IO_ADDR(EECR)),
      [eewe]  "I" (EEWE),
      [eemwe] "I" (EEMWE),
      [eearh] "I" (_SFR_IO_ADDR(EEARH)),
      [eearl] "I" (_SFR_IO_ADDR(EEARL)),
      [eedr]  "I" (_SFR_IO_ADDR(EEDR)),
      [eeptr] "w" (eeprom),
      [mem]   "x" (ram),
      [cnt]   "r" (count)
    );
}


/*************************************************************************
Name: BlockRdEEPROM

Input:    unsigned char *eeprom
          unsigned char *ram
          unsigned char count

Returns:  none
*************************************************************************/
void BlockRdEEPROM(unsigned char *eeprom, unsigned char *ram, unsigned char count)
{
    asm volatile(
    "cli"                       "\n\t"
    "wait_eeprom%=:"            "\n\t"
    "sbic %[eecr],%[eewe]"      "\n\t"
    "rjmp wait_eeprom%="        "\n\t"
    "out %[eearh], %B[eeptr]"   "\n\t"
    "out %[eearl], %A[eeptr]"   "\n\t"
    "sbi %[eecr], %[eere]"      "\n\t"
    "adiw %A[eeptr], 1"         "\n\t"
    "in  __tmp_reg__, %[eedr]"  "\n\t"
    "st  X+, __tmp_reg__"       "\n\t"
    "dec %[cnt]"                "\n\t"
    "brne wait_eeprom%="        "\n\t"
    "sei"                       "\n\t"
    :
    : [eecr]  "I" (_SFR_IO_ADDR(EECR)),
      [eewe]  "I" (EEWE),
      [eere]  "I" (EERE),
      [eearh] "I" (_SFR_IO_ADDR(EEARH)),
      [eearl] "I" (_SFR_IO_ADDR(EEARL)),
      [eedr]  "I" (_SFR_IO_ADDR(EEDR)),
      [eeptr] "w" (eeprom),
      [mem]   "x" (ram),
      [cnt]   "r" (count)
    );
}


/*************************************************************************
Name: __sprintf_P

Input:    char *__s
          const char *__fmt
          ...

Returns:  none
*************************************************************************/
void __sprintf_P(char *__s, const char *__fmt, ...)
{
    asm volatile(
    "cli                       \n\t"
    "in   __tmp_reg__, %[spl]  \n\t"
    "in   __zero_reg__, %[sph] \n\t"
    "push r16 \n\t"
    "push r26 \n\t"
    "push r27 \n\t"
    "push r28 \n\t"
    "push r29 \n\t"
    "push r30 \n\t"
    "push r31 \n\t"
    "mov  r29, __zero_reg__ \n\t"
    "mov  r28, __tmp_reg__  \n\t"
    "clr  __zero_reg__      \n\t"
    "adiw r28, 3  \n\t"
    "ld   r26, Y+ \n\t"
    "ld   r27, Y+ \n\t"
    "ld   r30, Y+ \n\t"
    "ld   r31, Y+ \n\t"
    "spf_%=:         \n\t"
    "lpm  r16, Z+    \n\t"

    "cpi  r16, 0x25  \n\t"
    "brne next_%=    \n\t"
    "lpm  r16, Z+    \n\t"
    "cpi  r16, 'x'   \n\t"
    "breq utoh_%=    \n\t"
    "cpi  r16, 'c'   \n\t"
    "brne next_f%=   \n\t"
    "ld   r16, Y+    \n\t"
    "rjmp next_%=    \n\t"

    "next_f%=:       \n\t"
    "next_%=:        \n\t"
    "st   X+,  r16   \n\t"
    "isend_%=:       \n\t"
    "tst  r16        \n\t"
    "brne spf_%=     \n\t"
    "pop  r31 \n\t"
    "pop  r30 \n\t"
    "pop  r29 \n\t"
    "pop  r28 \n\t"
    "pop  r27 \n\t"
    "pop  r26 \n\t"
    "pop  r16 \n\t"
    "sei                       \n\t"
    "ret      \n\t"

    "utoh_%=:              \n\t"
    "ld   r16, Y           \n\t"
    "swap r16              \n\t"
    "loop%=:               \n\t"
    "andi r16, 0x0F        \n\t"
    "subi r16, -'0'        \n\t"
    "cpi  r16, '9'+1       \n\t"
    "brlt isdigit%=        \n\t"
    "subi r16, -'A'+'0'+10 \n\t"
    "isdigit%=:            \n\t"
    "st   X+, r16          \n\t"
    "com  __zero_reg__     \n\t"
    "breq isend_%=         \n\t"
    "ld   r16, Y+          \n\t"
    "rjmp loop%=           \n\t"
    :
    : [spl] "I" (_SFR_IO_ADDR(SPL)),
      [sph] "I" (_SFR_IO_ADDR(SPH))
    );
}



