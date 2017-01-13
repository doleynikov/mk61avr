// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// http://code.google.com/p/mk61avr/
//
// Получить локальную копию проекта из GIT:
// git clone https://code.google.com/p/mk61avr/
//
// Дискуссия по проекту в Google Groups:
// http://groups.google.com/group/mk61avr_talks
//
// Copyright (C) 2009-2011 Алексей Сугоняев, Виталий Самуров
//
// Module name: store.c
//
// Module description:
//      Модуль работы с внешним хранилищем (I2C EEPROM);
//      простейшая файловая система. Для быстроты форматирования
//      требуется переписать заголовок EEPROM,
//      обнулив счетчик. Модуль внешней памяти EEPROM
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
#include <avr/pgmspace.h>
#include <string.h>
#include "config.h"
#include "mk61types.h"
#include "i2c.h"
#include "extlib.h"
#include "usart_module.h"
#include "store.h"

#ifdef DEBUG_STORE
 const char aTWI_ERROR0[]     PROGMEM = "\rSTORE: error %x %x";
 const char STORE_NOT_FOUND[] PROGMEM = "\rSTORE: MBR not found!";
#endif

const sMBR STD_MBR PROGMEM =
{
    {'m','k','6','1','a','v','r','.','S','T','O','R','E',0x00,0x00}, 0, sizeof(sMBR), 0
};

unsigned char StoreBuff[64];  // Буфер для работы с файловой системой
sMBR STORE_MBR;

#ifdef DEBUG_STORE
/*************************************************************************
Name: EEPROM_PutErrorCode

Input:    int ret

Returns:  none
*************************************************************************/
void EEPROM_PutErrorCode(int ret)
{
    __sprintf_P(&dump[0], aTWI_ERROR0, ((short int) twst)|((short int)(ret<<8)));
    putsram_UART(&dump[0]);
}
#endif


/*************************************************************************
Name: STORE_init

Input:    none

Returns:  signed char
*************************************************************************/
signed char STORE_init(void)
{
    register int tmp = ee24xx_read_bytes(STORE_DEVICE, 0, sizeof(sMBR),(unsigned char*) &STORE_MBR);
    if(tmp < 0)
    {
        memset(&STORE_MBR, 0, sizeof(sMBR));
#ifdef DEBUG_STORE
        EEPROM_PutErrorCode(tmp);
#endif
    }

    if(strcmp_P(STORE_MBR.name, STD_MBR.name) == 0)
    {
        memset(&STORE_MBR, 0, sizeof(sMBR));
#ifdef DEBUG_STORE
        putsrom_UART((unsigned char*) &STORE_NOT_FOUND);
#endif
    }

    return tmp;
}


/*************************************************************************
Name: STORE_format

Input:    unsigned int eesize

Returns:  signed char
*************************************************************************/
signed char STORE_format(unsigned int eesize)
{
    register int ret;

    memcpy_P(&STORE_MBR,&STD_MBR,sizeof(sMBR));
    STORE_MBR.Size = eesize;
    STORE_MBR.FLast = eesize - 1;
    ret = ee24xx_write_bytes(STORE_DEVICE, 0, sizeof(sMBR), (unsigned char*) &STORE_MBR);

    if(ret < 1)
    {
        memset(&STORE_MBR, 0, sizeof(sMBR));
#ifdef DEBUG_STORE
        EEPROM_PutErrorCode(ret);
#endif
    }

    return ret;
}


/*************************************************************************
Name: STORE_fopen

Input:    unsigned char FileNum
          FILE *file

Returns:  signed char

    Открыть файл - найти файл и заполнить структуру FILE данными
*************************************************************************/
signed char STORE_fopen(unsigned char FileNum, FILE *file)
{
    register int tmp;
    register unsigned int endfr = sizeof(sMBR);

    do
    {
        memset(&StoreBuff, 0, sizeof(StoreBuff));
        tmp = ee24xx_read_bytes(STORE_DEVICE, endfr, sizeof(StoreBuff),(unsigned char*) &StoreBuff);

        if(tmp < 1)
        {
#ifdef DEBUG_STORE
            EEPROM_PutErrorCode(tmp);
#endif
            return -1;
        }

        // Чтение прошло без ошибки
        tmp = (int) &StoreBuff;
        do
        {
            if(((FILE*) tmp)->Num == FileNum)
            {
                // Файл найден заполним структуру file
                memcpy(file,(void*) tmp,sizeof(FILE));
                return 0;
            }

        tmp += sizeof(FILE);
        } while(tmp < sizeof(StoreBuff));

        endfr += sizeof(StoreBuff);
    } while(endfr < STORE_MBR.FRLast);

    return -1;
}


/*************************************************************************
Name: STORE_buffered_write

Input:    unsigned int pStore
          void *pMem
          unsigned int Size

Returns:  unsigned int

    Буферезированная запись участка памяти в STORE
*************************************************************************/
unsigned int STORE_buffered_write(unsigned int pStore, void *pMem, unsigned int Size)
{
    register int ret;

    do
    {
        memcpy(&StoreBuff, pMem, sizeof(StoreBuff));  // скопируем в буфер содержимое страницы кода длинной в размер буфера
        ret = ee24xx_write_bytes(
                    STORE_DEVICE,
                    pStore,
                    Size < sizeof(StoreBuff)? Size : sizeof(StoreBuff),
                    (unsigned char*) &StoreBuff
                    );

        if(ret < 1)
        {
#ifdef DEBUG_STORE
            EEPROM_PutErrorCode(ret);
#endif
            return 0;
        }

        pStore += ret;
    } while((Size -= sizeof(StoreBuff)) > 0);

    return pStore;
}


/*************************************************************************
Name: STORE_fwrite

Input:    unsigned char FileNum
          unsigned char SizeCode

Returns:  none
*************************************************************************/
void STORE_fwrite(unsigned char FileNum, unsigned char SizeCode)
{
    register unsigned char *pSrc=(unsigned char*) &MK61.prg;
    register unsigned int tmp = STORE_MBR.FLast - (sizeof(MK61) - sizeof(MK61.prg) + SizeCode);
    register unsigned int first = tmp;
     FILE pFile;

    // Готовим запись о файле
    pFile.Start    = tmp;        //  адресс размещение в STORE
    pFile.CodeSize = SizeCode;   //  длину кода
    pFile.Num      = FileNum;    //  номер-имя файла

    // Запишем кодовую страницу
    tmp = STORE_buffered_write(tmp, pSrc, SizeCode);

    // Теперь необходимо записать остатки
    tmp = STORE_buffered_write(tmp, &MK61.label, sizeof(sMK61)-sizeof(MK61.prg));

    // Записать запись о файле
    STORE_buffered_write(STORE_MBR.FRLast, &pFile, sizeof(FILE));
    STORE_MBR.FLast = first;
    STORE_MBR.FRLast += sizeof(FILE);
}


/*************************************************************************
Name: STORE_fwrite

Input:    unsigned char nPrg

Returns:  none
*************************************************************************/
void STORE_fread(unsigned char nPrg)
{
    //ee24xx_read_bytes(DEV_STORE, par<<8, 256, (unsigned char*) &MK61.prg[0]);
}

