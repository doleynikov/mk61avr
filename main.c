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
// Module name: main.c
//
// Module description: Main-���� �������
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
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include <math.h>
#include "micro_os.h"
#include "mk61types.h"
#include "usart_module.h"
#include "time.h"
#include "i2c.h"
#include "store.h"
#ifdef MK61_KEYPAD_IN_USE
 #include "keyboard.h"
 #include "scancode_dc.h"
#endif // #ifdef MK61_KEYPAD_IN_USE
#include "terminal.h"
#include "lcd.h"
#include "extlib.h"

void InitHAL(void);
void InitVM61(void);
void MK61_CStack_push(void);
void MK61_CStack_pop(void);
void MK61_AStack_push(void);
void MK61_AStack_pop(void);
void ExecRS232Stop(void);
void OutputLCD_m1(void);
void OutputLCD_m0(void);
void MK61_displayed(void);

typedef union
{
    long int val;
    char a[4];
} uVector;

double to_R_GR_G(double X);
double from_R_GR_G(double X);

uOS_SIGNALS OS;

register unsigned char PC asm("r2");
register unsigned char OPCODE asm("r3");

// �������������� ����
register double X asm("r4");
register double Y asm("r8");
register double X1 asm("r12");
double Z,T;


register uALUF ALU61 asm("r16");        // ����� ��� ����������� ������ VM61
//uVM_61_STATE VM61;                    // ����� ���������� ����������� ������� VM61

// ����������� �������
register unsigned char TEMP asm("r17");

char disp[16];                 // ����� ��� ��������� CHAR ����������� ������
char dispprg[12];              // ����� ��� ������������ ������� ��� ����������������
char *pPutX = disp;            // ��������� � ������ CHAR ����������� X �� ������� ����� �����

/*
// ������ ���-����� (���������) ��61 105 - �����, ����� ������������� ��� ������������� �����
unsigned char MK61.prg[256];
// ������� �����
unsigned int label[256];
// �������� ������ ���������� ��61 0..E - ���
double MK61.reg[16];
// ���� ��������� MK61
unsigned char MK61ret[6];
unsigned char *pCStack = &MK61ret[5];
*/
sMK61 MK61;


shellcall *CmdHook;
unsigned char COMMAND_BUFFER[16];

const char aStepLine[]     PROGMEM = "%x. %x\r";
const char aStopLine[]     PROGMEM = "\rmk61avr [%x. %x]>";
const char aMemStartLine[] PROGMEM = "%x. ";
const char aProgramLine[]  PROGMEM = "%x:%x:%x:%x";
const char aKbdExecEvent[] PROGMEM = "\rrun";

const unsigned char PROGMEM MK61op[16] =
/* *    +    ,    -    .    /    0    1    2    3    4    5    6    7    8    9 */
 {0x12,0x10,0x54,0x11,0x0A,0x13,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09};


// ������ ��� ������ � ������� UCSRA
#define UCS_A (1<<U2X)

// ������ ��� ������ � ������� UCSRB
#define UCS_B ((1<<RXEN)|(1<<TXEN)|(1<<UDRIE)|(1<<RXCIE))

// ������ ��� ������ � ������� UCSRC
#define UCS_C (1<<URSEL)|(1<<USBS)|(1<<UCSZ1)|(1<<UCSZ0)
#define BOD_2400 416


/*************************************************************************
Name: JMP_TABLE

Input:    none

Returns:  none
*************************************************************************/
void JMP_TABLE(void);


/*************************************************************************
Name: InitHAL

Input:    none

Returns:  none

    ��������� ������������ ��������� ������� � ����� USART
*************************************************************************/
void InitHAL(void)
{
    MCUCR |= (1<<SE);
    asm volatile(
    "push r28"                  "\n\t"
    "ldi r28, 0x02"             "\n\t"      // ����� TX �� �����, ����� RX �� ����, ��������� �� ����
    "out %[ddrd], r28"          "\n\t"
    "ldi r28, 0xFF"             "\n\t"
    "out %[portd], r28"         "\n\t"      // ��� ����� ��������� � VCC

#ifdef MK61_KEYPAD_IN_USE
    "clr r28"                   "\n\t"
    "out %[ddrc], r28"          "\n\t"      // ��� ����� ����� C �� ����
    "out %[ddra], r28"          "\n\t"      // ��� ����� ����� A �� ����
    "out %[ddrb], r28"          "\n\t"      // ��� ����� ����� B �� ����
    "ser r28"                   "\n\t"
    "out %[portc], r28"         "\n\t"      // ��� ����� ����� C ��������� � VCC
    "out %[porta], r28"         "\n\t"      // ��� ����� ����� � ��������� � VCC
    "out %[portb], r28"         "\n\t"      // ��� ����� ����� B ��������� � VCC
#endif // #ifdef MK61_KEYPAD_IN_USE

    "ldi r28, hi8(416)"         "\n\t"      // hi baud rate
    "out %[ubrrh], r28"         "\n\t"
    "ldi r28, lo8(416)"         "\n\t"      // lo baud rate
    "out %[ubrrl], r28"         "\n\t"
    "ldi r28, %[ucs_A]"         "\n\t"
    "out %[ucsra], r28"         "\n\t"
    "ldi r28, %[ucs_B]"         "\n\t"
    "out %[ucsrb], r28"         "\n\t"
    "ldi r28, %[ucs_C]"         "\n\t"      // URSEL, USBS, UCSZ1, UCSZ0 2 stop bit,
    "out %[ucsrc], r28"         "\n\t"
#ifdef RC_OSC_CALIBRATE
    "ldi r28, %[oscval]"        "\n\t"
    "out %[osccal], r28"        "\n\t"
#endif // #ifdef RC_OSC_CALIBRATE
    "pop r28"                   "\n\t"
    :
    : [ddrb] "I" (_SFR_IO_ADDR(DDRB)),
      [ddrd] "I" (_SFR_IO_ADDR(DDRD)),
      [ddrc] "I" (_SFR_IO_ADDR(DDRC)),
      [ddra] "I" (_SFR_IO_ADDR(DDRA)),
      [portb] "I" (_SFR_IO_ADDR(PORTB)),
      [portd] "I" (_SFR_IO_ADDR(PORTD)),
      [portc] "I" (_SFR_IO_ADDR(PORTC)),
      [porta] "I" (_SFR_IO_ADDR(PORTA)),
      [ubrrh] "I" (_SFR_IO_ADDR(UBRRH)),
      [ubrrl] "I" (_SFR_IO_ADDR(UBRRL)),
      [ucsra] "I" (_SFR_IO_ADDR(UCSRA)),
      [ucsrb] "I" (_SFR_IO_ADDR(UCSRB)),
      [ucsrc] "I" (_SFR_IO_ADDR(UCSRC)),
      [osccal] "I" (_SFR_IO_ADDR(OSCCAL)),
      [oscval] "M" (0xB0),                  //B0
      [ucs_A] "M" (UCS_A),
      [ucs_B] "M" (UCS_B),
      [ucs_C] "M" (UCS_C)
    );

    pBuff_UART   = 0;
    OS.VAL   = 0;
    dump[49] = 0x00;

    // ������������� ��������� ������� OS � ��������� ������� �� �������
    TIME_Init();

    // ������������� ������ TWI (I2C)
    TWI_Init();
    STORE_init();

    // Init lcd
    lcd44780_init(LCD_DISP_ON);

    // Clear display and home cursor
    lcd44780_clrscr();

#ifdef MK61_KEYPAD_IN_USE
    Keyboard_Init();
    ScanDC_RESET();
#endif

    // ��������� "���"
#ifdef __AVR_ATmega8__
    MCUCR |= (1<<SE)|(0<<SM2)|(0<<SM1)|(0<<SM0);       // standby for sleep
#endif

#ifdef __AVR_ATmega16__
    MCUCR  |= (1<<SE)|(0<<SM2)|(0<<SM1)|(0<<SM0);
    // EMCUCR |= (0<<SM0);
#endif
}


