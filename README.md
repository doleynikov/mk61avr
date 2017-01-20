# mk61avr

**Эмулятор разбросан по файлам**
main.c
mk61types.h
vm61.c
vm61.h
vm61decoder.c

**есть служебные функции**
extlib.c - преобразования 16-ти ричной цифры в символ для печати;
	atoc() - ?;
	чтение-запись участка памяти из/в EEPROM;

**есть работа с аппаратурой**
keyboard.c - работа с клавиатурой как с устройством 
keyboard.h
lcd.c - работа с 16x2 LCD по параллельному интерфейсу
lcd.h
lowlevel.h
micro_os.h
scan_dc.c - преобразование сканкодов клавиатуры в ОПкоды 
store.c - работа с внешним i2c хранилищем
store.h
terminal.c - работа с калькулятором из терминала
terminal.h
time.c - ? работа с таймером. Вероятно для опроса клавиатуры
time.h
twi.c - TWI (i2C) функции для работы с внешним хранилищем
uart.c - библиотека функций для работы с последовательным портом
usart_module.h

