/********************************************************************
 *        HW tester
 *******************************************************************/
//---- Wiring -----
// Arduino -- 23LC1024
// D13 <------> SCK
// D12 <------> MISO
// D11 <------> MOSI
// D10 <------> CS
// 5V  <------> VCC
// 5V  <------> HOLD
// 5V  <-10K-> CS       Resistor
// GND <------> VSS

#include <SPI.h>
#include <LiquidCrystal_I2C.h>

//#define ENABLE_SERIAL_CONSOLE                     // Uncomment for Serial console prints

#if defined(ARDUINO) && ARDUINO >= 100
#define printByte(args)  write(args);
#else
#define printByte(args)  print(args,BYTE);
#endif
 
//SRAM opcodes
#define RDSR        5
#define WRSR        1
#define READ        3
#define WRITE       2

#define  MAX_SIZE_23LC1024_SRAM             131072

LiquidCrystal_I2C lcd(0x27, 16, 2);     // Set the LCD address to 0x27 for a 16 chars and 2 line display
const float v_coeff = 1.5/1012;

const char fw_version[]= {'0','.','3', 0};      // Version number of firmvare

uint8_t column_0[8]  = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10};
uint8_t column_1[8]  = {0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8};
uint8_t column_2[8]  = {0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4, 0x4};
uint8_t column_3[8]  = {0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2, 0x2};
uint8_t column_4[8]  = {0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1};
uint8_t line_0[8]  = {0x1F, 0, 0, 0, 0, 0, 0, 0};
uint8_t line_1[8]  = {0, 0x1F, 0, 0, 0, 0, 0, 0};
uint8_t line_2[8]  = {0, 0, 0x1F, 0, 0, 0, 0, 0};
uint8_t line_3[8]  = {0, 0, 0, 0x1F, 0, 0, 0, 0};
uint8_t line_4[8]  = {0, 0, 0, 0, 0x1F, 0, 0, 0};
uint8_t line_5[8]  = {0, 0, 0, 0, 0, 0x1F, 0, 0};
uint8_t line_6[8]  = {0, 0, 0, 0, 0, 0, 0x1F, 0};
uint8_t line_7[8]  = {0, 0, 0, 0, 0, 0, 0, 0x1F};

uint16_t counter;


//Byte transfer functions
uint8_t Spi23LC1024Read8(uint32_t address) 
{
    uint8_t read_byte;
 
    PORTB &= ~(1 << PORTB2);            // Set SPI_SS low
    SPI.transfer(READ);
    SPI.transfer((uint8_t)(address >> 16) & 0xff);
    SPI.transfer((uint8_t)(address >> 8) & 0xff);
    SPI.transfer((uint8_t)address);
    read_byte = SPI.transfer(0x00);
    PORTB |= (1 << PORTB2);             // Set SPI_SS high
    return read_byte;
}
  
void Spi23LC1024Write8(uint32_t address, uint8_t data_byte) 
{
    PORTB &= ~(1 << PORTB2);            // Set SPI_SS low
    SPI.transfer(WRITE);
    SPI.transfer((uint8_t)(address >> 16) & 0xff);
    SPI.transfer((uint8_t)(address >> 8) & 0xff);
    SPI.transfer((uint8_t)address);
    SPI.transfer(data_byte);
    PORTB |= (1 << PORTB2);             // Set SPI_SS high
}
 