/*************************************************************************
Name: InitVM61

Input:    none

Returns:  none
*************************************************************************/
void InitVM61(void)
{
    X1 = X = Y = Z = T = 0;
    PC = 0;
    MK61.VM61.VAL = VM_START;
    MK61.VM61.FLAGS.R_GRD_G = DEGREE;
    MK61.pCStack = &MK61.ret[5];
    memset(disp,0,16);
    memset(MK61.reg,0,sizeof(double[15]));
    memset(MK61.ret,0,6);
}



/*************************************************************************
Name: Input_rs232

Input:    signed char par

Returns:  unsigned char

    ���� � VM61 �� ������� ����������� ���������� - �� ����� 16 �������� ������� '='
*************************************************************************/
unsigned char Input_rs232(signed char par)
{
    register unsigned char *pServMK61  = &MK61.prg[105];
    register unsigned char *pCmd = &COMMAND_BUFFER[1];
    register unsigned char opcode61;

    while((opcode61 = *pCmd))
    {
        opcode61 -= '*';
        pCmd++;

        if(opcode61 < 17)
        {
            opcode61 = __LPM(&MK61op[0] + opcode61);
        }
        else if(opcode61 == ('~' - '*'))
        {
            opcode61 = 0x0B;
        }
        else if(opcode61 == ('r' - '*'))
        {
            opcode61 += ('*' - 'r' + 0x60);
            if(*pCmd <= '9')
            {
                opcode61 |= (*pCmd - '0');
            }
            else
            {
                opcode61 |= (*pCmd - 'A' + 10);
            }
        }
        else if(opcode61 == ('w' - '*'))
        {
            opcode61 += ('*' - 'w' + 0x40);

            if(*pCmd <= '9')
            {
                opcode61 |= (*pCmd - '0');
            }
            else
            {
                opcode61 |= (*pCmd - 'A' + 10);
            }
        }
        else
        {
            opcode61 = MK61_KNOP;
        }

        *pServMK61++ = opcode61;
    }

    *pServMK61++ = MK61_GOTO;
    *pServMK61   = PC;
    PC = 105;

    MK61.VM61.FLAGS.STOP = 0;
    MK61.VM61.FLAGS.SERVEXEC = 1;
    return 0;
}


/*************************************************************************
Name: Jmp_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char Jmp_rs232(signed char par)
{
    PC = 0;
    if(par >= 0)
    {
        PC = par;
    }

    return -1;
}


/*************************************************************************
Name: Step_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char Step_rs232(signed char par)
{
    register short int DT =  (short int) MK61.prg[PC];
    DT = (DT<<8) | ((short int) PC);

    MK61.VM61.FLAGS.STEPBYSTEP = 1;
    MK61.VM61.FLAGS.STOP = 0;

    __sprintf_P(&dump[0],aStepLine, (short int) DT);
    putsram_UART(&dump[0]);

    OS.FLAGS.STDOUT = 0; // ����������� STDOUT ����� ��� ������������ ������
    return 0;
}


/*************************************************************************
Name: Go_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char Go_rs232(signed char par)
{
    if(par >= 0)
    {
        PC = par;
    }

    MK61.VM61.FLAGS.STOP = 0;
    return 0;
}


/*************************************************************************
Name: AStack_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char AStack_rs232(signed char par)
{
    asm("push r16");
    dtostrf(X,8,8,&dump[0]);        // TODO: �������� ������ R16
    dtostrf(Y,8,8,&dump[11]);       // �������� ������ R16
    dtostrf(Z,8,8,&dump[22]);       // �������� ������ R16
    dtostrf(T,8,8,&dump[33]);       // �������� ������ R16
    dtostrf(X1,8,8,&dump[44]);      // �������� ������ R16
    asm("pop r16");
    dump[10] = '\r';
    dump[21] = '\r';
    dump[32] = '\r';
    dump[43] = '\r';
    dump[54] = 0;
    putsram_UART(dump);

    return -1;
}


/*************************************************************************
Name: Mem_rs232

Input:    signed char par

Returns:  unsigned char

    ��������� ����� � ������ �������� ��61 �� ����� ���������� � ������� mXX
*************************************************************************/
unsigned char Mem_rs232(signed char par)
{
    if(par >= 0)
    {
        PC = par;
    }

    __sprintf_P(&dump[0], aMemStartLine, (short int) PC);
    putsram_UART(&dump[0]);

    CmdHook = (shellcall*) &NextMem_rs232;
    OS.FLAGS.STDOUT = 0; // ����������� STDOUT ����� ��� ������������ ������
    return 0;
}


