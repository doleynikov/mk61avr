// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: scan_dc.c
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
#include "scancode_dc.h"
#include "keyboard.h"
#include "mk61types.h"
#include "extlib.h"
#include "usart_module.h"

unsigned char OPC;
signed char *pScanDC;
dccall *scandc_proc;

#ifdef DEBUG_SCAN_DECODER
 const char aUnknownScan[] PROGMEM = "\rScanDC ?";
 const char aScanDC_mod1[] PROGMEM = "\rMOD1 val=%x OPC=%x";
 const char aScanInf[]     PROGMEM = "\rpDC=%x%x TAB=%x%x Scan=%x CTRL=%x";
#endif

const servcall *SERVICE_PROC[] PROGMEM =
{
    (const servcall*) &MK61_FrwdStep,
    (const servcall*) &MK61_BackStep,
    (const servcall*) &MK61_Trace,
    (const servcall*) &MK61_ToAuto,
    (const servcall*) &MK61_ToProg,
    (const servcall*) &MK61_FrmtStore,
    (const servcall*) &MK61_SaveStore,
    (const servcall*) &MK61_LoadStore,
    (const servcall*) &MK61_Start,
    (const servcall*) &MK61_Stop,
    (const servcall*) &MK61_GoTop
};

const signed char aScanDecoder_NUM[] PROGMEM =
{
//
// + 0 ScanCode.Row == 0x40
//
/*  9   */  0x09,
/*  0   */  0x00,
/*  1   */  0x01,
/*  2   */  0x02,
/*  4   */  0x04,
/*  5   */  0x05,
/*  6   */  0x06,
/*  7   */  0x07,
/*  8   */  0x08,
/*  3   */  0x03,
//
// + 10 ScanCode.Row == 0x80
//
/*  B^  */  0x0E,
/*  +   */  0x00,
/*  -   */  0x01,
/*  X   */  0x02,
/* X<->Y*/  0x04,
/*  .   */  0x0A,
/* |-|  */  0x0B,
/* ВП   */  0x0C,
/* Cx   */  0x0D,
/* -:-  */  0x03
};


