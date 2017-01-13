// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2016 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: time.c
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
#include  <time.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <micro_os.h>

TTime time;
sTimeTask TimeTaskList[MAX_TASK];  // Очередь задач запуска по времени


/*************************************************************************
Name: TIME_Init

Input:    none

Returns:  none
*************************************************************************/
void TIME_Init(void)
{
    // Датчик времени обнулим
    time = 0;

    // Каждую задачу списка задач шедуллера установим в EMPTY
    register sTimeTask *pTask = &TimeTaskList[0];

    do
    {
        pTask->TimeMark.v[3] = TIME_TASK_EMPTY;
    } while(++pTask <= &TimeTaskList[MAX_TASK]);

    // Подключаем прерывания по таймеру 1
    TCCR1B = (1<<WGM12)|(1<<CS11)|(1<<CS10);
    OCR1A  = TIMER_COMPARE_CONST;
    TIMSK |=  (1<<OCIE1A);
}


/*************************************************************************
Name: TIME_Schedule

Input:    none

Returns:  none

    Шедулинг задач в очереди
*************************************************************************/
void TIME_Schedule(void)
{
    register sTimeTask *pTask = &TimeTaskList[0];

    do
    {
        if(pTask->TimeMark.v[3]&(TIME_TASK_SKIP|TIME_TASK_EMPTY))
        {
            continue;
        }

        if(pTask->uptime <= time)
        {
            if(pTask->TimeMark.set.kill)
            {
                pTask->TimeMark.set.skip = 1; // сброс одноразовых задач
            }
            else
            {
                pTask->uptime = pTask->TimeMark.set.T + time;
            }

            pTask->call();  // исполним вызов задачи
        }
    } while(++pTask < &TimeTaskList[MAX_TASK]);
}


/*************************************************************************
Name: TIME_UpTask

Input:    sTimeTask *pTask

Returns:  none
*************************************************************************/
void TIME_UpTask(sTimeTask *pTask)
{
    pTask->uptime = time + pTask->TimeMark.set.T;
    pTask->TimeMark.set.skip = 0;
}


/*************************************************************************
Name: SeekEmpty

Input:    none

Returns:  sTimeTask*
*************************************************************************/
sTimeTask* SeekEmpty(void)
{
    register sTimeTask *pTask = &TimeTaskList[0];

    while(!pTask->TimeMark.set.empty)
    {
        if(++pTask > &TimeTaskList[MAX_TASK])
        {
            return 0;
        }
    }

    return pTask;
}


/*************************************************************************
Name: TIME_AppendTask

Input:    TaskCall *tcall
          long int delta
          unsigned char flags

Returns:  sTimeTask*
*************************************************************************/
sTimeTask* TIME_AppendTask(TaskCall *tcall, long int delta, unsigned char flags)
{
    register sTimeTask *pTask = SeekEmpty();

    if(pTask)
    {
        pTask->call = tcall;
        pTask->TimeMark.set.T = delta;

        //pTask->uptime = time + delta; //pTask->TimeMark.set.T;
        TIME_UpTask(pTask);
        pTask->TimeMark.v[3] = flags;
    }

    return pTask;
}


/*
 При F = 8000000 и div=1024
  T = 64/8000000 = 0,000008 сек
  Квант времени 0.005сек = 5mks
  Константа срабатывания таймера 625
*/
SIGNAL(SIG_OUTPUT_COMPARE1A)
{
    time++;
    OS.FLAGS.QUANT = 1;
}






