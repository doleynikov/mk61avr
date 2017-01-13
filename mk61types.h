// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: mk61types.h
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
#ifndef _MK61TYPES_H_
#define _MK61TYPES_H_

#define VM_START             0x0083
#define DEGREE               0
#define RADIAN               1
#define GRD                  3
#define MK61_KNOP            0x54
#define MK61_GOTO            0x51
#define MK61_READREG         0x60
#define MK61_WRITEREG        0x40
#define EEPROM_MK61PRG       0x00
#define LAST_REGISTER        0x0E  // последний регистр

#define INPUT_START_MK61ADDR 105   // Шаг программы с которого размещается сервисная процедура ввода

/* Сервисные коды управления программно-аппаратным комплексом MK61avr */
#define STEP_BCK 0x80
#define STEP_FRW 0x40
#define MODE_PRG 0x20
#define MODE_AVT 0x10

unsigned char* MK61_GetPC(void);

// Типы данных
typedef void (mk61call)(void);

typedef struct
{
    unsigned char
    Z         : 1,
    b1        : 1,
    b2        : 1,
    b3        : 1,
    b4        : 1,
    b5        : 1,
    b6        : 1,
    S         : 1;
} sALUF;

typedef union
{
    sALUF FLAGS;            // Рассматриваем объединение как набор флагов
    unsigned char   BYTE;   // Рассматриваем объединение как байт данных
} uALUF;

typedef struct
{
    unsigned int
    REFRESH    : 1,  // Обновление содержимого дисплея (X изменился)
    CLOSEX     : 1,  // Ввод числа закончен, в X введено новое число
    ASTACK_UP  : 1,  // Дополнительное перемещение по стеку вверх
    SERVEXEC   : 1,  // Режим запуска сервисной процедуры в спец. адресном прсотарнстве VM61
    DISP       : 1,  // Отобразить буфер дисплея - он изменился
    STEPBYSTEP : 1,  // Режим пошагового исполнения команд
    MODE       : 1,  // Режим работы калькулятора ПРГ/АВТ
    STOP       : 1,  // выполнение программы остановлено
    DETAILED   : 1,  // отображение результатов расчета между шагами программы
    R_GRD_G    : 2;  // положение переключателя Р(01)-ГРД(11)-Г(00)
} sVM_SIGNALS;

typedef union
{
    sVM_SIGNALS    FLAGS;   // Рассматриваем объединение как набор флагов
    unsigned int     VAL;   // Рассматриваем объединение монолитный набор данных
} uVM_61_STATE;

typedef struct
{
    unsigned char lo;
    unsigned char hi;
} sWORD;

typedef union
{
    sWORD  s;
    short int WORD;
} uWORD;

typedef struct
{
    unsigned char prg[256];         // Область программ
    unsigned int  label[0x99];      // Таблица меток
    double        reg[16];          // РОН
    uVM_61_STATE  VM61;             // Флаги управления виртуальной машиной VM61
    unsigned char *pCStack;         // Указатель адреса возвратв из ПП
    unsigned char ret[6];           // Стек возвратов ПП
} sMK61;

extern sMK61 MK61;
extern char   disp[16];
extern unsigned char *pDig;
extern unsigned char LEDbit;
extern char dump[24+24+7];

extern unsigned char Input_rs232(signed char par);
extern unsigned char Go_rs232(signed char par);
extern unsigned char Mem_rs232(signed char par);
extern unsigned char AStack_rs232(signed char par);
extern unsigned char X_rs232(signed char par);
extern unsigned char Step_rs232(signed char par);
extern unsigned char Jmp_rs232(signed char par);
extern unsigned char NextMem_rs232(signed char par);
extern void RxUART_EOL(void);
extern void RxUART_BREAK(void);
extern void VM61_GetOPCODE(unsigned char byte, unsigned char op2);
extern void VM61_GetOPERAND(unsigned char byte);
extern void VM61_EXEC(void);

extern void MK61_Trace(void);
extern void MK61_ToAuto(void);
extern void MK61_ToProg(void);
extern void MK61_FrmtStore(void);
extern void MK61_SaveStore(void);
extern void MK61_LoadStore(void);
extern void MK61_Stop(void);
extern void MK61_Start(void);
extern void MK61_FrwdStep(void);
extern void MK61_BackStep(void);
extern void MK61_GoTop(void);

void code_NOP(void);
void code_00_to_09(void);                   // Маш.коды от 1 до 9 (цифры ввода числа)
void code_0A(void);                         // Маш.код десятичный разделитель (ввод числа)
void code_0B(void);                         // Маш.код смены знака числа /-/
void code_0E(void);                         // Маш.код движение стека вверх (с вершины Х)
void code_0F(void);                         // Маш.код сдвига стека вверх начиная с X1
void code_0D(void);                         // Маш.код Cx очистки X
void code_10_13(void);                      // Маш.код арифметической операции +,-,*,/
void code_14(void);                         // Маш.код обмен XY
void code_15(void);                         // Маш.код 10^X
void code_16(void);                         // Маш.код e^X
void code_17(void);                         // Маш.код Lg(X)
void code_18(void);                         // Маш.код Ln(X)
void code_19(void);                         // Маш.код Arcsin(X)
void code_1A(void);                         // Маш.код Arccos(X)
void code_1B(void);                         // Маш.код Arctg(X)
void code_1C(void);                         // Маш.код Sin(X)
void code_1D(void);                         // Маш.код Cos(X)
void code_1E(void);                         // Маш.код Tg(X)
void code_20(void);                         // Маш.код операция извлечения константы - число Пи
void code_21(void);                         // Маш.код одноместная операция SQRT(X)
void code_22(void);                         // Маш.код одноместная операция X^2
void code_23(void);                         // Маш.код одноместная операция 1/X
void code_24(void);                         // Маш.код двуместная операция X^Y
void code_25(void);                         // Маш.код кольцевого движение стека в обратном направлении
void code_31(void);                         // Маш.код  модуля числа  |X|
void code_40_4E(void);                      // Маш.код запись в память П0...ПE
void code_50(void);                         // Маш.код С/П
void code_51(void);                         // Маш.код БП
void code_52(void);                         // Маш.код В/0 возврат из подпрограммы
void code_53(void);                         // Маш.код ПП переход на подпрограмму
void code_57_59_5C_5E(void);                // Маш.код условного перехода
void code_58_5A_5B_5D(void);                // Маш.код цикла
void code_60_6E(void);                      // Маш.код чтение из памяти ИП0...ИПE
void code_70_7E(void);                      // Маш.код условного перехода x!=0 по косвенному адресу КБП0...КБПE
void code_80_8E(void);                      // Маш.код безусловного перехода по косвенному адресу КБП0...КБПE
void code_90_9E(void);                      // Маш.код условного перехода x>=0 по косвенному адресу КБП0...КБПE
void code_A0_AE(void);                      // Маш.код вызова подпрограммы по косвенному адресу КПП0...КППE
void code_B0_BE(void);                      // Маш.код косвенной записи в регистры КП0..КПЕ
void code_C0_CE(void);                      // Маш.код условного перехода x<0 по косвенному адресу КБП0...КБПE
void code_D0_DE(void);                      // Маш.код косвенное чтение из регистров КИП0..КИПЕ
void code_E0_EE(void);                      // Маш.код условного перехода x==0 по косвенному адресу КБП0...КБПE

#endif // _MK61TYPES_H_
















