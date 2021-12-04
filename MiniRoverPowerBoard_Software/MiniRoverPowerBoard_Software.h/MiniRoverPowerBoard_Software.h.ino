#include "DualVNH5019MotorShield.h"
#include <CuteBuzzerSounds.h>

DualVNH5019MotorShield md;

//helloworld

unsigned long int a, b, c;
int x[15], ch1[15], ch[7], i;

int buzz;    //this could be a bool but whatever
#define BUZZER_PIN 5
int mot1;
int mot2;
int motorball1;
int motorball2;
int mot1save;
int mot2save;
int posdead = 50;
int negdead = -50;

void setup() {

  cute.init(BUZZER_PIN);

  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();

  pinMode(5, OUTPUT);

  pinMode(3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), read_me, FALLING);

  delay(2000);

}

void loop() {

  // read reciever
  read_rc();


  Serial.print(ch[1]); Serial.print("\t");
  Serial.print(ch[2]); Serial.print("\t");
  Serial.print(ch[3]); Serial.print("\t");
  Serial.print(ch[4]); Serial.print("\t");
  Serial.print(ch[5]); Serial.print("\t");
  Serial.print(ch[6]); Serial.print("\n");

  buzz = map(ch[1], 88, 900, -2, 2 );


  mot1 = map(ch[2], 88, 900, -1200, 1200);
  mot2 = map(ch[4], 92, 920, -1200, 1200);
  Serial.print(mot1);
  Serial.print("mot1  ");
  Serial.print("    ");
  Serial.print(mot2);
  Serial.print("mot2  ");



  // motor 2
  if (mot2 < posdead && mot2 > negdead) {
    md.setM2Speed(0);
  }
  if ( mot2 > posdead) {

    md.setM2Speed(mot2);
    Serial.print("M2 current: ");
    Serial.print(md.getM2CurrentMilliamps());
    Serial.print("    ");
  }
  if ( mot2 < negdead) {

    md.setM2Speed(mot2);
    Serial.print("M2 current: ");
    Serial.print(md.getM2CurrentMilliamps());
    Serial.print("    ");
  }


  // motor 1
  if (mot1 < posdead && mot1 > negdead) {
    md.setM1Speed(0);
  }
  if ( mot1 > posdead) {

    md.setM1Speed(mot1);
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print("    ");
  }
  if ( mot1 < negdead) {

    md.setM1Speed(mot1);
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print("    ");
  }


 
  if (buzz == 2) {
    cute.play(S_JUMP);
  }
  if (buzz == -2) {
    cute.play(S_BUTTON_PUSHED);
  }



}

void read_me()  {
  //this code reads value from RC reciever from PPM pin (Pin 2 or 3)
  //this code gives channel values from 0-1000 values
  //    -: ABHILASH :-    //
  a = micros(); //store time value a when pin value falling
  c = a - b;  //calculating time inbetween two peaks
  b = a;      //
  x[i] = c;   //storing 15 value in array
  i = i + 1;       if (i == 15) {
    for (int j = 0; j < 15; j++) {
      ch1[j] = x[j];
    }
    i = 0;
  }
}//copy store all values from temporary array another array after 15 reading
void read_rc() {
  int t, j, k = 0;
  for (k = 14; k > -1; k--) {
    if (ch1[k] > 10000) {
      j = k; //detecting separation space 10000us in that another array
    }
  }
  for (t = 1; t <= 6; t++) {
    ch[t] = (ch1[t + j] - 1000); //assign 6 channel values after separation space
  }
}
