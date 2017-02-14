/*  (c0,c1,c2,c3) + (d2,d3,d4,d5)
     (b0,b1,b2,b3,b4)
*/
#define  NUM_ROWS  8
#define  NUM_COLS  5

byte scanMask = 0b00000001;
byte maskB = 0b11100000;
byte maskC = 0b11110000;
byte maskD = 0b11000011;

byte rowPins[NUM_ROWS] = {A0, A1, A2, A3, 2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad

#define KEY_ENTER 13
#define KEY_BRK '#'
#define SYMBOL_SHIFT 1
#define CAPS_SHIFT 2


// Keyboard
#define  ROWS  8
#define  COLS  5
char a[ROWS][COLS] = {
  {'5', '4', '3', '2', '1'},
  {'T', 'R', 'E', 'W', 'Q'},
  {'G', 'F', 'D', 'S', 'A'},
  {'V', 'C', 'X', 'Z', CAPS_SHIFT},
  {'6', '7', '8', '9', '0'},
  {'Y', 'U', 'I', 'O', 'P'},
  {'H', 'J', 'K', 'L', KEY_ENTER},
  {'B', 'N', 'M', SYMBOL_SHIFT, ' '}
};

int i = 0;
int j = 0;

void setup() {
  Serial.begin(9600);
}


void loop() {
  DDRB = 0xFF;
  PORTB = 0xFF;

  DDRC = 0x00;
  PORTC = 0b00001111; //0x0F;
  DDRD = 0x00;
  PORTD = 0b00111100; //0x0F;


  while (1) {
    for (i = 0; i < COLS; i++)
    {
      PORTB &= ~(1 << i);
      delay (20);
      for (j = 0; j < ROWS/2; j++)
      {
        if (!(PINC & (1 << j)))
        {
          Serial.println(a[j][i]);

        }
        if (!(PIND & (1 << j + 2)))
        {
          Serial.println(a[j + 4][i]);

        }
      }
      PORTB |= (1 << i);
      delay(10);
    }

  }

  return 0;
}

