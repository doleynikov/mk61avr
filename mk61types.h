// ***********************************************************
// Project: �������� ���������������� ������������ ��-61 �� AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (�) 2017 ������� ��������, ������� �������, ������� ���������
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
#define LAST_REGISTER        0x0E  // ��������� �������

#define INPUT_START_MK61ADDR 105   // ��� ��������� � �������� ����������� ��������� ��������� �����

/* ��������� ���� ���������� ����������-���������� ���������� MK61avr */
#define STEP_BCK 0x80
#define STEP_FRW 0x40
#define MODE_PRG 0x20
#define MODE_AVT 0x10

unsigned char* MK61_GetPC(void);

// ���� ������
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
    sALUF FLAGS;            // ������������� ����������� ��� ����� ������
    unsigned char   BYTE;   // ������������� ����������� ��� ���� ������
} uALUF;

typedef struct
{
    unsigned int
    REFRESH    : 1,  // ���������� ����������� ������� (X ���������)
    CLOSEX     : 1,  // ���� ����� ��������, � X ������� ����� �����
    ASTACK_UP  : 1,  // �������������� ����������� �� ����� �����
    SERVEXEC   : 1,  // ����� ������� ��������� ��������� � ����. �������� ������������ VM61
    DISP       : 1,  // ���������� ����� ������� - �� ���������
    STEPBYSTEP : 1,  // ����� ���������� ���������� ������
    MODE       : 1,  // ����� ������ ������������ ���/���
    STOP       : 1,  // ���������� ��������� �����������
    DETAILED   : 1,  // ����������� ����������� ������� ����� ������ ���������
    R_GRD_G    : 2;  // ��������� ������������� �(01)-���(11)-�(00)
} sVM_SIGNALS;

typedef union
{
    sVM_SIGNALS    FLAGS;   // ������������� ����������� ��� ����� ������
    unsigned int     VAL;   // ������������� ����������� ���������� ����� ������
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
    unsigned char prg[256];         // ������� ��������
    unsigned int  label[0x99];      // ������� �����
    double        reg[16];          // ���
    uVM_61_STATE  VM61;             // ����� ���������� ����������� ������� VM61
    unsigned char *pCStack;         // ��������� ������ �������� �� ��
    unsigned char ret[6];           // ���� ��������� ��
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
void code_00_to_09(void);                   // ���.���� �� 1 �� 9 (����� ����� �����)
void code_0A(void);                         // ���.��� ���������� ����������� (���� �����)
void code_0B(void);                         // ���.��� ����� ����� ����� /-/
void code_0E(void);                         // ���.��� �������� ����� ����� (� ������� �)
void code_0F(void);                         // ���.��� ������ ����� ����� ������� � X1
void code_0D(void);                         // ���.��� Cx ������� X
void code_10_13(void);                      // ���.��� �������������� �������� +,-,*,/
void code_14(void);                         // ���.��� ����� XY
void code_15(void);                         // ���.��� 10^X
void code_16(void);                         // ���.��� e^X
void code_17(void);                         // ���.��� Lg(X)
void code_18(void);                         // ���.��� Ln(X)
void code_19(void);                         // ���.��� Arcsin(X)
void code_1A(void);                         // ���.��� Arccos(X)
void code_1B(void);                         // ���.��� Arctg(X)
void code_1C(void);                         // ���.��� Sin(X)
void code_1D(void);                         // ���.��� Cos(X)
void code_1E(void);                         // ���.��� Tg(X)
void code_20(void);                         // ���.��� �������� ���������� ��������� - ����� ��
void code_21(void);                         // ���.��� ����������� �������� SQRT(X)
void code_22(void);                         // ���.��� ����������� �������� X^2
void code_23(void);                         // ���.��� ����������� �������� 1/X
void code_24(void);                         // ���.��� ���������� �������� X^Y
void code_25(void);                         // ���.��� ���������� �������� ����� � �������� �����������
void code_31(void);                         // ���.���  ������ �����  |X|
void code_40_4E(void);                      // ���.��� ������ � ������ �0...�E
void code_50(void);                         // ���.��� �/�
void code_51(void);                         // ���.��� ��
void code_52(void);                         // ���.��� �/0 ������� �� ������������
void code_53(void);                         // ���.��� �� ������� �� ������������
void code_57_59_5C_5E(void);                // ���.��� ��������� ��������
void code_58_5A_5B_5D(void);                // ���.��� �����
void code_60_6E(void);                      // ���.��� ������ �� ������ ��0...��E
void code_70_7E(void);                      // ���.��� ��������� �������� x!=0 �� ���������� ������ ���0...���E
void code_80_8E(void);                      // ���.��� ������������ �������� �� ���������� ������ ���0...���E
void code_90_9E(void);                      // ���.��� ��������� �������� x>=0 �� ���������� ������ ���0...���E
void code_A0_AE(void);                      // ���.��� ������ ������������ �� ���������� ������ ���0...���E
void code_B0_BE(void);                      // ���.��� ��������� ������ � �������� ��0..���
void code_C0_CE(void);                      // ���.��� ��������� �������� x<0 �� ���������� ������ ���0...���E
void code_D0_DE(void);                      // ���.��� ��������� ������ �� ��������� ���0..����
void code_E0_EE(void);                      // ���.��� ��������� �������� x==0 �� ���������� ������ ���0...���E

#endif // _MK61TYPES_H_
















