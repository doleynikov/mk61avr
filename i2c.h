// ***********************************************************
// Project: �������� ���������������� ������������ ��-61 �� AVR:
// http://code.google.com/p/mk61avr/
//
// �������� ��������� ����� ������� �� GIT:
// git clone https://code.google.com/p/mk61avr/
//
// ��������� �� ������� � Google Groups:
// http://groups.google.com/group/mk61avr_talks
//
// Copyright (C) 2009-2011 ������� ��������, ������� �������
//
// Module name: i2c.c
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
#ifndef _I2C_H_
#define _I2C_H_

typedef struct
{
    unsigned char
    addr : 7,       // ���������� ����� ����������
    a16  : 1;       // ���������� 16 ������
} sE24;


extern unsigned char twst;
extern void TWI_Init(void);
extern int ee24xx_read_bytes(sE24 dev, unsigned int eeaddr, int len, uint8_t *buf);
extern int ee24xx_write_bytes(sE24 dev, unsigned int eeaddr, int len, uint8_t *buf);
extern unsigned int ee24xx_check(sE24 dev);
#endif // _I2C_H_
