 
#define KEY_ENTER 13
#define KEY_BRKFLAG ' '
#define KEY_SHIFT1 2
#define KEY_SHIFT2 3

// Keyboard
#define  NUM_ROWS  8
#define  NUM_COLS  5
char keys[NUM_ROWS][NUM_COLS] = {
  {'5', '4', '3', '2', '1'},
  {'T', 'R', 'E', 'W', 'Q'},
  {'G', 'F', 'D', 'S', 'A'},
  {'V', 'C', 'X', 'Z', KEY_SHIFT1},
  {'6', '7', '8', '9', '0'},
  {'Y', 'U', 'I', 'O', 'P'},
  {'H', 'J', 'K', 'L', KEY_ENTER},
  {'B', 'N', 'M', KEY_SHIFT2, ' '}
};

char shift1_keys[NUM_ROWS][NUM_COLS] = {
  {'%', '$', '#', '\"', '\''},
  {'T', 'R', 'E', 'W', 'Q'},
  {'G', 'F', 'D', 'S', 'A'},
  {'V', 'C', 'X', 'Z', KEY_SHIFT1},
  {'^', '&', '*', '(', ')'},
  {'Y', 'U', 'I', 'O', 'P'},
  {'H', 'J', 'K', 'L', KEY_ENTER},
  {'B', 'N', 'M', KEY_SHIFT2, ' '}
};

char shift2_keys[NUM_ROWS][NUM_COLS] = {
  {'%', '$', '#', '\"', '\''},
  {'t', 'r', 'e', 'w', 'q'},
  {'g', 'f', 'd', 's', 'a'},
  {'v', 'c', 'x', 'z', KEY_SHIFT1},
  {'^', '&', '*', '(', ')'},
  {'y', 'u', 'i', 'o', 'p'},
  {'h', 'j', 'k', 'l', KEY_ENTER},
  {'b', 'n', 'm', KEY_SHIFT2, ' '}
};

byte rowPins[NUM_ROWS] = {A0, A1, A2, A3, 2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad

byte row;
byte col;
byte kb_row;
byte kb_col;
char keyPressed; //raw key from the first map
char keyReleased; //key released

byte shiftFlag = 0; //1-shift1, 2-shift2

void setup() {
  for (byte c = 0 ; c < NUM_COLS ; c++)
  {
    pinMode(colPins[c], INPUT);
    digitalWrite(colPins[c], HIGH);
  }
  for (byte r = 0 ; r < NUM_ROWS ; r++)
  {
    pinMode(rowPins[r], INPUT);
  }
  shiftFlag = 0;

  Serial.begin(9600);

  TCCR2A = 0;
  TCCR2B = 2;
  TCNT2 = 255;
  TIMSK2 |= (1 << TOIE2);
}

char kbGet()
{
  char lastkey = 0;
  if (keyReleased)
  {
    switch (shiftFlag)
    {
      case 0:
        {
          lastkey = keys[kb_row][kb_col];
        }
        break;
      case 1:
        {
          lastkey = shift1_keys[kb_row][kb_col];
          shiftFlag = 0;
        }
        break;
      case 2:
        {
          lastkey = shift2_keys[kb_row][kb_col];
          shiftFlag = 0;
        }
        break;
    }
  }
  keyReleased = 0;

  return lastkey;
}

void loop()
{
  char key = kbGet();
  if (key)Serial.println(key);
}


int  iCnt = 0;
char iKey = 0;
#define KEY_CNT 10
ISR(TIMER2_OVF_vect) {
  if (keyPressed == 0) {
      keyReleased = 0;
    for (byte r = 0 ; r < NUM_ROWS ; r++)
    {
      pinMode(rowPins[r], OUTPUT);
      digitalWrite(rowPins[r], LOW);
      for (byte c = 0 ; c < NUM_COLS ; c++)
      {
        if (digitalRead(colPins[c]) == LOW)
        {
          keyPressed = keys[r][c];
            iKey = keyPressed;
            iCnt = 0;
            if (keyPressed == KEY_SHIFT1) {
              shiftFlag = 1;
            }
            else if (keyPressed == KEY_SHIFT2) {
              shiftFlag = 2;
            }
            else {
              row = r;
              col = c;
          }
        }
      }
      pinMode(rowPins[r], INPUT);
    }

  }
  else {
    if (iKey){keyReleased=iKey;iKey=0;keyPressed=0;kb_row=row; kb_col=col;}
    }

  TCNT2 = 255; //55;
}



