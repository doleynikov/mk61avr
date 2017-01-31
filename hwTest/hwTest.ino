// ZX81 Keyboard
// © Dave Curran
// 2012-02-15

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define LED_PIN LED_BUILTIN
//13

#define NUM_ROWS 8
#define NUM_COLS 5

// Missing key definitions
#define KEY_ESCAPE 41
#define KEY_DELETE 42
#define KEY_PERIOD 55
#define KEY_RIGHT 79
#define KEY_LEFT 80
#define KEY_DOWN 81
#define KEY_UP 82

// keymap for normal use
byte keyMap[NUM_ROWS][NUM_COLS] = {
  {'5', '4', '3', '2', '1'}, 
  {'T', 'R', 'E', 'W', 'Q'}, 
  {'6', '7', '8', '9', '0'},
  {'G', 'F', 'D', 'S', 'A'}, 
  {'Y', 'U', 'I', 'O', 'P'},
  {'V', 'C', 'X', 'Z', '#'}, 
  {'H', 'J', 'K', 'L', '*'},
  {'B', 'N', 'M', '.', '='}
};

// keymap if shift is pressed
byte keyMapShifted[NUM_ROWS][NUM_COLS] = {
  {'<', 0, 0, 0, '~'}, 
  {0, 0, 0, 0, 0}, 
  {'v', '^', '>', 0, '-'},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0},
};

int debounceCount[NUM_ROWS][NUM_COLS];

// define the row and column pins
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12};
byte rowPins[NUM_ROWS] = {A0, A1, A2, A3, 2,3,4,5};

// where is the shift key
#define SHIFT_COL 4
#define SHIFT_ROW 5

// how many times does a key need to register as pressed?
#define DEBOUNCE_VALUE 200
#define REPEAT_DELAY 400

// If the timer isr is corrected
// to not take so long change this to 0.
#define BYPASS_TIMER_ISR 1

/*             */
/*    setup    */
/*             */
void setup()
{
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("MK61!");

  
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // set all pins as inputs and activate pullups
  for (byte c = 0; c < NUM_COLS; c++)
  {
    pinMode(colPins[c], INPUT);
    digitalWrite(colPins[c], HIGH);

    // clear debounce counts
    for (byte r = 0; r < NUM_ROWS; r++)
    {
      debounceCount[r][c] = 0;
    }
  }

  // set all pins as inputs
  for (byte r = 0; r < NUM_ROWS; r++)
  {
    pinMode(rowPins[r], INPUT);
  }

#if BYPASS_TIMER_ISR
  // disable timer 0 overflow interrupt (used for millis)
  TIMSK0&=!(1<<TOIE0); // ++
#endif
}

#if BYPASS_TIMER_ISR
/*               */
/*    delayMs    */
/*               */
void delayMs(unsigned int ms) 
{
  for (unsigned int i = 0; i < ms; i++)
  {
    delayMicroseconds(1000);
  }
}
#endif

/*             */
/*    loop     */
/*             */
void loop()
{
  // check for shift
  bool shifted = false;
  bool keyPressed = false;
  pinMode(rowPins[SHIFT_ROW], OUTPUT);

  if (digitalRead(colPins[SHIFT_COL]) == LOW)
  {
    shifted = true;
  }
  pinMode(rowPins[SHIFT_ROW], INPUT);

  // foreach row
  for (byte r = 0; r < NUM_ROWS; r++)
  {
    // turn the row on
    pinMode(rowPins[r], OUTPUT);
    digitalWrite(rowPins[r], LOW);

    for (byte c = 0; c < NUM_COLS; c++)
    {
      if (digitalRead(colPins[c]) == LOW)
      {
        // increase the debounce count
        debounceCount[r][c]++;

        // has the switch been pressed continually for long enough?
        int count = debounceCount[r][c];
        if (count == DEBOUNCE_VALUE)
        {
          // first press
          keyPressed = true;          
          pressKey(r, c, shifted);
        }
        else if (count > DEBOUNCE_VALUE)
        {
          // check for repeats
          count -= DEBOUNCE_VALUE;
          if (count % REPEAT_DELAY == 0)
          {
            // send repeat
            keyPressed = true;          
            pressKey(r, c, shifted);
            //pressKey(0, 0, false); // for testing, send 1 on the repeats rather than the key to distingush the repeats.
          }  
        }
      }
      else
      {
        // not pressed, reset debounce count
        debounceCount[r][c] = 0;
      }
    }

    // turn the row back off
    pinMode(rowPins[r], INPUT);
  }

  if (keyPressed)
  {
    // turn the LED on
    digitalWrite(LED_PIN, HIGH);
  
#if BYPASS_TIMER_ISR  // check if timer isr fixed.
    delayMs(20);
#else
    delay(20);
#endif

    // turn the LED off again
    digitalWrite(LED_PIN, LOW);
  }
  digitalWrite(rowPins[SHIFT_ROW], LOW);
}

// Send the keypress
void pressKey(byte r, byte c, bool shifted)
{  
  // which keybaord map to use?
  if (shifted)
  {
    byte key = keyMapShifted[r][c];
    if (key > 0)
    {
      // specially defined key in the shifted bitmap
      lcd.print(key);
Serial.println(char(key));
}
    else
    {
      // use the normal keymap and send the shift modifier
      key = keyMap[r][c];
      if (key > 0)
      {
             lcd.print(key+":MOD_SHIFT_LEFT");
Serial.println(char(key)+":MOD_SHIFT_LEFT");
}
    }
  }
  else
  {
    byte key = keyMap[r][c];
    if (key > 0)
    {
      // send the key
      lcd.print(key);
      Serial.println(char(key));
    }
  }
}

