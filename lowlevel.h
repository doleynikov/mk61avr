// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: lowlevel.h
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
#ifndef _LOWLEVEL_H_
#define _LOWLEVEL_H_

typedef unsigned char uchar;


#define BCDop10000(value,digit)\
__asm__ __volatile__(           \
"ldi %1, -1"             "\n\t" \
"BCD_%=: "               "\n\t" \
"inc %1"                 "\n\t" \
"subi %A0,  0x10"        "\n\t" \
"sbci %B0,  0x27"        "\n\t" \
"brsh BCD_%="            "\n\t" \
"subi %A0,  0xF0"        "\n\t" \
"sbci %B0,  0xD8"        "\n\t" \
:                               \
: "w" (value), "d" (digit)      \
)

#define BCDop1000(value,digit) \
__asm__ __volatile__(           \
"ldi %1, -1"             "\n\t" \
"BCD_%=: "               "\n\t" \
"inc %1"                 "\n\t" \
"subi %A0,  0xE8"        "\n\t" \
"sbci %B0,  0x03"        "\n\t" \
"brsh BCD_%="            "\n\t" \
"subi %A0,  0x18"        "\n\t" \
"sbci %B0,  0xFC"        "\n\t" \
:                               \
: "w" (value), "d" (digit)      \
)

#define BCDop100(value,digit) \
__asm__ __volatile__(           \
"ldi %1, -1"             "\n\t" \
"BCD_%=: "               "\n\t" \
"inc %1"                 "\n\t" \
"subi %A0,  0x64"        "\n\t" \
"sbci %B0,  0x00"        "\n\t" \
"brsh BCD_%="            "\n\t" \
"subi %A0,  0x9C"        "\n\t" \
:                               \
: "w" (value), "d" (digit)      \
)

#define BCDop10(value,digit) \
__asm__ __volatile__(           \
"ldi %1, -1"             "\n\t" \
"BCD_%=: "               "\n\t" \
"inc %1"                 "\n\t" \
"subi %A0,    10"        "\n\t" \
"brsh BCD_%="            "\n\t" \
"subi %A0,   -10"        "\n\t" \
:                               \
: "w" (value), "d" (digit)      \
)

#define expand2x(inVar) \
 __asm__ __volatile__ (       \
"push r16"             "\n\t" \
"ldi r16, 4"           "\n\t" \
"L_%=: " "lsr %0"      "\n\t" \
"ror r1"               "\n\t" \
"lsr r1"               "\n\t" \
"dec r16"              "\n\t" \
"brne L_%="            "\n\t" \
"mov %0,r1"            "\n\t" \
"lsl %0"               "\n\t" \
"or  %0,r1"            "\n\t" \
"eor r1,r1"            "\n\t" \
"pop r16"              "\n\t" \
:                             \
: "r" (inVar)                 \
)

#define swap_if_bit(inVar,bitf,num) \
 __asm__ __volatile__ (       \
"sbrs %1, %2"              "\n\t" \
"swap %0"              "\n\t" \
:                             \
: "r" (inVar), "r" (bitf), "I" (num)                 \
)

#define swap(value) asm volatile("swap %0" : "=r" (value) : "0" (value))

#define bit8_invert(reg)        \
asm volatile(                   \
 "com %0"                "\n\t" \
 "lsl %0"                "\n\t" \
 "com %0"                "\n\t" \
 "ror %0"                "\n\t" \
 :                              \
 : "r" (reg)                    \
)

#endif // _LOWLEVEL_H_

