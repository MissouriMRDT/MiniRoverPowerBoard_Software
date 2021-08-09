#include <Servo.h>

Servo Servo1;

unsigned long int a,b,c;
int x[15],ch1[15],ch[7],i;
int mapval;
//specifing arrays and variables to store values 

void setup() {
  Serial.begin(9600);
  Servo1.attach(3);
  pinMode(3,OUTPUT);
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), read_me, FALLING);
  // enabling interrupt at pin 2
}

void loop() {
read_rc();

Serial.print(ch[1]);Serial.print("\t");
Serial.print(ch[2]);Serial.print("\t");
Serial.print(ch[3]);Serial.print("\t");
Serial.print(ch[4]);Serial.print("\t");
Serial.print(ch[5]);Serial.print("\t");
Serial.print(ch[6]);Serial.print("\n");


mapval = map(ch[2],72,892,0,180);

Serial.println(mapval);
Serial.print("MAPVAL");
Servo1.write(mapval);

if (ch[2] < 380){
  Serial.println("MOTOR 1 LOW");
  //Serial.println(ch[2]);
  //Serial.println(mapval);
  //Servo1.write(mapval);
}
if (ch[2] > 520){
  Serial.println("MOTOR 1 HIGH");
}
if (ch[4] < 380){
  Serial.println("MOTOR 2 LOW");
}
if (ch[4] > 520){
  Serial.println("MOTOR 2 HIGH");
}
delay(100);
}


void read_me()  {
 //this code reads value from RC reciever from PPM pin (Pin 2 or 3)
 //this code gives channel values from 0-1000 values 
 //    -: ABHILASH :-    //
a=micros(); //store time value a when pin value falling
c=a-b;      //calculating time inbetween two peaks
b=a;        // 
x[i]=c;     //storing 15 value in array
i=i+1;       if(i==15){for(int j=0;j<15;j++) {ch1[j]=x[j];}
             i=0;}}//copy store all values from temporary array another array after 15 reading  
void read_rc(){
int i,j,k=0;
  for(k=14;k>-1;k--){if(ch1[k]>10000){j=k;}}  //detecting separation space 10000us in that another array                     
  for(i=1;i<=6;i++){ch[i]=(ch1[i+j]-1000);}}     //assign 6 channel values after separation space
