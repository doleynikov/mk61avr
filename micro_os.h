// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// http://code.google.com/p/mk61avr/
//
// Получить локальную копию проекта из GIT:
// git clone https://code.google.com/p/mk61avr/
//
// Дискуссия по проекту в Google Groups:
// http://groups.google.com/group/mk61avr_talks
//
// Copyright (C) 2009-2011 Алексей Сугоняев, Виталий Самуров
//
// Module name: micro_os.h
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
#ifndef _MICRO_OS_H_
#define _MICRO_OS_H_

#define KEYBOARD_EVENT_BIT          0x07
#define KEYBOARD_EVENT_MASK         0x80
#define KEYBOARD_PRESSKEY_BIT       0x06
#define KEYBOARD_PRESSKEY           0x40
#define KEYBOARD_KEYHOLD_BIT        0x05
#define KEYBOARD_KEYHOLD_MASK       0x20
#define UARTROM_BIT                 0
#define UARTBSY_BIT                 1
#define UARTRAM_BIT                 0

typedef struct
{
    unsigned char
    UARTSRC     : 1,        // ФЛАГ    - строку на UART брать из SRC=1 ROM, SRC=0 RAM
    UARTBSY     : 1,        // ФЛАГ    - UART занят
    STDOUT      : 1,        // OS освободила канал для вывода текстовой информации
    b3          : 1,
    QUANT       : 1,        // СОбытие от счетчика времени
    notuse      : 2,
   //KEYHOLD    : 1,        // ФЛАГ - подавления дребезга клавиатуры
   //KEYPRESS   : 1,        // нажатие кнопки на клавиатуре подтверждено
   KEYBRD       : 1;        // Событие от клавиатуры
} sOS_SIGNALS;

typedef union
{
    sOS_SIGNALS   FLAGS;
    signed char   VAL;
} uOS_SIGNALS;

// флаги ОС
extern uOS_SIGNALS  OS;

extern void OS_STDIN_BREAK(void);
extern void OS_STDIN_OVERFLOW(void);

#endif // _MICRO_OS_H_