/*************************************************************************
Name: NextMem_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char NextMem_rs232(signed char par)
{
    MK61.prg[PC++] = par;

    __sprintf_P(&dump[0], aMemStartLine, (short int) PC);
    putsram_UART(&dump[0]);

    OS.FLAGS.STDOUT = 0; // ����������� STDOUT ����� ��� ������������ ������
    return 0;
}


/*************************************************************************
Name: X_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char X_rs232(signed char par)
{
    register char *pDump = (char*) &dump[0];

    putsram_UART(disp); // ����� ����������� ������ ������� X

    *pDump++ = '\r';
    *pDump++ = 'X';
    *pDump++ = '=';
    asm("push r16");
    dtostrf(X,LIMIT_NUMERIC,LIMIT_NUMERIC-1,pDump);  // �������� ������ R16
    asm("pop r16");
    putsram_UART(dump);

    return -1;
}


/*************************************************************************
Name: MK61_Trace

Input:    none

Returns:  none
*************************************************************************/
void MK61_Trace(void)
{
    Step_rs232(0);
}


/*************************************************************************
Name: MK61_ToAuto

Input:    none

Returns:  none
*************************************************************************/
void MK61_ToAuto(void)
{
    MK61.VM61.FLAGS.MODE = 0;
}


/*************************************************************************
Name: MK61_ToProg

Input:    none

Returns:  none
*************************************************************************/
void MK61_ToProg(void)
{
    MK61.VM61.FLAGS.MODE = 1;
}


/*************************************************************************
Name: MK61_FrmtStore

Input:    none

Returns:  none
*************************************************************************/
void MK61_FrmtStore(void)
{
}


/*************************************************************************
Name: MK61_SaveStore

Input:    none

Returns:  none
*************************************************************************/
void MK61_SaveStore(void)
{
}


/*************************************************************************
Name: MK61_LoadStore

Input:    none

Returns:  none
*************************************************************************/
void MK61_LoadStore(void)
{
}


/*************************************************************************
Name: MK61_Stop

Input:    none

Returns:  none
*************************************************************************/
void MK61_Stop(void)
{
    MK61.VM61.FLAGS.STOP = 1;
}


/*************************************************************************
Name: MK61_Start

Input:    none

Returns:  none
*************************************************************************/
void MK61_Start(void)
{
    MK61.VM61.FLAGS.STOP = 0;
}


/*************************************************************************
Name: MK61_FrwdStep

Input:    none

Returns:  none
*************************************************************************/
void MK61_FrwdStep(void)
{
    if(PC<103)
    {
        PC++;
    }
}


/*************************************************************************
Name: MK61_BackStep

Input:    none

Returns:  none
*************************************************************************/
void MK61_BackStep(void)
{
    if(PC)
    {
        PC--;
    }
}


/*************************************************************************
Name: MK61_GoTop

Input:    none

Returns:  none
*************************************************************************/
void MK61_GoTop(void)
{
    PC = 0;
}


/*************************************************************************
Name: MK61_CStack_push

Input:    none

Returns:  none
*************************************************************************/
void MK61_CStack_push(void)
{
    register unsigned char *pSP = MK61.pCStack;

    //*MK61.pCStack = PC + 1;
    *pSP-- = PC + 1;

/*
    if(MK61.pCStack > &MK61.ret[0])
    {
        MK61.pCStack--;
    }
*/
    if(pSP >= &MK61.ret[0])
    {
        MK61.pCStack = pSP;
    }
}


/*************************************************************************
Name: MK61_CStack_pop

Input:    none

Returns:  none
*************************************************************************/
void MK61_CStack_pop(void)
{
    register unsigned char *pSP = MK61.pCStack;
    PC = *pSP++;

    if(pSP <= &MK61.ret[5])
    {
        PC = *pSP;
        MK61.pCStack = pSP;
    }

/*
    if(MK61.pCStack < &MK61.ret[5])
    {
        MK61.pCStack++;
    }

    PC = *MK61.pCStack;
*/
}


/*************************************************************************
Name: MK61_AStack_push

Input:    none

Returns:  none
*************************************************************************/
void MK61_AStack_push(void)
{
    T = Z;
    Z = Y;
    Y = X;
}


/*************************************************************************
Name: MK61_AStack_pop

Input:    none

Returns:  none
*************************************************************************/
void MK61_AStack_pop(void)
{
    X = Y;
    Y = Z;
    Z = T;
}


/*************************************************************************
Name: MK61_GetPC

Input:    none

Returns:  none
*************************************************************************/
unsigned char* MK61_GetPC(void)
{
    return &MK61.prg[PC];
}



