// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: keyboard.h
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
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

#include "mk61types.h"

// Время подавления переходных процессов клавиш (дребезг) = 0.4 сек
#define TIME_DRIBLING_WAIT          TIME_200ms

#define KEY_P0                      PINA
#define KEY_P1                      PINB
#define SCANPORT                    PORTB
#define SCANDDR                     DDRB
#define MASK_SCANLINE               0x0F
#define MASK_KEY_P1                 0x03
#define TCCR1B_T1_HOLD              (0<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10)
#define TCCR1B_T1_SCAN              (1<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10)

typedef struct
{
    unsigned int
    op61        : 8,
    type        : 2,   // тип полученного опкода 00 - команда, 01 - операнд, 10 - сервис-команда, 11 - неопределенность
    b2          : 1,
    b3          : 1,
    b4          : 1,
    b5          : 1,
    b6          : 1,
    uncomplited : 1;
} sOpContainer;


typedef union
{
    sOpContainer f;
    sWORD val;
} uOpContainer;


typedef struct
{
    unsigned char
    code    :  4,
    line3   :  1,
    line1   :  1,
    line2   :  1,
    line4   :  1;
} sScanCode;


typedef union
{
    sScanCode scan;
    unsigned char val;
} uScanCode;


extern unsigned char RowScan;
extern uScanCode ScanCode;
extern void Keyboard_Init(void);
extern void Keyboard_Scan(void);

#endif // _KEYBOARD_H_


