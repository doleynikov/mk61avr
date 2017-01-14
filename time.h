// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: time.h
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
#ifndef _TIME_CLASS_H_
#define _TIME_CLASS_H_

#define TIME_1sec           200   // 1 сек
#define TIME_400ms          80    // 0,4 сек
#define TIME_200ms          40    // 0,2 сек
#define TIME_150ms          30    // 0,15 сек
#define TIME_10ms           2     // 0.01 сек
#define TIME_5ms            1     // 0.005 сек

#define MAX_TASK            5
#define TIME_REPEATED_TASK  0x00
#define TIME_DISPOSABLE_TASK 0x01

// Характеристики задач
#define TIME_TASK_EMPTY     0x40
#define TIME_TASK_SKIP      0x80
#define TIME_TASK_UNITARY   0x01

#define TIMER_COMPARE_CONST 625

typedef void (TaskCall)(void);  // Тип вызова задачи
typedef long int TTime;         // Тип переменной для хранения и обработки времени в ОС

typedef struct
{
    unsigned long int
    T           : 24,
    kill        : 1,
    nouse       : 5,
    empty       : 1,
    skip        : 1;
} sTimeAction;


typedef struct
{
    unsigned char
    kill        : 1,
    nouse       : 5,
    empty       : 1,
    skip        : 1;
} sTaskFlags;

typedef union
{
    unsigned char val;
    sTaskFlags set;
} uTaskFlags;

typedef union
{
    //  unsigned long int val;
    sTimeAction set;
    unsigned char v[4];
} uTimeAction;

// Элемент очереди задач планировщика по времени
typedef struct
{
    uTimeAction TimeMark;
    long int    uptime;
    TaskCall    *call;
} sTimeTask;

extern void TIME_Schedule(void);
extern void TIME_Init(void);
extern void TIME_UpTask(sTimeTask *pTask);
extern sTimeTask* TIME_AppendTask(TaskCall *tcall, long int delta, unsigned char flags);

sTimeTask* SeekEmpty(void);

#endif // _TIME_CLASS_H_