/*
 Таблица перекодировки скан-кодов и в опкоды MK61
  SERVICE - режим управления mk61avr (код управления смещен на +5)
  SHIFT   - режим смещения кодовой страницы на N байт (смещение N  на +5)
  ALTER   - режим альтернативного смещения кодовой страницы (базовое смещение OFFSET на +5)
  MOD_1   - режим модификации однобайтного опкода (модицифицируемый код смещен на +5)
  MOD_2   - режим модификации двухбайтного опкода (первый байт опкода смещен на +5)
  В первых 4 колонках могут идти только ОПКОДЫ VM61 или КОМАНДЫ УПРАЛВЕНИЯ ДЕКОДЕРОМ СКАН КОДОВ
  в колонках начиная с 5 идут либо опкоды, либо данные команд управления декодером скан-кодов.
*/
const unsigned char aScanDecoder[] PROGMEM =
{
// + 0 ScanCode.Row == 0x10
//            -      шифт F   шифт K    шифт F+K
//
/* F    */  SHIFT,   SHIFT,   SHIFT,    UNKNOWN,    +1,          0,          0xFF,
/* С/П  */  ALTER,   MOD_2,   MOD_1,    UNKNOWN,    +0,          op_IFNE,    op_IFNE_R,
/* БП   */  MOD_2,   MOD_2,   MOD_1,    UNKNOWN,    op_JMP,      op_FOR2,    op_JMP_R,
/* В/О  */  ALTER,   MOD_2,   MOD_1,    UNKNOWN,    +21,         op_IFGE,    op_IFGE_R,
/* X->П */  MOD_1,   MOD_2,   MOD_1,    MK_SAVE,    op_mov_X_R,  op_FOR1,    op_ldx_MR,
/* ШГ-> */  SERVICE, MOD_2,   MOD_1,    UNKNOWN,    MK_FRWD,     op_IFLT,    op_IFLT_R,
/* П->X */  MOD_1,   MOD_2,   MOD_1,    MK_LOAD,    op_mov_R_X,  op_FOR0,    op_stx_MR,
/* <-ШГ */  SERVICE, MOD_2,   MOD_1,    UNKNOWN,    MK_BACK,     op_IFEQ,    op_IFEQ_R,
/* K    */  SHIFT,   SHIFT,   SHIFT,    UNKNOWN,    +2,          +2,         0xFE,
/* ПП   */  ALTER,   MOD_2,   MOD_1,    UNKNOWN,    +42,         op_FOR3,    op_SUB_R,
//
// + 70 ScanCode.Row == 0x20
//            -      шифт F   шифт K    шифт F+K
//
/*  9   */  op_9,    op_Tg,   op_Max,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  0   */  op_0,    op_10x,  op_Nop,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  1   */  op_1,    op_ex,   op_K1,    UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  2   */  op_2,    op_lg,   op_K2,    UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  4   */  op_4,    op_asin, op_modx,  UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  5   */  op_5,    op_acos, op_sgn,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  6   */  op_6,    op_atg,  op_msg,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  7   */  op_7,    op_sin,  op_int,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  8   */  op_8,    op_cos,  op_frac,  UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  3   */  op_3,    op_ln,   op_gms,   UNKNOWN,    UNKNOWN,     UNKNOWN,    UNKNOWN,
//
// + 140 ScanCode.Row == 0x40
//            -      шифт F   шифт K   шифт F+K
//
/*  B^  */  op_Bl,   op_Bx,   op_rnd,  UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  +   */  op_Add,  op_Pi,   op_gm,   UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  -   */  op_Sub,  op_Sqrt, op_Ks,   UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  X   */  op_Mul,  op_Sqr,  op_Km,   UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/* X<->Y*/  op_XY,   op_Xy,   op_gs,   UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/*  .   */  op_cma,  op_Cyc,  op_and,  UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/* |-|  */  op_csg,  SERVICE, op_or,   UNKNOWN,     UNKNOWN,     MK_AUTO,    UNKNOWN,
/* ВП   */  op_E,    SERVICE, op_xor,  UNKNOWN,     UNKNOWN,     MK_PROG,    UNKNOWN,
/* Cx   */  op_Cx,   SHIFT,   op_not,  MK_FORMAT,   UNKNOWN,        0xFF,    UNKNOWN,
/* -:-  */  op_Div,  op_1dx,  op_Kd,   UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
//
// Таблица альтернатив + 210
//
// + 0  для C/П
/* AUTO */  SERVICE, UNKNOWN, UNKNOWN, UNKNOWN,     MK_START,    UNKNOWN,    UNKNOWN,
/* PROG */  op_STOP, UNKNOWN, UNKNOWN, UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/* RUN  */  SERVICE, UNKNOWN, UNKNOWN, UNKNOWN,     MK_STOP,     UNKNOWN,    UNKNOWN,
// + 21 для В/О
/* AUTO */  SERVICE, UNKNOWN, UNKNOWN, UNKNOWN,     MK_GOTOP,    UNKNOWN,    UNKNOWN,
/* PROG */  op_RET,  UNKNOWN, UNKNOWN, UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/* RUN  */  UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
// + 42 для ПП
/* AUTO */  SERVICE, UNKNOWN, UNKNOWN, UNKNOWN,     MK_TRACE,    UNKNOWN,    UNKNOWN,
/* PROG */  op_SUB,  UNKNOWN, UNKNOWN, UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
/* RUN  */  UNKNOWN, UNKNOWN, UNKNOWN, UNKNOWN,     UNKNOWN,     UNKNOWN,    UNKNOWN,
};



/*************************************************************************
Name: ScanDC_RESET

Input:    none

Returns:  none

    Сброс декодера скан-последовательностей
*************************************************************************/
void ScanDC_RESET(void)
{
    pScanDC = (signed char*) &aScanDecoder[0]; // укзатель таблицы декодировки в начальное положение
    scandc_proc = &ScanDC_common_alg;           // Указатель обработчика в основной алгоритм
}



/*************************************************************************
Name: ScanDC_PROCESS

Input:    none

Returns:  none

    Обработка скан-кода декодером скан-последовательностей
*************************************************************************/
void ScanDC_PROCESS(void)
{
    register unsigned char *pDC = (unsigned char*) (&aScanDecoder_NUM[0] + (ScanCode.scan.code-1));

    if(ScanCode.scan.line2)
    {
        pDC += 10;
    }

    scandc_proc(__LPM(pDC));
}


/*************************************************************************
Name: ScanDC_mod21

Input:    unsigned char val

Returns:  none

    Обработчик скан кода как модификатор OPERAND|NUM
*************************************************************************/
void ScanDC_mod21(unsigned char val)
{
    VM61_GetOPERAND(OPC|val);   // декодер готов выпустить ОПКОД VM61
    ScanDC_RESET();             // мавр сделал свое дело - вернем атовмат наместо
}


