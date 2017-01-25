// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: terminal.c
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
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include <avr/pgmspace.h>
#include "terminal.h"
#include "mk61types.h"
#include "usart_module.h"
#include "extlib.h"
//#include "i2c.h"
//#include "store.h"

void* SeekCmd(char SymCmd);

const char aUnknownCmd[] PROGMEM = "\r?cmd?";

const cCommand shell[15] PROGMEM =
{
    {'t'<<1,Step_rs232},
    {'@'<<1,Regs_rs232},
    {'='<<1,Input_rs232},
    {'m'<<1,Mem_rs232},
    {'n'<<1,NextMem_rs232},
    {'g'<<1,Go_rs232},
    {'l'<<1,Load_rs232},
    {'c'<<1,Ctrl_rs232},
    {'p'<<1,Jmp_rs232},
    {'d'<<1,dump_rs232},
    {'x'<<1,X_rs232},
    {'s'<<1,AStack_rs232},
    {'r'<<1,CStack_rs232},
    {'f'<<1,SaveFlash},
    {('i'<<1)|0x01,LoadFlash}
};

// Для отладки по RS232
char dump[24+24+7];


/*************************************************************************
Name: Ctrl_rs232

Input:    signed char par

Returns:  unsigned char

    Управление VM61
*************************************************************************/
unsigned char Ctrl_rs232(signed char par)
{
    // register int t;
    if(par == 1)
    {
        MK61.VM61.FLAGS.R_GRD_G = RADIAN;
    }
    else if(par == 2)
    {
        MK61.VM61.FLAGS.R_GRD_G = DEGREE;
    }

/* else if(par == 3){
  t = ee24xx_check(DEV_STORE);
  if(t<0)
   __sprintf_P(&dump[0], aTWI_ERROR0, twst);
  else
   __sprintf_P(&dump[0], aTWI_CHKI2C, t);

  putsram_UART(&dump[0]);
 } */
    else
    {
        MK61.VM61.FLAGS.DETAILED = ~MK61.VM61.FLAGS.DETAILED;
    }

    return -1;
}


/*************************************************************************
Name: Load_rs232

Input:    signed char par

Returns:  unsigned char

    Загрузка программы из терминала по протоколу X-modem
*************************************************************************/
unsigned char Load_rs232(signed char par)
{
    return -1;
}


/*************************************************************************
Name: Regs_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char Regs_rs232(signed char par)
{
    register unsigned char tmp = 0;
    register double *pRg =  &MK61.reg[0];

    do
    {
        dump[0] = 'R';
        dump[1] = HEX(tmp>>4);
        dump[2] = HEX(tmp&0x0F);
        dump[3] = '=';
        asm("push r16");
        dtostrf(*pRg++,LIMIT_NUMERIC,LIMIT_NUMERIC-1,&dump[4]);  // Засранка гробит R16
        asm("pop r16");
        dump[4+LIMIT_NUMERIC] = '\r';
        dump[4+LIMIT_NUMERIC+1] = 0x00;

        putsram_UART(dump);
    } while(++tmp < 15);

    return -1;
}


/*************************************************************************
Name: CStack_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char CStack_rs232(signed char par)
{
    register unsigned char tmp;
    register char *pDump = (char*) &dump[0];
    register unsigned char *pSt =  &MK61.ret[0];

    while(pSt < &MK61.ret[6])
    {
        tmp = *pSt;
        *pDump++ = HEX(tmp>>4);
        *pDump++ = HEX(tmp &= 0x0F);
        *pDump++ = (pSt++ == MK61.pCStack)?  '<' : ' ';
    }

    *pDump = 0x00;
    putsram_UART(dump);

    return -1;
}


/*************************************************************************
Name: unknown_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char unknown_rs232(signed char par)
{
    putsrom_UART((char*) &aUnknownCmd[0]);

    return -1;
}


/*************************************************************************
Name: dump_rs232

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char dump_rs232(signed char par)
{
    register unsigned char tmp;
    register char *pDump;
    register unsigned char *pDumpPRG = &MK61.prg[0];

    while(pDumpPRG < &MK61.prg[255])
    {
        pDump = (char*) &dump[0];
        do
        {
            tmp  = *pDumpPRG;
            *pDump++ = HEX(tmp>>4);
            *pDump++ = HEX(tmp&=0x0F);
            *pDump++ = (pDumpPRG++ == MK61_GetPC())?  '<' : ' ';
        } while(pDump < &dump[48]);

        *pDump++ = '\r';
        *pDump++ = 0x00;
        putsram_UART(dump);
    }

    return -1;
}


/*************************************************************************
Name: SaveFlash

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char SaveFlash(signed char par)
{
/*    if(par >= 0)
    {
        int ret = ee24xx_write_bytes(STORE_DEVICE, par<<8, 256, (unsigned char*) &MK61.prg[0]);
#ifdef DEBUG
        EEPROM_PutErrorCode(ret);
#endif
    }
    else*/
    {
        // Сохраним программу МК61 в EEPROM
        BlockWrEEPROM((unsigned char*) EEPROM_MK61PRG,(unsigned char*) &MK61.prg[0], 105);

        // Сохраним регистры памяти в EEPROM
        BlockWrEEPROM((unsigned char*) EEPROM_MK61PRG+105,(unsigned char*) &MK61.reg[0], sizeof(MK61.reg));
    }

    return -1;
}


