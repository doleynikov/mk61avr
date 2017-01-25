// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: extlib.h
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
#ifndef _EXTLIB_H_
#define _EXTLIB_H_

char HEX(unsigned char val);

extern unsigned char atoc(unsigned char *pStr);
extern void BlockWrEEPROM(unsigned char *eeprom, unsigned char *ram, unsigned char count);
extern void BlockRdEEPROM(unsigned char *eeprom, unsigned char *ram, unsigned char count);
extern void __sprintf_P(char *__s, const char *__fmt, ...) __attribute__((naked));

#endif // _EXTLIB_H_

