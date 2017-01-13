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
// Module name: keyboard.c
//
// Module description: ������ ����������
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
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "micro_os.h"
#include "keyboard.h"
#include "time.h"
#include "mk61types.h"

// ���������� ����������
sTimeTask *sDTRIM;
sTimeTask *sSCANP;
sTimeTask *sSCANR;

void TASK_DTRIM(void);
void TASK_SCAN_PRESS(void);
void TASK_SCAN_RELEASE(void);

unsigned char RowScan;
uScanCode ScanCode;


/*************************************************************************
Name: Keyboard_Init

Input:    none

Returns:  none
*************************************************************************/
void Keyboard_Init(void)
{
    RowScan  = 0x08;
    ScanCode.val = 0x00;

    // �������� ������������ �������� ������������ ����������
    sDTRIM = TIME_AppendTask(TASK_DTRIM, TIME_DRIBLING_WAIT, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    sSCANR = TIME_AppendTask(TASK_SCAN_RELEASE, TIME_10ms, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    sSCANP = TIME_AppendTask(TASK_SCAN_PRESS, TIME_10ms, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    Keyboard_Scan();
}


/*************************************************************************
Name: TASK_DTRIM

Input:    none

Returns:  none

    ######## ����������� ��������� ########
    ��������������������� ������ ���������� �������� ��������� - � �������� �����
    ���������� �������.
    ����� ���������� Thold =  0.4 ���
*************************************************************************/
void TASK_DTRIM(void)
{
    register unsigned char KEY;
    register unsigned char tmp=9;

    // ���������� ����-���
    KEY = ~KEY_P0;
    asm volatile
    (
        "sec"                           "\n\t"
        "loop%=:" "dec %[cnt]"          "\n\t"
        "rol %[key]"                    "\n\t"
        "brcc loop%="                   "\n\t"
        : [cnt] "+r" (tmp), [key] "+r" (KEY)
        :
    );

    KEY = (~KEY_P1)&(MASK_KEY_P1);
    if(KEY)
    {
        KEY += 8;
    }

    if((KEY += tmp))
    {
        // ����� �������� �� ��������, � ���������� ������ ��� - ���������
        // ������ �������� ������������ ����������
        TIME_UpTask(sSCANR);
        ScanCode.val = KEY|RowScan;
    }
    else
    {
        // ����� ���������� �������� ���������� �� ������� (������ ������������)
        // ������� �������
        TIME_UpTask(sSCANP);
    }
}


/*************************************************************************
Name: SetScanLine

Input:    none

Returns:  none
*************************************************************************/
void SetScanLine(void)
{
}


/*************************************************************************
Name: TASK_SCAN_PRESS

Input:    none

Returns:  none

    ######## ����������� ��������� ########
    ��������������������� ������ ������������ ����� ������������ ������� �
    �������� �� �������
    Tscan =  0.01 ���
*************************************************************************/
void TASK_SCAN_PRESS(void)
{
    // ��������� ��������� ����� ������ ����������� ������
    register unsigned char KEYS = KEY_P0 - (KEY_P1|(~MASK_KEY_P1));
    if(KEYS)
    {
        TIME_UpTask(sDTRIM); // ���������� ������� - ��������� ������ ���������� ��������
    }
    else
    {
        // ���������� �� �������, ��������� � ������������ ��������� �����
        KEYS = RowScan << 1;
        if(KEYS == 0)
        {
            KEYS = 0x10;  // ����������� �����, ������� �� ������� ����� ������������
        }

        RowScan = KEYS;
        asm volatile
        (
            "push r28"                  "\n\t"
            "in   r28, %[scanport]"     "\n\t"
            "ori  r28, ~%[scanmask]"    "\n\t"
            "eor  r28, %[rowscan]"      "\n\t"  // �������� ������� ����-���� � ���. ���������
            "out %[scanport], r28"      "\n\t"  // ������ � ���� ������������ ��������� � VCC
            "in   r28, %[scanddr]"      "\n\t"  // ����������� ���� �����������
            "andi r28, %[scanmask]"     "\n\t"
            "or   r28, %[rowscan]"      "\n\t"  // �������� ������� ����-����
            "out %[scanddr], r28"       "\n\t"
            "pop r28"
            :
            : [scanddr]  "I" (_SFR_IO_ADDR(SCANDDR)),
            [scanport] "I" (_SFR_IO_ADDR(SCANPORT)),
            [scanmask] "M" (MASK_SCANLINE),
            [rowscan]  "r" (RowScan)
        );

        TIME_UpTask(sSCANP);
    }
}


/*************************************************************************
Name: TASK_SCAN_RELEASE

Input:    none

Returns:  none

    ######## ����������� ��������� ########
    ��������������������� ������ ������������ ����� ������������ ������� � ��������
    �� �������
    Tscan =  0.01 ���
*************************************************************************/
void TASK_SCAN_RELEASE(void)
{
    // ��������� ��������� ������ ���������� ������� � ������
    register unsigned char KEYS = KEY_P0 - (KEY_P1|(~MASK_KEY_P1));
    if(!KEYS)
    {
        OS.FLAGS.KEYBRD = 1; // ���������� ��������� - ������� �� � ��� ��� ���������� ������ ���
    }
    else
    {
        TIME_UpTask(sSCANR);
    }
}



/*************************************************************************
Name: Keyboard_Scan

Input:    none

Returns:  none

    ��� ������� ��������� ��, ��������� ������������ �����
*************************************************************************/
void Keyboard_Scan(void)
{
    OS.FLAGS.KEYBRD = 0;
    TIME_UpTask(sSCANP);
}


