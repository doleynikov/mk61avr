// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: store.h
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
#ifndef _STORE_H_
#define _STORE_H_

#define CNT_BYTE_TO_PACK 4

#define STORE_DEVICE (sE24) {addr:0,a16:1}

typedef struct
{
    char name[16];
    unsigned int Size;
    unsigned int FLast;
    unsigned int FRLast;
} sMBR;

typedef struct
{
    unsigned char Num;
    unsigned char CodeSize;
    unsigned int  Start;
} FILE;

extern signed char STORE_format(unsigned int eesize);
extern void STORE_fwrite(unsigned char nPrg, unsigned char SizeCode);
extern void STORE_fread(unsigned char nPrg);
extern signed char STORE_init(void);
extern void EEPROM_PutErrorCode(int ret);

#endif // _STORE_H_