/*************************************************************************
Name: LoadFlash

Input:    signed char par

Returns:  unsigned char
*************************************************************************/
unsigned char LoadFlash(signed char par)
{
/*    if(par >= 0)
    {
        STORE_fread(par);
    }
    else*/
    {
        // Считаем программу МК61 в EEPROM
        BlockRdEEPROM((unsigned char*) EEPROM_MK61PRG,(unsigned char*) &MK61.prg[0], 105);

        // Считаем регистры памяти в EEPROM
        BlockRdEEPROM((unsigned char*) EEPROM_MK61PRG+105,(unsigned char*) &MK61.reg[0], sizeof(double[15]));
    }

    return -1;
}


/*************************************************************************
Name: SeekCmd

Input:    char SymCmd

Returns:  void*
*************************************************************************/
void* SeekCmd(char SymCmd)
{
    register void *pCmd;
    asm volatile(
        "clc"                                         "\n\t"
        "loop%=:"                                     "\n\t"
        "brcs unknown_cmd%="                          "\n\t"
        "adiw r30, %[size]"                           "\n\t"
        "lpm  __tmp_reg__, Z+"                        "\n\t"
        "lsr  __tmp_reg__"                            "\n\t"
        "eor  __tmp_reg__, %[rx]"                     "\n\t"
        "brne loop%="                                 "\n\t"
        "lpm %A[pcmd], Z+"                            "\n\t"
        "lpm %B[pcmd], Z"                             "\n\t"
        "rjmp exit%="                                 "\n\t"
        "unknown_cmd%=:"                              "\n\t"
        "ldi %A[pcmd], lo8(pm(unknown_rs232))"        "\n\t"
        "ldi %B[pcmd], hi8(pm(unknown_rs232))"        "\n\t"
        "exit%=:"                                     "\n\t"
        : [pcmd] "=r" (pCmd)
        : [rx]   "r"  (SymCmd),
        [cmd]  "z"  (((int)&shell[0])-sizeof(cCommand)+1),
        [size] "I"  (sizeof(cCommand)-1)
    );

    return pCmd;
}


/*************************************************************************
Name: ExecuteShell

Input:    unsigned char *pCmd

Returns:  void*

    Исполнить команду pCmd в интерпритаторе команд терминала
*************************************************************************/
unsigned char ExecuteShell(unsigned char *pCmd)
{
    register unsigned char par;
    register shellcall *CmdCall;

    if(CmdHook)
    {
        CmdCall = CmdHook;
    }
    else
    {
        CmdCall = (shellcall*) SeekCmd(*pCmd++); // Обработчик команды из стурктуры shell
    }

    par = -1;   // Если параметр не задан в командной строке значит он равен -1
    if(*pCmd)
    {
        par = atoc(pCmd);
    }

    return CmdCall(par);
}







