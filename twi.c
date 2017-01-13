// ***********************************************************
// Project: Эмулятор программируемого калькулятора МК-61 на AVR:
// https://github.com/doleynikov/mk61avr
//
// Copyright (с) 2017 Алексей Сугоняев, Виталий Самуров, Дмитрий Олейников
//
// Module name: twi.c
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

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 */

/* $Id: twitest.c,v 1.6 2005/11/05 22:32:46 joerg_wunsch Exp $ */

/*
 * Simple demo program that talks to a 24Cxx IІC EEPROM using the
 * builtin TWI interface of an ATmega device.
 */
#include "config.h"

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/twi.h>       /* Note [1] */
#include "extlib.h"
#include "mk61types.h"
#include "usart_module.h"
#include "i2c.h"

/*
 * System clock in Hz.
 */
#define F_CPU 8000000UL /* Note [2] */

/*
 * Note [3]
 * TWI address for 24Cxx EEPROM:
 *
 * 1 0 1 0 E2 E1 E0 R/~W    24C01/24C02
 * 1 0 1 0 E2 E1 A8 R/~W    24C04
 * 1 0 1 0 E2 A9 A8 R/~W    24C08
 * 1 0 1 0 A10 A9 A8 R/~W   24C16
 */
#define TWI_SLA_24CXX   0xA0    /* E2 E1 E0 = 0 0 0 */

/*
 * Maximal number of iterations to wait for a device to respond for a
 * selection.  Should be large enough to allow for a pending write to
 * complete, but low enough to properly abort an infinite loop in case
 * a slave is broken or not present at all.  With 100 kHz TWI clock,
 * transfering the start condition and SLA+R/W packet takes about 10
 * µs.  The longest write period is supposed to not exceed ~ 10 ms.
 * Thus, normal operation should not require more than 100 iterations
 * to get the device to respond to a selection.
 */
#define MAX_ITER    200

/*
 * Number of bytes that can be written in a row, see comments for
 * ee24xx_write_page() below.  Some vendor's devices would accept 16,
 * but 8 seems to be the lowest common denominator.
 *
 * Note that the page size must be a power of two, this simplifies the
 * page boundary calculations below.
 */
#define PAGE_SIZE 8

/*
 * Saved TWI status register, for error messages only.  We need to
 * save it in a variable, since the datasheet only guarantees the TWSR
 * register to have valid contents while the TWINT bit in TWCR is set.
 */
unsigned char twst;
#define SCL_CLOCK 100000UL
/*
 * Do all the startup-time peripheral initializations: UART (for our
 * debug/test output), and TWI clock.
 */
void TWI_Init(void)
{

  /* initialize TWI clock: 100 kHz clock, TWPS = 0 => prescaler = 1 */
#if defined(TWPS0)
  /* has prescaler (mega128 & newer) */
  TWSR = 0;
#endif

#if F_CPU < 3600000UL
  TWBR = 10;            /* smallest TWBR value, see note [5] */
#else
  TWBR = (F_CPU / SCL_CLOCK - 16) / 2;
#endif
}

/*
 * Note [7]
 *
 * Read "len" bytes from EEPROM starting at "eeaddr" into "buf".
 *
 * This requires two bus cycles: during the first cycle, the device
 * will be selected (master transmitter mode), and the address
 * transfered.  Address bits exceeding 256 are transfered in the
 * E2/E1/E0 bits (subaddress bits) of the device selector.
 *
 * The second bus cycle will reselect the device (repeated start
 * condition, going into master receiver mode), and transfer the data
 * from the device to the TWI master.  Multiple bytes can be
 * transfered by ACKing the client's transfer.  The last transfer will
 * be NACKed, which the client will take as an indication to not
 * initiate further transfers.
 */
