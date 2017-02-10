#include "host.h"
#include "basic.h"



#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#define echo 1
#include <Keypad.h>
#define KEY_ENTER 13
#define KEY_BRK '#'
#define SYMBOL_SHIFT 1
#define CAPS_SHIFT 2


// Keyboard
#define  ROWS  8 
#define  COLS  5 
char keys[ROWS][COLS] = {
  {'5', '4', '3', '2', '1'},
  {'T', 'R', 'E', 'W', 'Q'},
  {'G', 'F', 'D', 'S', 'A'},
  {'V', 'C', 'X', 'Z', CAPS_SHIFT},
  {'6', '7', '8', '9', '0'},
  {'Y', 'U', 'I', 'O', 'P'},
  {'H', 'J', 'K', 'L', KEY_ENTER},
  {'B', 'N', 'M', SYMBOL_SHIFT, ' '}
};
char keysSymbol[ROWS][COLS] = {
  {'%', '$', '#', '@', '!'},
  {'>', '<', 'E', 'W', 'Q'},
  {'G', 'F', 'D', 'S', 'A'},
  {'/', '?', 'X', ':', CAPS_SHIFT},
  {'&', '\'', '(', ')', '_'},
  {'Y', 'U', 'I', ';', '\"'},
  {'H', '-', '+', '=', KEY_ENTER},
  {'*', ',', '.', SYMBOL_SHIFT, ' '}
};

byte rowPins[ROWS] = {A0, A1, A2, A3, 2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad

Keypad keyboard = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS); 

LiquidCrystal_I2C lcd(0x27, 16, 2);

//extern EEPROMClass EEPROM;
int timer1_counter;
volatile char flash = 0, redraw = 0;

char inputMode = 0;
char inkeyChar = 0;
char buzPin = 0;
#define SIZE_LINE 48

char lineBuffer[SIZE_LINE];
 const char  bytesFreeStr[] PROGMEM = "b free";

void initTimer() {
    noInterrupts();           // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    timer1_counter = 34286;   // preload timer 65536-16MHz/256/2Hz
    TCNT1 = timer1_counter;   // preload timer
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    interrupts();             // enable all interrupts
}

ISR(TIMER1_OVF_vect)        // interrupt service routine 
{
    TCNT1 = timer1_counter;   // preload timer
}

//***************************************************************************
// Terminal control


char inkey=0;

#define LCD_W 16
#define LCD_H 2

int lcdX = 0;
int lcdY = LCD_H-1; // start from the bottom line. Then - scrollUp
char screen[LCD_H][LCD_W];

void host_outchar(char c)
{
  lcd.write(c);
  }

void host_display_clear_rest()
{
  lcd.setCursor(lcdX, lcdY);         // cursor to original position
  for (int i = lcdX; i < LCD_W; i++) // clear the rest of line
  {
    host_outchar(' ');
    screen[lcdY][i] = ' ';
  }
  lcd.setCursor(lcdX, lcdY);         //return cursor to original position
}

void host_display_scroll()
{
  lcd.setCursor(0, 0);
  for (int y = 0; y < LCD_H - 1; y++) //move screen one line up
  {
    for (int x = 0; x < LCD_W; x++)
    {
      screen[y][x] = screen[y + 1][x];
      host_outchar(screen[y + 1][x]);
    }
  }
  lcd.setCursor(lcdX, lcdY);         //return cursor to original position
  host_display_clear_rest();
}

void host_display_write(char c)
{
  if (c != 13 && c != 10) // regular character
  {
    if (lcdX >= LCD_W) // is x-position out of the screen?
    {
      lcdX = 0;
      host_display_scroll();
    }
    Serial.write(c);
    host_outchar(c);
    screen[lcdY][lcdX] = c;
    lcdX++;
  }

  else if (c == 10) {
    host_display_scroll();
 //     Serial.println("LF ("+String(lcdX)+","+String(lcdY)+")");
    host_display_clear_rest();
    Serial.write('\n');
  }//LineFeed - next line
  else if (c == 13) {
    lcdX = 0;
    Serial.write('\r');
 //     Serial.println("CR ("+String(lcdX)+","+String(lcdY)+")");
  }//CR - first position of current line
  lcd.setCursor(lcdX, lcdY);
}

void host_display_cls()
{
    lcd.clear();
    lcd.setCursor(0,0);

lcdX=0;
lcdY=LCD_H-1;  
}
int host_getch() {
//  delay(1);  
  int ret;
  ret=keyboard.getKey();
if (!ret) {ret=Serial.read();}
  return ret;
}

