double x=0;
double y=0;
double z=0;
double t=0;
double x1=0;

void sPush(double p)//втолкнуть значение в стек
{
  t=z;
  z=y;
  y=x;
  x1=x;
  x=p;
  }
double sPushCycle(double p)
{ 
  x1=x;
  x=y;
  y=z;
  z=t;
  t=x1
  }

  




void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
