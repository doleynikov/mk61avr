/*
 * пока используем atmega8
 * 
 * клавиатура
 * F   ШГ>  П->X   7   8   9   -   /
 * K   ШГ<  X->П   4   5   6   +   *
 * ||  В/О  БП     1   2   3   <-> В|
 * А|  С/П  ПП     0   .   /-/ ВП  Cx 
 */


uint8_t mem [105];
uint8_t pp = 0; //указатель програмной памяти (адрес выполняемой команды)
uint8_t mode = 0; //0 - интерактивный режим, 1- выполнение программы, 2 - ввод программы


char screenX[16]=" 0               ";
uint8_t scP=1;// указатель на текущую позицию экранного представления X. 0-разряд знака, 1-первый значащий разряд
char digits[12]="0123456789.-";
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
//  x = p;
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
{ switch (op)
  { case '+':
      sOpDoXY(x+y);
      break;
    case '-':
      sOpDoXY(x-y);
      break;
  }

}

void sOpDoXY(float nx)// спуск стека вниз, съедание регистра Y
// в X - переданное значение
{
        x1 = x;
        x = nx;
        y = z;
        z = t;
}

void addDigitToX(uint8_t d) {
if (d >11 ) return; // 0123456789./-/
else if (d <11 ) {screenX[scP++]=digits[d];screenX[scP]=0; }
else screenX[0]='-'; // меняем знак числа в X
 //дальше - синхронизировать X и screenX
x= atof(screenX);
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

void test(){
  addDigitToX(1);serialShowStatus();
  addDigitToX(2);serialShowStatus();
  addDigitToX(3);serialShowStatus();
  addDigitToX(4);serialShowStatus();
  addDigitToX(10);serialShowStatus();
  addDigitToX(6);serialShowStatus();
  addDigitToX(11);serialShowStatus();
}
void setup() {
  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(8,OUTPUT);
/* or use:
DDRB = DDRB | B00000001;  // this sets pin 8  as output
                       // without changing the value of the other pins 
*/
// Disable interrupts while loading registers
cli();
// Set the registers
TCCR1A = 0; //Timer Counter Control register
// Set mode
TCCR1B = (1 << WGM12); // turn on CTC mode
// Set prescale values (1024). (Could be done in same statement
// as setting the WGM12 bit.)
TCCR1B |= (1 << CS12) | (1 << CS10);
//Enable timer compare interrupt===> TIMSK1 for ATmega328, 
//TIMSK for ATmega8
TIMSK |= (1 << OCIE1A);
// Set OCR1A
//OCR1A = 15624;
OCR1A = 4092;

// Enable global interrupts
sei();
}

ISR (TIMER1_COMPA_vect) {
   digitalWrite(8, !digitalRead(8));
   //PORTB ^= _BV(PB0); // as digitalWrite(8,x) is an Arduino 
   //function, direct writing to the port may be preferable
}

void loop() {
  // put your main code here, to run repeatedly:
    Serial.println("==================");
    Serial.println(Serial.read());
}

void mode2(){
  // ввод программы: все коды, кроме управляющих программированием, помещаются в програмную память.
}

#define F_PRG  2

uint8_t doCmd(uint8_t op){ // выполнить одну команду интерактивно
  switch (op) {
    case F_PRG: // переход в режим программирования
      {mode=2; mode2();//сюда вернемся после выхода из режима программирования
      }
  
}
}

uint8_t doOne(uint8_t op){  // выполнить одну команду программы

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
      {mode=2; mode2();//сюда вернемся после выхода из режима программирования
      }
      break;
    case 0x0d:
      x=0;
      break;
    case 0x0e:
      sPush();  
      
    default:
Serial.println("Unavailable yet!");
break;
  }

}

