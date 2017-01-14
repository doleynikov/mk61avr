// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: keyboard.c
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
#include "config.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "micro_os.h"
#include "keyboard.h"
#include "time.h"
#include "mk61types.h"

// √лобальные переменные
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

    // «агрузка планировщика задачами обслуживани¤ клавиатуры
    sDTRIM = TIME_AppendTask(TASK_DTRIM, TIME_DRIBLING_WAIT, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    sSCANR = TIME_AppendTask(TASK_SCAN_RELEASE, TIME_10ms, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    sSCANP = TIME_AppendTask(TASK_SCAN_PRESS, TIME_10ms, TIME_TASK_SKIP|TIME_TASK_UNITARY);
    Keyboard_Scan();
}


/*************************************************************************
Name: TASK_DTRIM

Input:    none

Returns:  none

    ######## ѕЋјЌ»–”≈“—я Ў≈ƒ”Ћ≈–ќћ ########
    ќднократнозапускаема¤ задача подавлени¤ дребезга контактов - и проверка после
    подавлени¤ нажати¤.
    ¬рем¤ подавлени¤ Thold =  0.4 сек
*************************************************************************/
void TASK_DTRIM(void)
{
    register unsigned char KEY;
    register unsigned char tmp=9;

    // ќпредел¤ем скан-код
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
        // после таймаута по дребезгу, с клавиатуры считан код - поднимаем
        // задачу ожидани¤ освобождени¤ клавиатуры
        TIME_UpTask(sSCANR);
        ScanCode.val = KEY|RowScan;
    }
    else
    {
        // после подавлени¤ дребезга клавиатура не активна (ложное срабатывание)
        // ожидаем нажати¤
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

    ######## ѕЋјЌ»–”≈“—я Ў≈ƒ”Ћ≈–ќћ ########
    ќднократнозапускаема¤ задача сканировани¤ строк клавиатурной матрицы и
    проверки на нажатие
    Tscan =  0.01 сек
*************************************************************************/
void TASK_SCAN_PRESS(void)
{
    // —читываем состо¤ние линий портов клавиатурых строки
    register unsigned char KEYS = KEY_P0 - (KEY_P1|(~MASK_KEY_P1));
    if(KEYS)
    {
        TIME_UpTask(sDTRIM); //  лавиатура активна - поднимаем задачу подавлени¤ дребезга
    }
    else
    {
        //  лавиатура не активна, переходим к сканированию следующей линии
        KEYS = RowScan << 1;
        if(KEYS == 0)
        {
            KEYS = 0x10;  // циклическа¤ смена, переход на младшую линию сканировани¤
        }

        RowScan = KEYS;
        asm volatile
        (
            "push r28"                  "\n\t"
            "in   r28, %[scanport]"     "\n\t"
            "ori  r28, ~%[scanmask]"    "\n\t"
            "eor  r28, %[rowscan]"      "\n\t"  // копируем разр¤ды скан-кода с доп. инверсией
            "out %[scanport], r28"      "\n\t"  // плюнем в порт сканировани¤ поддт¤жки к VCC
            "in   r28, %[scanddr]"      "\n\t"  // синтезируем порт направлени¤
            "andi r28, %[scanmask]"     "\n\t"
            "or   r28, %[rowscan]"      "\n\t"  // копируем разр¤ды скан-кода
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

    ######## ѕЋјЌ»–”≈“—я Ў≈ƒ”Ћ≈–ќћ ########
    ќднократнозапускаема¤ задача сканировани¤ строк клавиатурной матрицы и проверки
    на нажатие
    Tscan =  0.01 сек
*************************************************************************/
void TASK_SCAN_RELEASE(void)
{
    // —читываем состо¤ние портов клавиатуры колонки и строки
    register unsigned char KEYS = KEY_P0 - (KEY_P1|(~MASK_KEY_P1));
    if(!KEYS)
    {
        OS.FLAGS.KEYBRD = 1; //  лавиатура неактивна - сообщим ќ— о том что клавиатура выдала код
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

     од клавиши обработан ќ—, разрешаем сканирование далее
*************************************************************************/
void Keyboard_Scan(void)
{
    OS.FLAGS.KEYBRD = 0;
    TIME_UpTask(sSCANP);
}