/*************************************************************************
Name: ScanDC_mod20

Input:    unsigned char val

Returns:  none

    Обработчик скан кода как модификатор OPERAND = NUM<<4
*************************************************************************/
void ScanDC_mod20(unsigned char val)
{
    OPC =  val<<4;
    scandc_proc = &ScanDC_mod21;
}


/*************************************************************************
Name: ScanDC_mod1

Input:    unsigned char val

Returns:  none

    Обработчик скан кода как модификатор OPCODE|NUM
*************************************************************************/
void ScanDC_mod1(unsigned char val)
{
#ifdef DEBUG_SCAN_DECODER
    __sprintf_P(&dump[0], &aScanDC_mod1[0], (unsigned int) OPC<<8|val);
    putsram_UART(&dump[0]);
#endif
    VM61_GetOPCODE(OPC|val, 0);     // декодер готов выпустить ОПКОД VM61
    ScanDC_RESET();                 // мавр сделал свое дело - вернем атовмат наместо
}


/*************************************************************************
Name: ScanDC_common_alg

Input:    unsigned char val

Returns:  none
*************************************************************************/
void ScanDC_common_alg(unsigned char val)
{
    register signed char *pDC = pScanDC;
    register unsigned char DCCtrl = ((unsigned char)(ScanCode.scan.code-1))*7;

    if(ScanCode.scan.line1)
    {
        DCCtrl += 70;
    }

    if(ScanCode.scan.line2)
    {
        DCCtrl += 140;
    }

    pDC += (unsigned char) DCCtrl;

    DCCtrl = __LPM(pDC);

    if(DCCtrl >= UNKNOWN)               // Ошибка декодирования
    {
        ScanDC_RESET();
#ifdef DEBUG_SCAN_DECODER
        putsrom_UART((char*) &aUnknownScan[0]);
#endif
        return;
    }

#ifdef DEBUG_SCAN_DECODER
    __sprintf_P(&dump[0],&aScanInf[0], pDC, pScanDC, (unsigned int) DCCtrl<<8|ScanCode.val);
    putsram_UART((char*) &dump[0]);
#endif

    // Работа декодера по исполнению команд управления декодером организующих сдвиг в странице
    if((DCCtrl==ALTER)||(DCCtrl==SHIFT))
    {
        pDC = pScanDC + (signed char) __LPM(pDC+4); // сдвинем указатель в странице перекодировки
#ifdef DEBUG_SCAN_DECODER
        __sprintf_P(&dump[0],&aScanInf[0], pDC, pScanDC, (unsigned int) DCCtrl<<8|ScanCode.val);
        putsram_UART((char*) &dump[0]);
#endif

        if(DCCtrl==ALTER)
        {
            // Если ALTER дополнительно сдвинем указатель в соответствии с альтернативной
            pDC += 210;
            if(MK61.VM61.FLAGS.MODE)
            {
                pDC += 7;
            }

            if(!MK61.VM61.FLAGS.STOP)
            {
                pDC += 14;
            }

            DCCtrl = __LPM(pDC);
        }
        else
        {
            // Если шифт то сохраним новый указатель на таблицу перекодировки и уйдем до след. нажатия
            pScanDC = pDC;
            return;
        }
    }

    // Работа декодера по исполнению команд управления аппаратным комплексом или выпуском ОПКОДА VM61
    OPC = __LPM(pDC+4);

#ifdef DEBUG_SCAN_DECODER
    __sprintf_P(&dump[0],&aScanInf[0], pDC, pScanDC, (unsigned int) DCCtrl<<8|ScanCode.val);
    putsram_UART((char*) &dump[0]);
#endif

    // Работа декодера по отработке модификаторов MOD_2 и MOD_1
    if(DCCtrl==MOD_1)
    {
        scandc_proc = ScanDC_mod1;
        return;
    }
    else if(DCCtrl==MOD_2)
    {
        scandc_proc = ScanDC_mod20;
        VM61_GetOPCODE(OPC, 1);
        return;
    }
    else if(DCCtrl < SERVICE)       // декодер готов выпустить ОПКОД VM61
    {
        VM61_GetOPCODE(DCCtrl, 0);
    }
    else if(DCCtrl==SERVICE)        // декодер должен управлять апп.комплексом
    {
        ((servcall*) __LPM_word(&SERVICE_PROC[__LPM(pDC+4)]))();
        MK61.VM61.FLAGS.DISP = 1; // Изменения управлением комплекса вызывают изменения индикации
    }

    ScanDC_RESET(); // мавр сделал свое дело - вернем автомат в ноль
}

