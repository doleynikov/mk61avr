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
// Module name: config.h
//
// Module description: ���������������� ���� �������
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
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define DEBUG                       // ���� ������ ��������������� ���������� ����������!!!

//#define DEBUG_SCAN_DECODER        // ���� ������ ���������� ���������� �� ������ �������� ����-����

#define DEBUG_STORE                 // ���� ������ ���������� ���������� �� ������ ��������� �������� STORE

#define LIMIT_NUMERIC 11

//#define VMLAB_SIMULATED

#define RC_OSC_CALIBRATE            // TODO

//#define MK61_KEYPAD_IN_USE            // ������������ ���������� �� ��61

#define LCD_CONTRAST_MAX          // ������������� ��������

#endif // _CONFIG_H_

