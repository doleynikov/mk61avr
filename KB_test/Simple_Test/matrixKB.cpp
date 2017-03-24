#include "matrixKB.h"


#define KEY_ENTER 13
#define KEY_BRK 27 // пусть break будет кодом ESCAPE
#define SYMBOL_SHIFT 1
#define CAPS_SHIFT 2


// Keyboard
#define  ROWS  8
#define  COLS  5
char charMap[4][ROWS][COLS] =
{
    {
        //0
        {'5', '4', '3', '2', '1'},
        {'T', 'R', 'E', 'W', 'Q'},
        {'G', 'F', 'D', 'S', 'A'},
        {'V', 'C', 'X', 'Z', CAPS_SHIFT},
        {'6', '7', '8', '9', '0'},
        {'Y', 'U', 'I', 'O', 'P'},
        {'H', 'J', 'K', 'L', KEY_ENTER},
        {'B', 'N', 'M', SYMBOL_SHIFT, ' '}
    },
    {
        //1 = CAPS_SHIFT
        {PS2_LEFTARROW, '4', '3', '2', '1'},
        {'T', 'R', 'E', 'W', 'Q'},
        {'G', 'F', 'D', 'S', 'A'},
        {'V', 'C', 'X', 'Z', CAPS_SHIFT},
        {PS2_DOWNARROW, PS2_UPARROW, PS2_LEFTARROW, '9', PS2_BACKSPACE},
        {'Y', 'U', 'I', 'O', 'P'},
        {'H', 'J', 'K', 'L', KEY_ENTER},
        {'B', 'N', 'M', SYMBOL_SHIFT, KEY_BRK}
    },
    {
        //2 = SYMBOL_SHIFT
        {'%', '$', '#', '@', '!'},
        {'>', '<', 'E', 'W', 'Q'},
        {'}', '{', '\\', '|', '~'},
        {'/', '?', 'X', ':', CAPS_SHIFT},
        {'&', '\'', '(', ')', '_'},
        {'Y', 'U', 'I', 'O', 'P'},
        {'^', '-', '+', '=', KEY_ENTER},
        {'*', ',', '.', SYMBOL_SHIFT, ' '}
    },
    {
        //3 = CAPS_SHIFT and SYMBOL_SHIFT
        {'5', '4', '3', '2', '1'},
        {'T', 'R', 'E', 'W', 'Q'},
        {'G', 'F', 'D', 'S', 'A'},
        {'V', 'C', 'X', 'Z', CAPS_SHIFT},
        {'6', '7', '8', '9', '0'},
        {'Y', 'U', 'I', 'O', 'P'},
        {'H', 'J', 'K', 'L', KEY_ENTER},
        {'B', 'N', 'M', SYMBOL_SHIFT, ' '}
    },

};

uint8_t i = 0;
uint8_t j = 0;

static uint8_t CharBuffer=0;
static uint8_t row=0;
static uint8_t col=0;
static uint8_t oldRow=0;
static uint8_t oldCol=0;
static uint8_t shift=0B00000000;
static uint8_t keyPressed=0;

uint8_t getScanCode(uint8_t r,uint8_t c)
{
//    sh &= 0B00000011; // выдаляем биты шифтов
uint8_t sh=0;
    uint8_t ret=charMap[sh][r][c];

    if (ret==CAPS_SHIFT)
                {
                    shift ^=0B00000001; // инвертируем CAPS_SHIFT
                    ret=0;
                }
                else if (ret==SYMBOL_SHIFT)
                {
                    shift ^=0B00000010; // инвертируем SUMBOL_SHIFT
                    ret=0;
                }
                else {
                  sh=shift;
                  ret=charMap[sh][r][c];
                  shift &= 0B11111100; //обнулим биты шифтов
                  }

    
    if (ret==CharBuffer)ret=0;

    return ret;

}

// The ISR for the interrupt
ISR (TIMER2_COMPA_vect)
{
//    Serial.println(".");
    // Обработчик прерывания таймера 2
    for (i = 0; i < COLS; i++)
    {col=i;
        PORTB &= ~(1 << i);
        delay (20);
        for (j = 0; j < ROWS / 2; j++)
        {
            if (!(PINC & (1 << j)))
            {
                row=j;
                keyPressed = 1;
            }
            else if (!(PIND & (1 << (j + 2))))
            {
                row=j+4;
                keyPressed = 1;
            }
            else keypressed=0; // клавиши не нажаты

            if (keyPressed != 0 && (col != oldCol || row !=oldRow))
            {
                
                    uint8_t key=getScanCode(row, col);
                    if (key>0) {CharBuffer=key; oldRow=row; oldCol=col;}
                   
/*               Serial.print(CharBuffer);
               Serial.print(" -- row=");
               Serial.print(row);
               Serial.print(" -- col=");
               Serial.println(col);*/
            }
            keyPressed = 0;
        }
        PORTB |= (1 << i);
        delay(10);
    }
}


#define BREAK     0x01
#define MODIFIER  0x02
#define SHIFT_L   0x04
#define SHIFT_R   0x08
#define ALTGR     0x10



bool matrixKB::available()
{
    if (CharBuffer) return true;
    return false;
}

int matrixKB::read()
{
    uint8_t result;


    result = CharBuffer;
    if (result)
    {
        CharBuffer = 0;
    }

    if (!result) return -1;
    return result;
}


matrixKB::matrixKB()
{
    // nothing to do here, begin() does it all
}

void matrixKB::begin()
{
    cli();
    DDRB = 0xFF;
    PORTB = 0xFF;

    DDRC = 0x00;
    PORTC = 0b00001111; //0x0F;
    DDRD = 0x00;
    PORTD = 0b00111100; //0x0F;
    delay(10);


    TCCR2A = (1<<WGM21);    // Режим CTC (сброс по совпадению)
// TCCR2B = (1<<CS20);                     // Тактирование от CLK.
// TCCR2B = (1<<CS21);                     // CLK/8
// TCCR2B = (1<<CS20)|(1<<CS21);           // CLK/32
// TCCR2B = (1<<CS22);                     // CLK/64
// TCCR2B = (1<<CS20)|(1<<CS22);           // CLK/128
// TCCR2B = (1<<CS21)|(1<<CS22);           // CLK/256
    TCCR2B = (1<<CS20)|(1<<CS21)|(1<<CS22); // CLK/1024

    OCR2A = 255;            // Верхняя граница счета. Диапазон от 0 до 255.
    // Частота прерываний будет = Fclk/(N*(1+OCR2A))
    // где N - коэф. предделителя (1, 8, 32, 64, 128, 256 или 1024)
    TIMSK2 = (1<<OCIE2A);   // Разрешить прерывание по совпадению
    // получили примерно 150 раз в секунду
    sei();              // разрешаем прерывания (запрещаем: cli(); )
}



