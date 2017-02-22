uint8_t mem [105];
uint8_t pp = 0; //указатель програмной памяти (адрес выполняемой команды)
uint8_t mode = 0; //0 - интерактивный режим, 1- выполнение программы, 2 - ввод программы


char[16] screenX=" 0               ";
uint8_t scP=1;// указатель на текущую позицию экранного представления X. 0-разряд знака, 1-первый значащий разряд
char[10] digits="0123456789."
double x = 0;
double y = 0;
double z = 0;
double t = 0;
double x1 = 0;

void sPush(double p)//втолкнуть значение в стек
{
  t = z;
  z = y;
  y = x;
  x1 = x;
  x = p;
}
void sCycle(double p) // круговое движение стека назад
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

void sOpDoXY(double nx)// спуск стека вниз, съедание регистра Y
{
        x1 = x;
        x = nx;
        y = z;
        z = t;
}


void addDigitToX(uint8_t d) {
if (d > 10) return;
 screenX[scP++]=digits[d]; 
 
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
                 Serial.println("Mode=" + String(mode))
                 Serial.println("->" + String(pp));
}


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
