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
// Module name: terminal.h
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
#ifndef _TERMINAL_H_
#define _TERMINAL_H_

typedef unsigned char (shellcall)(signed char);

extern unsigned char MK61ret[6];
extern double MK61reg[16];
extern unsigned char *pCStack;
extern unsigned char* MK61_GetPC(void);
extern shellcall *CmdHook;
extern char disp[16];

typedef struct
{
    char symbol;          // Символ - деректива команды
    shellcall *call;      // Сам вызов процедуры обслуживания команды
} cCommand;

extern unsigned char unknown_rs232(signed char par);
extern unsigned char SaveFlash(signed char par);
extern unsigned char LoadFlash(signed char par);
extern unsigned char CStack_rs232(signed char par);
extern unsigned char Regs_rs232(signed char par);
extern unsigned char dump_rs232(signed char par);
extern unsigned char Load_rs232(signed char par);
extern unsigned char ExecuteShell(unsigned char *pCmd);
extern unsigned char Ctrl_rs232(signed char par);

#endif // _TERMINAL_H_



















