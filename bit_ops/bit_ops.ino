/*  (c0,c1,c2,c3) + (d2,d3,d4,d5)
 *   (b0,b1,b2,b3,b4)
 */
#define  NUM_ROWS  8
#define  NUM_COLS  5

byte scanMask=0b00000001;
byte maskB=0b11100000;
byte maskC=0b11110000;
byte maskD=0b11000011;

byte rowPins[NUM_ROWS] = {A0, A1, A2, A3, 2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad

 void setup(){
/*  PORTB=PORTB&maskB;
  PORTC=PORTC&maskC;
  PORTD=PORTD&maskD;
  DDRB=DDRB&maskB;//выход
  DDRC=DDRC|maskC;//вход
  DDRD=DDRD|maskD;//вход*/


  for (byte c = 0 ; c < NUM_COLS ; c++)
  {
    pinMode(colPins[c], OUTPUT);
 //   digitalWrite(colPins[c], HIGH);
  }
  for (byte r = 0 ; r < NUM_ROWS ; r++)
  {
    pinMode(rowPins[r], INPUT);
 //   digitalWrite(rowPins[r], HIGH);
  }
  
  Serial.begin(9600);
  }
byte curC=0;
byte lstC=0;
byte curD=0;
byte lstD=0;

 void loop(){
PORTB=PORTB|0b00000100; 
delay(10);
  curC=PINC|maskC;
  curD=PIND|maskD;
  if (curC!=lstC) {
    Serial.print("  :  C:");
    Serial.print (lstC,BIN);
    Serial.print("  -  ");
    Serial.println (curC,BIN);
    }
    lstC=curC;
  if (curD!=lstD) {
    Serial.print("  :  D:");
    Serial.print (lstD,BIN);
    Serial.print("  -  ");
    Serial.println (curD,BIN);
    }
    lstD=curD;
delay(500);
}
