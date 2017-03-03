// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: scancode_dc.h
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
#ifndef _SCANCODE_DC_H_
#define _SCANCODE_DC_H_


#define op_9    0x09
#define op_0    0x00
#define op_1    0x01
#define op_2    0x02
#define op_3    0x03
#define op_4    0x04
#define op_5    0x05
#define op_6    0x06
#define op_7    0x07
#define op_8    0x08
#define op_cma  0x0A
#define op_csg  0x0B
#define op_E    0x0C
#define op_Cx   0x0D
#define op_Bl   0x0E
#define op_Bx   0x0F
#define op_Add  0x10
#define op_Sub  0x11
#define op_Mul  0x12
#define op_Div  0x13
#define op_XY   0x14
#define op_Tg   0x1E
#define op_10x  0x15
#define op_ex   0x16
#define op_lg   0x17
#define op_ln   0x18
#define op_asin 0x19
#define op_acos 0x1A
#define op_atg  0x1B
#define op_sin  0x1C
#define op_cos  0x1D
#define op_Pi   0x20
#define op_Sqrt 0x21
#define op_Sqr  0x22
#define op_1dx  0x23
#define op_Xy   0x24
#define op_Cyc  0x25
#define op_gm   0x26
#define op_Ks   0x27
#define op_Km   0x28
#define op_Kd   0x29
#define op_gs   0x2A
#define op_gms  0x30
#define op_modx 0x31
#define op_sgn  0x32
#define op_msg  0x33
#define op_int  0x34
#define op_frac 0x35
#define op_Max  0x36м
#define op_and  0x37
#define op_or   0x38
#define op_xor  0x39
#define op_not  0x3A
#define op_rnd  0x3B
#define op_STOP 0x50
#define op_JMP  0x51
#define op_RET  0x52
#define op_SUB  0x53
#define op_Nop  0x54
#define op_K1   0x55
#define op_K2   0x56
#define op_IFNE 0x57
#define op_FOR2 0x58
#define op_IFGE 0x59
#define op_FOR3 0x5A
#define op_FOR1 0x5B
#define op_IFLT 0x5C
#define op_FOR0 0x5D
#define op_IFEQ 0x5E

// Cмещения для MOD_1
#define op_mov_R_X 0x60
#define op_mov_X_R 0x40
#define op_IFNE_R  0x70
#define op_JMP_R   0x80
#define op_IFGE_R  0x90
#define op_SUB_R   0xA0
#define op_ldx_MR  0xB0
#define op_IFLT_R  0xC0
#define op_stx_MR  0xD0
#define op_IFEQ_R  0xE0

// Команды управления МК61avr
#define MK_FRWD    0x00
#define MK_BACK    0x01
#define MK_TRACE   0x02
#define MK_AUTO    0x03
#define MK_PROG    0x04
#define MK_FORMAT  0x05
#define MK_SAVE    0x06
#define MK_LOAD    0x07
#define MK_START   0x08
#define MK_STOP    0x09
#define MK_GOTOP   0x0A

#define SERVICE    0xF0   // Управление комплексом
#define ALTER      0xF1   // Выбор альтернатив декодирования для режимов AUTO или PROG
#define MOD_2      0xF2   // Формирование операнда по схеме донабор 2-х цифр   => OPCODE, {NUM_H,NUM_L}
#define MOD_1      0xF3   // Формирование операнда по схеме донабор 1-ой цифры => OPCODE + NUM_L
#define SHIFT      0xF4   // Сдвинуть шифт на N (N указан в смещении +3)
#define UNKNOWN    0xF5   // Неизветсная ошибочная последовательность или команда

#endif // _SCANCODE_DC_H_