/*************************************************************************
Name: ExecRS232Stop

Input:    none

Returns:  none

    ��������� ���������� ��� ����� �������� �� ���������
*************************************************************************/
void ExecRS232Stop(void)
{
    register short int DT = ((short int) MK61.prg[PC])<<8;
    DT |= (short int) PC;

    MK61.VM61.FLAGS.SERVEXEC   = 0;
    MK61.VM61.FLAGS.STEPBYSTEP = 0;
    MK61.VM61.FLAGS.STOP = 1;

    __sprintf_P(&dump[0],&aStopLine[0],DT);
    putsram_UART(&dump[0]);
    OS.FLAGS.STDOUT = 0; // ����������� STDOUT ����� ��� ������������ ������
}


/*************************************************************************
Name: OS_STDIN_BREAK

Input:    none

Returns:  none

    CallBack �� ����� � RS232 ������� Break
*************************************************************************/
void OS_STDIN_BREAK(void)
{
    CmdHook = NULL;
    ExecRS232Stop();
}



/*************************************************************************
Name: OS_STDIN_OVERFLOW

Input:    none

Returns:  none

    CallBack �� ������������ ������ � RS232
*************************************************************************/
void OS_STDIN_OVERFLOW(void)
{
}



/*************************************************************************
Name: VM61_GetOPCODE

Input:    unsigned char byte
          unsigned char op2

Returns:  none

    ��������� ����� � � ����������� �� ������ ������ - ���� ���������,
    ���� ��������� � ������ �����
*************************************************************************/
void VM61_GetOPCODE(unsigned char byte, unsigned char op2)
{
    OPCODE = byte;

    if(MK61.VM61.FLAGS.MODE)        // ����� ��������������� ������� ��������� ����� � ������
    {
        MK61.prg[PC++] = byte;
        MK61.VM61.FLAGS.DISP = 1;   // ��������� ��������� � ������ ���������������� �������� ��������� ���������
    }
    else if(!op2)
    {
        VM61_EXEC();
    }
}


/*************************************************************************
Name: VM61_GetOPCODE

Input:    unsigned char byte

Returns:  none

    ��������� ������� � � ����������� �� ������ ������ - ���� ���������,
    ���� ��������� � ������ �����
*************************************************************************/
void VM61_GetOPERAND(unsigned char byte)
{
    TEMP   = byte;
    if(MK61.VM61.FLAGS.MODE)        // ����� ��������������� ������� ��������� ������� � ������
    {
        MK61.prg[PC++] = byte;
        MK61.VM61.FLAGS.DISP = 1; // ��������� ��������� � ������ ���������������� �������� ��������� ���������
    }
    else
    {
        VM61_EXEC();
    }
}


/*************************************************************************
Name: VM61_EXEC

Input:    none

Returns:  none

    ��������� ���������� VM61 ������ OPCODE � �������� ������ TEMP
*************************************************************************/
void VM61_EXEC(void)
{
    register mk61call *ADDR;
    register unsigned char nReg;
    register unsigned long int VAL;

    // ---- ���������� � ����� � VM61  -----
    ADDR =  (mk61call*) ((unsigned int) JMP_TABLE + (unsigned int) OPCODE);

/*
    ���� ���������� ����� �� ������ � ������ ����� � "����� �������" ����� ��������� �������������� �� dispX -> X
     ������������ ������������ �����
        �) ��� ���������� ��������� ����� � ���������� ����� �� 9 ������ �� �� ��������� ��� ����������
          �������� � ������� ������� � ������������� ����� �� 9 ��������, ��� ����� ���� ��������������
          ���� �������!
 */
    if((OPCODE > 0x0A)&&(!MK61.VM61.FLAGS.CLOSEX))
    {
        X = strtod(&disp[0],NULL);
    }

    // ������������� ���������� ��������� � �������� ������
    if(OPCODE&0x80)
    {
        nReg = OPCODE & 0x0F;                               // �������� ����� ��������
        VAL = (unsigned long int) trunc(MK61.reg[nReg]);    // ������� FP

        // ���� ������� �� 0..3 �� ������������, ���� 4..6 �� �������������
        if(nReg < 4)
        {
            VAL--;
        }
        else if(nReg < 7)
        {
            VAL++;
        }

        TEMP = (unsigned char) VAL;     // �������� ��� VM61 �������� �������� ����� ������� � ���
        MK61.reg[nReg] = (double) VAL;

        //!!!!! �������������  ��� �������� ��������� �������� ��� ��������� ��������!!!!!
    }

    ADDR();
 }



/*************************************************************************
Name: OutputLCD_m1

Input:    none

Returns:  none

 ���������� � ����� ����������� ������ �� LCD � ������ ���������������� ��61

 ���������� ������:
 ==================

    ��������� ��������� �� ��� ��������� PC ����������� ����������������� ���� �� 3 �������
    ����������� � ������ �������� �� PC ����������� � ������� ������ ������� ������������ ������ PC
    => MK61.prg[PC] MK61.prg[PC-1] MK61.prg[PC-2]  PC

    0123456789A
    AA BB CC DD

*************************************************************************/
void OutputLCD_m1(void)
{
    register char *pBuff = &dispprg[0];
    register unsigned char *pPRG  = &MK61.prg[PC];
    register uWORD a;
    a.s.lo = *pPRG--;
    a.s.hi = *pPRG--;

    __sprintf_P(pBuff, aProgramLine, a, (int) ((PC<<8)|*pPRG));

/* TODO
    pBuff += 8;
    if(PC < 2) {*--pBuff = ':'; *--pBuff = ':';}
    pBuff--;
    if(PC < 1) {*--pBuff = ':'; *--pBuff = ':';}
    pBuff -= 2;
    if(PC == 0) {*--pBuff = ':'; *--pBuff = ':';}
*/

    // Clear display and home cursor
    lcd44780_clrscr();
    lcd44780_gotoxy(0, 0);

    lcd44780_puts(dispprg);
}


