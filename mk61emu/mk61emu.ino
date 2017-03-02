/*  (c0,c1,c2,c3) + (d2,d3,d4,d5)
     (b0,b1,b2,b3,b4)

#define  NUM_ROWS  8
#define  NUM_COLS  5
byte rowPins[NUM_ROWS] = {A0, A1, A2, A3, 2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[NUM_COLS] = {8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad

В данном скетче сканирование идет по столбцам, то есть на них подается сигнал, а снимается со строк.
Значит диоды ставятся на пины, подключенные к столбцам, чтобы исключить КЗ между столбцами с выставленным 0 и 1,
а резисторы ограничивают ток на читающих пинах.


                                     
                                     D8 D9 D10 D11 D12
                                                      
                                      |  |  |  |  |   
                                      d  d  d  d  d   
                                      1  2  3  4  5   
                                      |  |  |  |  |   
                   +==================+==+==+==+==+====+
          R1 _     |                . |  |  |  |  |    |   _ r8
   A3   ----|_|----+--1--2--3--4--5 . 6--7--8--9--0----+--|_|---   D2
          r2 _     |  |  |  |  |  | . |  |  |  |  |    |   _ r7
   A4   ----|_|----+--Q--W--E--R--T . Y--U--I--O--P----+--|_|---   D3
          r3 _     |  |  |  |  |  | . |  |  |  |  |    |   _ r6
   A5   ----|_|----+--A--S--D--F--G . H--J--K--L--ENTER+--|_|---   D4
          r4 _     |  |  |  |  |  | . |  |  |  |  |    |   _ r5
   A6   ----|_|----+--^--Z--X--C--V . B--N--M--#--' '--+--|_|---   D5
                   +==+==+==+==+==+===+==+==+==+==+====+
                      |  |  |  |  |___|  |  |  |  |
                      |  |  |  |_________|  |  |  |
                      |  |  |_______________|  |  |
                      |  |_____________________|  |
                      |___________________________|
                       ____ ____
                       |   U   |          _______________________ 
              reset----|1    28|--A5--scl+    LCD 16x2          |
              rx-------|2    27|--A4--sda+______________________|
              tx-------|3    26|-------------------K1-строки 
     строки   K2-------|4  a 25|-------------------K1-левой 
     левой    K2-------|5  t 24|-------------------K1-половины 
     половины K2-------|6  m 23|-------------------K1-клавиатуры
                   vcc-|7  e 22|-gnd                  
                   gnd-|8  g 21|-
                      -|9  a 20|-vcc
                      -|10 8 19|-
  клавиатуры  K2-------|11   18|---------K3 матрицы
                      -|12   17|---------K3 клавиатуры
                      -|13   16|---------K3
      столбци K3-------|14   15|---------K3
                       |_______| 
                       
*/

/*
   пока используем atmega8

   клавиатура на основе mk52
   F   ШГ>  П->X   7   8   9   -   /
   K   ШГ<  X->П   4   5   6   +
   ||  В/О  БП     1   2   3   <-> В|
   А|  С/П  ПП     0   .   /-/ ВП  Cx
*/
#include "scancode_dc.h"
#define MEMSIZE 105
uint8_t mem [MEMSIZE];
uint8_t pp = 0; //указатель програмной памяти (адрес выполняемой команды)
uint8_t mode = 0; //0 - интерактивный режим, 1- выполнение программы, 2 - ввод программы
uint8_t lastCmd; //последняя принятая с клавиатуры команда
uint8_t cmdReady = 0; //признак, что пришла команда с клавиатуры :0=нет


char screenX[16] = " 0               ";
uint8_t scP = 1; // указатель на текущую позицию экранного представления X. 0-разряд знака, 1-первый значащий разряд
char digits[12] = "0123456789.-";
float x = 0;
float y = 0;
float z = 0;
float t = 0;
float x1 = 0;

void sPush()//втолкнуть значение X в стек c потерей T (стрелка вверх)
{
  t = z;
  z = y;
  y = x;
  x1 = x; // ?
}
void sCycle(float p) // круговое движение стека назад
{
  x1 = x;
  x = y;
  y = z;
  z = t;
  t = x1;
}