int ee24xx_read_bytes(sE24 dev, unsigned int eeaddr, int len, unsigned char *buf)
{
  unsigned char sla, twcr, n = 0;
  int rv = 0;

  /* выделяем физический адрес устройства - старшие 4 бита для 24LC32 */
  sla = TWI_SLA_24CXX | (dev.addr << 1);

  /*
   * Note [8]
   * First cycle: master transmitter mode
   */
  restart:
  if (n++ >= MAX_ITER)  return -1;   // Таймаут по кол-ву неподтвержденных ACK циклов
  begin:

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); // Посылка START условия

  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

  switch ((twst = TW_STATUS))
    {
    case TW_REP_START:      /* OK, but should not happen */
    case TW_START:
      break;

    case TW_MT_ARB_LOST:    /* Note [9] */
      goto begin;

    default:
      return -1; // Отсюда еще можно сбежать поскольку ошибка произошла при посылке START условия и посылка STOP не требуется
                /* NB: do /not/ send stop condition */
    }

  /* Note [10] */

 /*
   Для установки произвольного адреса для последующего чтения, инициализируем операцию записи
   формируем байт адрес устройства I2C [A0:A2] и бит RW = 1 (TW_WRITE) (send SLA+W)
 */
  TWDR = sla | TW_WRITE;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */

  while ((TWCR & _BV(TWINT)) == 0) ; /* Ждем завершения передачи */

  switch ((twst = TW_STATUS))
    {
    case TW_MT_SLA_ACK:
      break;

    case TW_MT_SLA_NACK:    /* nack during select: device busy writing */
                /* Note [11] */
      goto restart;

    case TW_MT_ARB_LOST:    /* re-arbitrate */
      goto begin;

    default:
      goto error;       /* must send stop condition */
    }

  if(dev.a16){ // Если устройство 16 битное отправим старший байт адреса

   TWDR = (unsigned char) (eeaddr>>8);      // Старшие 8 бит, остальное маскируем
   TWCR = _BV(TWINT) | _BV(TWEN);            // Продолжаем операции в передачтике TWI
   while ((TWCR & _BV(TWINT)) == 0) ; /* Ожидаем конца передачи */

   switch ((twst = TW_STATUS))
     {
     case TW_MT_DATA_ACK:
       break;

     case TW_MT_DATA_NACK:
       goto quit;

     case TW_MT_ARB_LOST:
       goto begin;

     default:
       goto error;      /* must send stop condition */
     }
  }

/*  Посылаем логический адрес внутри устройства по которому начнем чтение - младшие 8 бит */
  TWDR = (unsigned char) eeaddr; // вторая часть адреса - младшие 8 бит
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */

  while ((TWCR & _BV(TWINT)) == 0) ; // Ожидаем конца передачи

  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;       /* must send stop condition */
    }

  /*
   * Note [12]
   * Next cycle(s): master receiver mode
   */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);   // Посылка START условия, повторно REP.START
  while ((TWCR & _BV(TWINT)) == 0);             // Ожидаем конца передачи

  switch ((twst = TW_STATUS))
    {
    case TW_START:      /* OK, but should not happen */
    case TW_REP_START:
      break;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;
    }

  // Посылка управляющего байта - теперь уже на чтение, физадрес устройства и RW = 1 (send SLA+R)
  TWDR = sla | TW_READ;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0); // Ожидаем конца передачи

  switch ((twst = TW_STATUS))
    {
    case TW_MR_SLA_ACK:
      break;

    case TW_MR_SLA_NACK:
      goto quit;

    case TW_MR_ARB_LOST:
      goto begin;

    default:
      goto error;
    }

  for (twcr = _BV(TWINT) | _BV(TWEN) | _BV(TWEA) /* Устанавливаем посылку ACK Note [13] */;
       len > 0;
       len--)
    {
      if (len == 1) twcr = _BV(TWINT) | _BV(TWEN); /* В последнем принятом байте ACK не шлем send NAK this time */
      TWCR = twcr;      /* clear int to start transmission */
      while ((TWCR & _BV(TWINT)) == 0); // Ожидаем конца передачи
      switch ((twst = TW_STATUS))
    {
    case TW_MR_DATA_NACK:
      len = 0;      /* force end of loop */
      /* FALLTHROUGH */
    case TW_MR_DATA_ACK:
      *buf++ = TWDR;
      rv++;
      break;

    default:
      goto error;
    }
    }
  quit:
  /* Note [14] */
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

  return rv;

  error:
  rv = -1;
  goto quit;
}

/*
 * Write "len" bytes into EEPROM starting at "eeaddr" from "buf".
 *
 * This is a bit simpler than the previous function since both, the
 * address and the data bytes will be transfered in master transmitter
 * mode, thus no reselection of the device is necessary.  However, the
 * EEPROMs are only capable of writing one "page" simultaneously, so
 * care must be taken to not cross a page boundary within one write
 * cycle.  The amount of data one page consists of varies from
 * manufacturer to manufacturer: some vendors only use 8-byte pages
 * for the smaller devices, and 16-byte pages for the larger devices,
 * while other vendors generally use 16-byte pages.  We thus use the
 * smallest common denominator of 8 bytes per page, declared by the
 * macro PAGE_SIZE above.
 *
 * The function simply returns after writing one page, returning the
 * actual number of data byte written.  It is up to the caller to
 * re-invoke it in order to write further data.
 */