/*************************************************************************
Name: OutputLCD_m0

Input:    none

Returns:  none

 ���������� � ����� ����������� ������ �� LCD � ������ �������������� ������ ��61

 ���������� ������:
 ==================
   ��� ������ ����������� ��������� ����� �������� � ����:
        8 �������� �������� + ����� (���� ��� ����), ����� ����������� double � ������,
    ������� dtostrf ��������� ���������� ������� �����, �� ���� ��������
    ����� ���� ������ ������� dtostrf ��������� ����� ���-�� �������� �� �������, ������� ������
    ���������� �������� ������������� �� 9 �������� �������. ���� ��������� ��������� ������� � ����� �����-
    ���������� �������� ������� � �� ���� ���������.

    P.S. ����� ���������� ������� �������������� double ��� �������� � ���� ������� dtostrf :)
*************************************************************************/
void OutputLCD_m0(void)
{
    // TODO register signed char tmp;
    // TODO    register char *ptmp;
    register char *pdisp;

    pdisp= &disp[0];

    // Clear display and home cursor
    lcd44780_clrscr();
    lcd44780_home();
    lcd44780_puts(pdisp);
}


/*************************************************************************
Name: MK61_displayed

Input:    none

Returns:  none
*************************************************************************/
void MK61_displayed(void)
{
    if(MK61.VM61.FLAGS.REFRESH)
    {
        // <<<<<<<<<<  ��������� ���������� ���.������  >>>>>>>>>>
        asm("push r16");
        dtostrf(X,LIMIT_NUMERIC,LIMIT_NUMERIC-1,&disp[0]);  // �������� ������ R16
        asm("pop r16");

        MK61.VM61.FLAGS.DISP    = 1;
        MK61.VM61.FLAGS.REFRESH = 0;
    }

    if(MK61.VM61.FLAGS.DISP)
    {
        if(MK61.VM61.FLAGS.MODE)
        {
            OutputLCD_m1();
        }
        else
        {
            OutputLCD_m0();
        }

        MK61.VM61.FLAGS.DISP    = 0;
    }
}


/*************************************************************************
Name: main

Input:    none

Returns:  int

   >>>>>>>>>>>>>>>>> Main function <<<<<<<<<<<<<<<<<<
*************************************************************************/
int main(void)
{
    asm("cli");

    InitHAL();              // ������������� ���������� �����
    InitVM61();             // ������������� ����������� ������ MK61
    ExecRS232Stop();
    MK61_displayed();

/*
 =============================================================
   �������� ���� ������ ����������� ������ VM61
     VM61 - ��������� ��������� ������
 =============================================================
*/
    while(1)
    {
        asm("cli");

        if(OS.FLAGS.QUANT)              // ������� �� ������ �������
        {
            TIME_Schedule();
            OS.FLAGS.QUANT = 0;
        }

        if(MK61.VM61.FLAGS.STOP | MK61.VM61.FLAGS.DETAILED)
        {
            // ���������� �� ����������� ��������� - ���� VM61 �����������, ���� �������� �����������
            MK61_displayed();
        }

#ifdef MK61_KEYPAD_IN_USE
        if(OS.FLAGS.KEYBRD)
        {
            ScanDC_PROCESS();
            OS.FLAGS.STDOUT   = 0; // ����������� STDOUT ����� ��� ������������ ������
            Keyboard_Scan(); // ������� ������������� �������, ��������� ������������ ����.�������
        }
#endif // #ifdef MK61_KEYPAD_IN_USE

        // ���� ���������� ������ ������������, �������� ������� � ������ UART � �������� ��
        if(MK61.VM61.FLAGS.STOP)
        {
            // �������� ������� ������� � ����� ������ � RS232 ����������
            if(RxExtractLine(&COMMAND_BUFFER[0]))
            {
                // ��� ������� ������� � ������ ��������� ��,
                // ���� ������������� ������ �� NULL �� ������������� VM61
                if(ExecuteShell(&COMMAND_BUFFER[0]))
                {
                    ExecRS232Stop();
                }
            }
        }

        asm("sei");

        if(MK61.VM61.FLAGS.STOP)
        {
            asm("sleep");  // ���� VM ����������� �� ����
        }
        else
        {
            asm("cli");
            // <<<<<<<<<<<<   ������ �������� ����������� ������   >>>>>>>>
            OPCODE = MK61.prg[PC++];
            TEMP = MK61.prg[PC];     // �������� ����� �������� ��� ������ ��������
            VM61_EXEC();

            if(MK61.VM61.FLAGS.STEPBYSTEP)      // ���������� ��������� �����
            {
                ExecRS232Stop();
            }
        }
   }
}




/*
 ����� �������� ����� � ������ X ��� ����� �����
 1. ����� ����������� ��������:
   push AStack
   clear dispX
 2. ����� �������� ���������� VM �/�, �/� ....
   push AStack
   clear dispX
 3. ����� �������� �� ������ Bx, XY, loop
   clear dispX

����� ���������� ����������� ������� VM61
###########################
  ASTACK_UP - push AStack
###########################
  ������� ����� ��������� ������ ��� �������� ����� �����, �
  ������ ��� ����� ���� � ����������� �����������, ���� ��������
  ��������������� ����� ����� �����, ��� ��� ���� � ��� ��������
  ������������ ���������� �������
###########################
  CLOSEX   - clear dispX
###########################
  ������� ����� �������� ��� ������������ ���������� ������� ��������
  �������� X ���� ������������� �� ��� ����������� �������� dispX, �
  ��� ���������� �������� ����� ����� (0..9 � .) ����� dispX �������
  ��������������� ���������.
  ����� �� ����� ���� ������ �� ����������� ���� �������� ��� ��� �
  ���������� �������� �������� ���������� �������� � ������� dispX ��
  ���������� ������ ��� ��������� VM.
###########################
  REFRESH
###########################
  ������� ����� �������� ��� � ��������� X ���� ��������� ��������
  ��������� ��� ����������� ��� � ���������� ������ LED_BUFFER,
  ��� � � ������ ����������� ������������� dispX.

!!!!! ������� �� ������� �� ��� ����������� ������� VM61 ������ !!!!

 ���� ���� CLOSEX ������
   ��� �������� ��� X ��� ������������
   � ����� ����������� �������� X - dispX ��������� � ������� ��� �������
   ������ ��������� ������.
 �� �������������� �� ����������� ������������� dispX � double X
   ����������� �� ���������!!!!

*/