//***************************************************************************
void host_init(int buzzerPin) {
    buzPin = buzzerPin;
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
  host_display_cls();
  
  if (buzPin)
        pinMode(buzPin, OUTPUT);
    initTimer();
}

void host_sleep(long ms) {
    delay(ms);
}

void host_digitalWrite(int pin,int state) {
    digitalWrite(pin, state ? HIGH : LOW);
}

int host_digitalRead(int pin) {
    return digitalRead(pin);
}

int host_analogRead(int pin) {
    return analogRead(pin);
}

void host_pinMode(int pin,int mode) {
    pinMode(pin, mode);
}

void host_startupTone() {
    if (!buzPin) return;
    for (int i=1; i<=2; i++) {
        for (int j=0; j<50*i; j++) {
            digitalWrite(buzPin, HIGH);
            delay(3-i);
            digitalWrite(buzPin, LOW);
            delay(3-i);
        }
        delay(100);
    }    
}

void host_cls() {
  void host_display_cls();
}

void host_moveCursor(int x, int y) {
//lcd.setCursor(x,y); // unimplemented
}

void host_outputString(char *str) {
    while (*str) {
      host_display_write(*str++);
    }
}

void host_outputProgMemString(const char *p) {
    while (1) {
        unsigned char c = pgm_read_byte(p++);
        if (c == 0) break;
        host_display_write(c);
    }
}

void host_outputChar(char c) {
 host_display_write(c);
}

int host_outputInt(long num) {
    // returns len
    long i = num, xx = 1;
    int c = 0;

    do {
        c++;
        xx *= 10;
        i /= 10;
    } 
    while (i);

    for (int i=0; i<c; i++) {
        xx /= 10;
        char digit = ((num/xx) % 10) + '0';
        host_outputChar(digit);
    }
    return c;
}

char *host_floatToStr(float f, char *buf) {
    // floats have approx 7 sig figs
    float a = fabs(f);
    if (f == 0.0f) {
        buf[0] = '0'; 
        buf[1] = 0;
    }
    else if (a<0.0001 || a>1000000) {
        // this will output -1.123456E99 = 13 characters max including trailing nul
        dtostre(f, buf, 6, 0);
    }
    else {
        int decPos = 7 - (int)(floor(log10(a))+1.0f);
        dtostrf(f, 1, decPos, buf);
        if (decPos) {
            // remove trailing 0s
            char *p = buf;
            while (*p) p++;
            p--;
            while (*p == '0') {
                *p-- = 0;
            }
            if (*p == '.') *p = 0;
        }   
    }
    return buf;
}

void host_outputFloat(float f) {
    char buf[16];
    host_outputString(host_floatToStr(f, buf));
}


void host_newLine() {
host_outputChar(13);
host_outputChar(10);
}

char c_isprint(char c) {
  return (c >= 32 && c <= 126);
}
char c_isspace(char c) {
  return (c == ' ' || (c <= 13 && c >= 9));
}

char *host_readLine() {
    inputMode = 1;
    int pos = 0;
    bool done = false;
    host_outputChar('>'); // prompt
    while (!done) {
            char c = host_getch();
            // read the next key
            if (c>=32 && c<=126)
                {lineBuffer[pos++] = c;
                host_outputChar(c); }//printable char
            else if (c==8 && pos > 0)
                lineBuffer[--pos] = 0;
            else if (c==KEY_ENTER) //13
                done = true;
    }
    lineBuffer[pos] = 0;
    inputMode = 0;
    return &lineBuffer[0];
}

char host_getKey() {
    return host_getch();
}

bool host_ESCPressed() {
        inkeyChar = host_getKey();
        if (inkeyChar == KEY_BRK) //27 - escape
            return true;
    return false;
}


void host_outputFreeMem(unsigned int val)
{
    host_newLine();
    host_outputInt(val);
    host_outputChar(' ');
    host_outputProgMemString(bytesFreeStr);  
    host_newLine();    
}

void host_saveProgram(bool autoexec) {
    EEPROM.write(0, autoexec ? MAGIC_AUTORUN_NUMBER : 0x00);
    EEPROM.write(1, sysPROGEND & 0xFF);
    EEPROM.write(2, (sysPROGEND >> 8) & 0xFF);
    for (int i=0; i<sysPROGEND; i++)
        EEPROM.write(3+i, mem[i]);
}

void host_loadProgram() {
    // skip the autorun byte
    sysPROGEND = EEPROM.read(1) | (EEPROM.read(2) << 8);
    for (int i=0; i<sysPROGEND; i++)
        mem[i] = EEPROM.read(i+3);
}