void sOp(uint8_t op) // выполнить операцию со стеком
{ float resOp;
  switch (op)
  { case op_Add:
      resOp = x + y;
      break;
    case op_Sub:
      resOp = x - y;
      break;
    case op_Mul:
      resOp = x * y;
      break;
    case op_Div:
      resOp = x / y;
      break;

  }
  // спуск стека вниз, съедание регистра Y. X=результат операции
  x1 = x;
  x = resOp;
  y = z;
  z = t;
}

void addDigitToX(uint8_t d) {
  if (d > 11 ) return; // 0123456789./-/
  else if (d < 11 ) {
    screenX[scP++] = digits[d];
    screenX[scP] = 0;
  }
  else screenX[0] = '-'; // меняем знак числа в X
  //дальше - синхронизировать X и screenX
  x = atof(screenX);
}

void setup() {
  Serial.begin(9600);
  pinMode(8, OUTPUT);  /* or use:    DDRB = DDRB | B00000001;  // this sets pin8 as output without changing the value of the other pins  */
  DDRB = 0xFF;  // инициализировать клавиатуру
  PORTB = 0xFF;
  DDRC = 0x00;
  PORTC = 0b00001111; //0x0F;
  DDRD = 0x00;
  PORTD = 0b00111100; //0x0F;
  delay(10);
  cli();      // Disable interrupts while loading registers
  // Set the registers
  TCCR1A = 0; //Timer Counter Control register
  // Set mode
  TCCR1B = (1 << WGM12); // turn on CTC mode
  // Set prescale values (1024). (Could be done in same statement as setting the WGM12 bit.)
  TCCR1B |= (1 << CS12) | (1 << CS10);
  //Enable timer compare interrupt===> TIMSK1 for ATmega328,
  //TIMSK for ATmega8
  TIMSK |= (1 << OCIE1A);
  // Set OCR1A
  //OCR1A = 15624;
  OCR1A = 4092;
  sei();    // Enable global interrupts
  
}

ISR (TIMER1_COMPA_vect) {
  digitalWrite(8, !digitalRead(8));
  //PORTB ^= _BV(PB0); // as digitalWrite(8,x) is an Arduino
  //function, direct writing to the port may be preferable
}

void loop() {//0 - интерактивный режим, 1- выполнение программы, 2 - ввод программы
  if (cmdReady) {
    if (mode == 0 ) //интерактивный режим работы
    {
      execOne(lastCmd);
    }
    else if (mode = 2 ) 
    {
      storeOne(lastCmd);// сохранить команду в памяти
    }
    else if (mode = 3 ) 
    {
      runner();// сохранить команду в памяти
    }

    show();// отобразить результат
    cmdReady=0;
  }
}

uint8_t runner() { // записать в програмную память одну команду программы
while (pp<MEMSIZE){
  
  execOne(mem[pp++]);
}
}

uint8_t storeOne(uint8_t op) { // записать в програмную память одну команду программы
  if (op<0xff)//заглушка для проверки, что команда подходит для режима программирования
  {mem[pp++]=op;}
}
uint8_t execOne(uint8_t op) { // выполнить одну команду программы
uint8_t ret=0;
  switch (op) {
    case 0x00:
    case 0x01:
    case 0x02:
    case 0x03:
    case 0x04:
    case 0x05:
    case 0x06:
    case 0x07:
    case 0x08:
    case 0x09:
    case 0x0a: // .
    case 0x0b: // /-/
      addDigitToX(op);
      break;
    case 0x0c: // Вп - ввод порядка числа
      { 
      }
      break;
    case 0x0d:
      x = 0;
      break;
    case 0x0e:
      sPush();

    default:
      {Serial.println("Unavailable yet!");ret=1;}
      break;
  }
return ret;
}

void show(){
  serialShowStatus();
  
  }

void serialShowStatus() {
  Serial.println("Stack");
  Serial.println(" T=" + String(t));
  Serial.println(" Z=" + String(z));
  Serial.println(" Y=" + String(y));
  Serial.println(" X=" + String(x));
  Serial.println("x1=" + String(x1));
  Serial.println("screenX=" + String(screenX));
  Serial.println("PROG");
  Serial.println("Mode=" + String(mode));
  Serial.println("->" + String(pp));
}

void test() {
  addDigitToX(1); serialShowStatus();
  addDigitToX(2); serialShowStatus();
  addDigitToX(3); serialShowStatus();
  addDigitToX(4); serialShowStatus();
  addDigitToX(10); serialShowStatus();
  addDigitToX(6); serialShowStatus();
  addDigitToX(11); serialShowStatus();
}