/*************************************************************************
Name: code_NOP

Input:    none

Returns:  none
*************************************************************************/
void code_NOP(void)
{
    asm("nop");
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_00_to_09

Input:    none

Returns:  none

    ���.���� �� 1 �� 9 (����� ����� �����)
*************************************************************************/
void code_00_to_09(void)
{
    // ���������� ����������� ���� ����� ������� ������ �����
    if(MK61.VM61.FLAGS.ASTACK_UP)
    {
        MK61_AStack_push();
    }

    if(MK61.VM61.FLAGS.CLOSEX)
    {
        // ���� ����� � ����� ��������, ������� ������
        memset(&disp[0],0,16);
        pPutX = &disp[0];
    }

    // ����� ���� �� ������� �����  8 - �������� ��������
    //  ��� �� �� ����������� � ����������� ������� �� �������� �������� �������� �� 9, ���� �����
    // � ������������� �� 8 ��������� ������ ������ ����� ������������� ���������� �����
    if(pPutX < &disp[LIMIT_NUMERIC])
    {
        *pPutX++ = OPCODE + '0';
    }

    MK61.VM61.FLAGS.ASTACK_UP = 0;
    MK61.VM61.FLAGS.CLOSEX    = 0;
    MK61.VM61.FLAGS.DISP      = 1;
}


/*************************************************************************
Name: code_0A

Input:    none

Returns:  none

    ���.��� ���������� ����������� (���� �����)
*************************************************************************/
void code_0A(void)
{
    // ����� ���� �� ������� �����  8 - �������� ��������
    //  ��� �� �� ����������� � ����������� ������� �� �������� �������� �������� �� 9, ���� �����
    // � ������������� �� 8 ��������� ������ ������ ����� ������������� ���������� �����
    if(pPutX < &disp[LIMIT_NUMERIC])
    {
        *pPutX++ = '.';
    }

    MK61.VM61.FLAGS.DISP = 1;
}


/*************************************************************************
Name: code_0B

Input:    none

Returns:  none

    ���.��� ����� ����� ����� /-/
*************************************************************************/
void code_0B(void)
{
    X = -X;

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
}


/*************************************************************************
Name: code_0E

Input:    none

Returns:  none

    ���.��� �������� ����� ����� (� ������� �)
*************************************************************************/
void code_0E(void)
{
    MK61_AStack_push();
    MK61.VM61.FLAGS.CLOSEX = 1;
}


/*************************************************************************
Name: code_0F

Input:    none

Returns:  none

    ���.��� ������ ����� ����� ������� � X1
*************************************************************************/
void code_0F(void)
{
    MK61_AStack_push();
    X = X1;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_0D

Input:    none

Returns:  none

    ���.��� Cx ������� X
*************************************************************************/
void code_0D(void)
{
    X = 0.00;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
}


/*************************************************************************
Name: code_10_13

Input:    none

Returns:  none

    ���.��� �������������� �������� +,-,*,/
*************************************************************************/
void code_10_13(void)
{
    X1 = X;
    switch(OPCODE)
    {
        case 0x10: Y += X1; break;
        case 0x11: Y -= X1; break;
        case 0x12: Y *= X1; break;
        case 0x13: Y /= X1;
    }

    MK61_AStack_pop();
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX  = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_14

Input:    none

Returns:  none

    ���.��� ����� XY
*************************************************************************/
void code_14(void)
{
    X1 = X;
    X  = Y;
    Y  = X1;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_15

Input:    none

Returns:  none

    ���.��� 10^X
*************************************************************************/
void code_15(void)
{
    X1 = X;
    X  = pow(10,X);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_16

Input:    none

Returns:  none

    ���.��� e^X
*************************************************************************/
void code_16(void)
{
    X1 = X;
    X  = pow(2.718281,X);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_17

Input:    none

Returns:  none

    ���.��� Lg(X)
*************************************************************************/
void code_17(void)
{
    X1 = X;
    X  = log10(X);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_18

Input:    none

Returns:  none

    ���.��� Ln(X)
*************************************************************************/
void code_18(void)
{
    X1 = X;
    X  = log(X);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_19

Input:    none

Returns:  none

    ���.��� Arcsin(X)
*************************************************************************/
void code_19(void)
{
    X1 = X;
    X  = to_R_GR_G(asin(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_1A

Input:    none

Returns:  none

    ���.��� Arccos(X)
*************************************************************************/
void code_1A(void)
{
    X1 = X;
    X  = to_R_GR_G(acos(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_1B

Input:    none

Returns:  none

    ���.��� Arctg(X)
*************************************************************************/
void code_1B(void)
{
    X1 = X;
    X  = to_R_GR_G(atan(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_1C

Input:    none

Returns:  none

    ���.��� Sin(X)
*************************************************************************/
void code_1C(void)
{
    X1 = X;
    X  = sin(from_R_GR_G(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_1D

Input:    none

Returns:  none

    ���.��� Cos(X)
*************************************************************************/
void code_1D(void)
{
    X1 = X;
    X  = cos(from_R_GR_G(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_1E

Input:    none

Returns:  none

    ���.��� Tg(X)
*************************************************************************/
void code_1E(void)
{
    X1 = X;
    X  = tan(from_R_GR_G(X));
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_20

Input:    none

Returns:  none

    ���.��� �������� ���������� ��������� - ����� Pi
*************************************************************************/
void code_20(void)
{
    // X1  = X;
    MK61_AStack_push();
    X   = 3.1415926;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_21

Input:    none

Returns:  none

    ���.��� ����������� �������� SQRT(X)
*************************************************************************/
void code_21(void)
{
    X1  = X;
    X   = sqrt(X);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_22

Input:    none

Returns:  none

    ���.��� ����������� �������� X^2
*************************************************************************/
void code_22(void)
{
    X1  = X;
    X   = square(X1);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_23

Input:    none

Returns:  none

    ���.��� ����������� �������� 1/X
*************************************************************************/
void code_23(void)
{
    X1  = X;
    X   = 1/X;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}



/*************************************************************************
Name: code_24

Input:    none

Returns:  none

    ���.��� ���������� �������� X^Y
*************************************************************************/
void code_24(void)
{
    X1  = X;
    X   = pow(X, Y);
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_25

Input:    none

Returns:  none

    ���.��� ���������� �������� ����� � �������� �����������
*************************************************************************/
void code_25(void)
{
    X1  = X;
    MK61_AStack_pop();
    T   = X1;
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}



/*************************************************************************
Name: code_31

Input:    none

Returns:  none

    ���.���  ������ �����  |X|
*************************************************************************/
void code_31(void)
{
    X = fabs(X);

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
}


/*************************************************************************
Name: code_40_4E

Input:    none

Returns:  none

    ���.��� ������ � ������ �0...�E
*************************************************************************/
void code_40_4E(void)
{
    MK61.reg[OPCODE - 0x40] = X;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_50

Input:    none

Returns:  none

    ���.��� �/�
*************************************************************************/
void code_50(void)
{
    //X_rs232(0);
    //if(MK61.VM61.FLAGS.STOP)
    //{
    //    MK61.VM61.FLAGS.STOP = 0;
    //}
    //return;

    ExecRS232Stop();

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.STOP = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;

    MK61_displayed();
}


/*************************************************************************
Name: code_51

Input:    none

Returns:  none

    ���.��� ��
*************************************************************************/
void code_51(void)
{
    PC = TEMP;
    // ���� ������� �������������� � ������ �������
    // ������-��������� (������������ ������� =)

    if(MK61.VM61.FLAGS.SERVEXEC)
    {
        ExecRS232Stop();
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_52

Input:    none

Returns:  none

    ���.��� �/0 ������� �� ������������
*************************************************************************/
void code_52(void)
{
    //if(MK61.VM61.FLAGS.STOP)
    // PC = 0;                // � ������ �������� �/� - ������������� ��� = 00
    //else{

    MK61_CStack_pop();     // � ������ ���������� �/� - ������� �� ��
    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
    //}
}


/*************************************************************************
Name: code_53

Input:    none

Returns:  none

    ���.��� �� ������� �� ������������
*************************************************************************/
void code_53(void)
{
    MK61_CStack_push();
    PC = TEMP;

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_57_59_5C_5E

Input:    none

Returns:  none

    ���.��� ��������� ��������
*************************************************************************/
void code_57_59_5C_5E(void)
{
    ALU61.FLAGS.Z = 0;
    ALU61.FLAGS.S = 0;

    if(X==0)
    {
        ALU61.FLAGS.Z = 1;
    }

    if(signbit(X))
    {
        ALU61.FLAGS.S = 1;
    }

    PC++;

    switch(OPCODE)
    {
        case 0x57:                      // X != 0
            if(ALU61.FLAGS.Z)
            {
                PC = TEMP;
            }
            break;

        case 0x59:                      // X >= 0
            if(ALU61.FLAGS.S)
            {
                PC = TEMP;
            }
            break;

        case 0x5C:                      // X < 0
            if(!ALU61.FLAGS.S)
            {
                PC = TEMP;
            }
            break;

        default:                        // X == 0
            if(!ALU61.FLAGS.Z)
            {
                PC = TEMP;
            }
            break;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_58_5A_5B_5D

Input:    none

Returns:  none

    ���.��� �����
*************************************************************************/
void code_58_5A_5B_5D(void)
{
    register double cnt;
    PC++;

    // �������� � ����� �������� 58 - �2, 5A - �3, 5B - �1, 5D - �0
    OPCODE &= 0x03;
    OPCODE ^= 0x02;

    if(OPCODE == 0)
    {
        OPCODE = 3;
    }
    else if(OPCODE == 3)
    {
        OPCODE = 0;
    }

    cnt = MK61.reg[OPCODE] - 1;

    if(cnt > 0)
    {
        MK61.reg[OPCODE] = cnt;  // ������� ������ � �������� ���� ��� � ������� �������� �� �����
        PC = TEMP;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_60_6E

Input:    none

Returns:  none

    ���.��� ������ �� ������ ��0...��E
*************************************************************************/
void code_60_6E(void)
{
    //X1 = X;
    MK61_AStack_push();
    X = MK61.reg[OPCODE - 0x60];

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_70_7E

Input:    none

Returns:  none

    ���.��� ��������� �������� x!=0 �� ���������� ������ ���0...���E
*************************************************************************/
void code_70_7E(void)
{
    ALU61.FLAGS.Z = 0;

    if(X==0)
    {
        ALU61.FLAGS.Z = 1;
    }

    PC++;
    if(ALU61.FLAGS.Z)               // X != 0
    {
        PC = TEMP;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_80_8E

Input:    none

Returns:  none

    ���.��� ������������ �������� �� ���������� ������ ���0...���E
*************************************************************************/
void code_80_8E(void)
{
    PC = TEMP;

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_90_9E

Input:    none

Returns:  none

    ���.��� ��������� �������� x>=0 �� ���������� ������ ���0...���E
*************************************************************************/
void code_90_9E(void)
{
    ALU61.FLAGS.S = 0;

    if(signbit(X))
    {
        ALU61.FLAGS.S = 1;
    }

    PC++;
    if(ALU61.FLAGS.S)                   // X >= 0
    {
        PC = TEMP;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}



/*************************************************************************
Name: code_A0_AE

Input:    none

Returns:  none

    ���.��� ������ ������������ �� ���������� ������ ���0...���E
*************************************************************************/
void code_A0_AE(void)
{
    PC--;                   // ��������� ������� �� ����� �������� ������ ��������
    MK61_CStack_push();
    PC = TEMP;

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}



/*************************************************************************
Name: code_B0_BE

Input:    none

Returns:  none

    ��������� ������ � �������� ��0..���
*************************************************************************/
void code_B0_BE(void)
{
    uVector dt;
    if(TEMP > LAST_REGISTER)
    {
        dt.val = lrint(X);
    }
    else
    {
        MK61.reg[TEMP] = X;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}



/*************************************************************************
Name: code_C0_CE

Input:    none

Returns:  none

    ���.��� ��������� �������� x<0 �� ���������� ������ ���0...���E
*************************************************************************/
void code_C0_CE(void)
{
    ALU61.FLAGS.S = 0;

    if(signbit(X))
    {
        ALU61.FLAGS.S = 1;
    }

    PC++;
    if(!ALU61.FLAGS.S)              // X < 0
    {
        PC = TEMP;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_D0_DE

Input:    none

Returns:  none

    ���.��� ��������� ������ �� ��������� ���0..����
*************************************************************************/
void code_D0_DE(void)
{
    //X1 = X;
    MK61_AStack_push();

    if(TEMP > LAST_REGISTER)
    {
        TEMP = 0;
    }

    X = MK61.reg[TEMP];

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: code_E0_EE

Input:    none

Returns:  none

    ���.��� ��������� �������� x==0 �� ���������� ������ ���0...���E
*************************************************************************/
void code_E0_EE(void)
{
    ALU61.FLAGS.Z = 0;

    if(X==0)
    {
        ALU61.FLAGS.Z = 1;
    }

    PC++;
    if(!ALU61.FLAGS.Z)              // X == 0
    {
        PC = TEMP;
    }

    MK61.VM61.FLAGS.REFRESH = 1;
    MK61.VM61.FLAGS.CLOSEX = 1;
    MK61.VM61.FLAGS.ASTACK_UP = 1;
}


/*************************************************************************
Name: JMP_TABLE

Input:    none

Returns:  none
*************************************************************************/
void JMP_TABLE(void)
{
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_00_to_09");
    asm("rjmp code_0A");
    asm("rjmp code_0B");
    asm("rjmp code_NOP");
    asm("rjmp code_0D");
    asm("rjmp code_0E");
    asm("rjmp code_0F");
    asm("rjmp code_10_13");
    asm("rjmp code_10_13");
    asm("rjmp code_10_13");
    asm("rjmp code_10_13");
    asm("rjmp code_14");
    asm("rjmp code_15");
    asm("rjmp code_16");
    asm("rjmp code_17");
    asm("rjmp code_18");
    asm("rjmp code_19");
    asm("rjmp code_1A");
    asm("rjmp code_1B");
    asm("rjmp code_1C");
    asm("rjmp code_1D");
    asm("rjmp code_1E");
    asm("rjmp code_NOP");
    asm("rjmp code_20");
    asm("rjmp code_21");
    asm("rjmp code_22");
    asm("rjmp code_23");
    asm("rjmp code_24");
    asm("rjmp code_25");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_31");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_40_4E");
    asm("rjmp code_NOP");
    asm("rjmp code_50");
    asm("rjmp code_51");
    asm("rjmp code_52");
    asm("rjmp code_53");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_57_59_5C_5E");     // 57
    asm("rjmp code_58_5A_5B_5D");     // 58
    asm("rjmp code_57_59_5C_5E");     // 59
    asm("rjmp code_58_5A_5B_5D");     // 5A
    asm("rjmp code_58_5A_5B_5D");     // 5B
    asm("rjmp code_57_59_5C_5E");     // 5C
    asm("rjmp code_58_5A_5B_5D");     // 5D
    asm("rjmp code_57_59_5C_5E");     // 5E
    asm("rjmp code_NOP");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_60_6E");
    asm("rjmp code_NOP");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_70_7E");
    asm("rjmp code_NOP");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_80_8E");
    asm("rjmp code_NOP");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_90_9E");
    asm("rjmp code_NOP");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_A0_AE");
    asm("rjmp code_NOP");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_B0_BE");
    asm("rjmp code_NOP");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_C0_CE");
    asm("rjmp code_NOP");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_D0_DE");
    asm("rjmp code_NOP");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_E0_EE");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");
    asm("rjmp code_NOP");  // opcode = 0xFF
}


/*************************************************************************
Name: to_R_GR_G

Input:    double Angle

Returns:  double

    ������� ������� � ������������ ����� �-��-�
*************************************************************************/
double to_R_GR_G(double Angle)
{
    if(MK61.VM61.FLAGS.R_GRD_G == DEGREE)
    {
        return Angle*180/3.1415926;
    }
    else
    {
        return Angle;
    }
}


/*************************************************************************
Name: from_R_GR_G

Input:    double X

Returns:  double

    ������� ������� �� �������������� ������ �-��-�
*************************************************************************/
double from_R_GR_G(double Angle)
{
    if(MK61.VM61.FLAGS.R_GRD_G == DEGREE)
    {
        return (Angle/180)*3.1415926;
    }
    else
    {
        return Angle;
    }
}