void setup(void) 
{
    uint32_t i;
    uint8_t r_value, w_value;
    uint8_t ram_ok = 1;

#ifdef ENABLE_SERIAL_CONSOLE
    Serial.begin(19200);
    while (!Serial);
#endif
    SPI.begin();
    lcd.init();

    pinMode(A3, INPUT);             // ADC3 port
    analogReference(INTERNAL);    

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SRAM test");
#ifdef ENABLE_SERIAL_CONSOLE
    Serial.println("23LC1024 RAM test started");
#endif
    
    for (i = 0; i < MAX_SIZE_23LC1024_SRAM; i++)
    {
        w_value = 0;
        Spi23LC1024Write8(i, w_value);
        r_value = Spi23LC1024Read8(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0x55;
        Spi23LC1024Write8(i, w_value);
        r_value = Spi23LC1024Read8(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xAA;
        Spi23LC1024Write8(i, w_value);
        r_value = Spi23LC1024Read8(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }

        w_value = 0xFF;
        Spi23LC1024Write8(i, w_value);
        r_value = Spi23LC1024Read8(i);
        if (r_value != w_value)
        {
          ram_ok = 0;
          break;
        }
        
        if((i % 4096) == 0)
        {
#ifdef ENABLE_SERIAL_CONSOLE
            Serial.print("Passed bytes: ");
            Serial.println(i);
#endif
            lcd.setCursor(0,1);
            lcd.print("bytes Ok: ");
            lcd.print(i);
        }        
    }

    if(ram_ok == 0)
    {
#ifdef ENABLE_SERIAL_CONSOLE
        Serial.print("SRAM error at ");
        Serial.print(i);
        Serial.print(" ");
        Serial.print(w_value);
        Serial.print(" != ");
        Serial.println(r_value);
#endif
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SRAM error at");
        lcd.setCursor(0,1);
        lcd.print(i);   
    }
    else
    {
#ifdef ENABLE_SERIAL_CONSOLE
        Serial.println("SRAM test passed");  
#endif
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SRAM test");
        lcd.setCursor(0,1);
        lcd.print("SRAM Ok");     
    }   
    
    delay(4000);  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LCD bcklght test");
    lcd.backlight();
    lcd.setCursor(0,1);
    lcd.print("ON");
    lcd.backlight();
    delay(4000);
    lcd.setCursor(0,1);
    lcd.print("OFF");
    lcd.noBacklight(); 
    delay(4000); 
    lcd.clear();

    lcd.setCursor(0,0);
    lcd.backlight();
    lcd.print("LCD pixels test");
    delay(2000);

    lcd.clear();
    counter = 0;   
}


void loop() 
{
    int adc_value;
    char buf[16];
    int  m, k;

    if(counter < 7)
    {
        if(counter % 2)
        {
            lcd.createChar(0, column_0);
            lcd.createChar(1, column_1);
            lcd.createChar(2, column_2);
            lcd.createChar(3, column_3);
            lcd.createChar(4, column_4);
      
            for (m = 0; m < 5; m++)
            {
                for (k = 0; k < 16; k++)
                {
                    lcd.setCursor(k, 0);
                    lcd.printByte(m);

                    lcd.setCursor(k, 1);
                    lcd.printByte(m);
                }

                delay(500);
            }
        }
        else
        {
            lcd.createChar(0, line_0);
            lcd.createChar(1, line_1);
            lcd.createChar(2, line_2);
            lcd.createChar(3, line_3);
            lcd.createChar(4, line_4);
            lcd.createChar(5, line_5);
            lcd.createChar(6, line_6);
            lcd.createChar(7, line_7);
        
            for (m = 0; m < 8; m++)
            {
                for (k = 0; k < 16; k++)
                {
                    lcd.setCursor(k, 0);
                    lcd.printByte(m);

                    lcd.setCursor(k, 1);
                    lcd.printByte(m);
                }

                delay(500);
            }
        }

        if(counter > 3)
        {
            lcd.noBacklight();
        }

        counter++;
    }
    else
    {
        adc_value = analogRead(A3);
        float voltage = v_coeff * adc_value;
        memset(buf, 0, sizeof(buf));
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("HW tester v.");
        lcd.print(fw_version);
        sprintf(buf, "ADC:%d Bat:%d.%01d", adc_value, (int)voltage, (int)(voltage*100)%100);
        lcd.setCursor(0,1);
        lcd.print(buf);
        delay(5000);  
        //Serial.print("Analog value: ");
        //Serial.println(adc_value);
    }
}