int ee24xx_write_page(sE24 dev, unsigned int eeaddr, int len, unsigned char *buf)
{
  unsigned char sla, n = 0;
  int rv = 0;
  unsigned int endaddr;

  /* выделяем физический адрес устройства */
  sla = TWI_SLA_24CXX | (dev.addr << 1);

  if (eeaddr + len < (eeaddr | (PAGE_SIZE - 1)))
    endaddr = eeaddr + len;
  else
    endaddr = (eeaddr | (PAGE_SIZE - 1)) + 1;
  len = endaddr - eeaddr;

  restart:
  if (n++ >= MAX_ITER)
    return -1;
  begin:

  /* Note [15] */
  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_REP_START:      /* OK, but should not happen */
    case TW_START:
      break;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      return -1;        /* error: not in start condition */
                /* NB: do /not/ send stop condition */
    }

 /*
   Для установки произвольного адреса для последующей записи, инициализируем операцию записи
   формируем байт адрес устройства I2C [A0:A2] и бит RW = 1 (TW_WRITE) (send SLA+W)
 */
  TWDR = sla | TW_WRITE;
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
  switch ((twst = TW_STATUS))
    {
    case TW_MT_SLA_ACK:
      break;

    case TW_MT_SLA_NACK:    /* nack during select: device busy writing */
      goto restart;

    case TW_MT_ARB_LOST:    /* re-arbitrate */
      goto begin;

    default:
      goto error;       /* must send stop condition */
    }


/* Посылаем логический адрес внутри устройства по которому начнем чтение */
 if(dev.a16){ // посылаем старшие 8 бит если они есть

  TWDR = (unsigned char) (eeaddr>>8);       // Старшие 8 бит
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */

  while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */

  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;       /* must send stop condition */
    }
 }
/*
  Посылаем логический адрес внутри устройства по которому начнем чтение
  для устройств 24LC32 валиден только адрес в первых 12 битах, второй пакет младшие 8 бит
*/
  TWDR = (unsigned char) eeaddr;        /* вторая часть адреса - младшие 8 бит */
  TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */

  while ((TWCR & _BV(TWINT)) == 0) ; /* Ожидаем конца передачи */

  switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_ACK:
      break;

    case TW_MT_DATA_NACK:
      goto quit;

    case TW_MT_ARB_LOST:
      goto begin;

    default:
      goto error;       /* must send stop condition */
    }

  for (; len > 0; len--)
    {
      TWDR = *buf++;
      TWCR = _BV(TWINT) | _BV(TWEN); /* start transmission */
      while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
      switch ((twst = TW_STATUS))
    {
    case TW_MT_DATA_NACK:
      goto error;       /* device write protected -- Note [16] */

    case TW_MT_DATA_ACK:
      rv++;
      break;

    default:
      goto error;
    }
    }
  quit:
  TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */

  return rv;

  error:
  rv = -1;
  goto quit;
}

/*
 * Wrapper around ee24xx_write_page() that repeats calling this
 * function until either an error has been returned, or all bytes
 * have been written.
 */
int ee24xx_write_bytes(sE24 dev, unsigned int eeaddr, int len, unsigned char *buf)
{
  int rv, total;

  total = 0;
  do
    {
      rv = ee24xx_write_page(dev, eeaddr, len, buf);
      if (rv == -1)
    return -1;
      eeaddr += rv;
      len -= rv;
      buf += rv;
      total += rv;
    }
  while (len > 0);

  return total;
}

/*
 Обнаружение и определение размера I2C устройства подключенного к внешней шине I2C
 по его физическому адресу device_addr.
  Результат SIZE:
  0 - устройство с данным адресом не обнаружено.
  >0 - устройство обнаружено объем данных SIZE

unsigned int ee24xx_check(sE24 dev)
{
 register unsigned int chk_addr = 64;
 unsigned char data;
 register unsigned i=0;

 if(ee24xx_read_bytes(dev, 15, 1, &data) == -1) return 0;

 while(chk_addr != 0){
  if(ee24xx_read_bytes(dev, (chk_addr<<=1) - 1, 1, &data) == -1) break;
  i++;
 };

 if(chk_addr > 128) return i; //chk_addr;
 return i;
}
*/
