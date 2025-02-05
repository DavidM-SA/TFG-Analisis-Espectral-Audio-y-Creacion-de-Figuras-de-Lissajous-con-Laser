#include <Arduino.h>
int stepPin = 3; 
int dirPin = 2;
int pot = A0;
int maxtime =3000;
int pasos = 1; 
int usDelay = 500; // mínimo es 950 para modo paso completo y motor NEMA15
// mínimo es 35 para el modo decimosexto paso
float velocidad (int analogPin, int maxtime);

void setup() { 
Serial.begin(115200); 
pinMode(stepPin, OUTPUT); 
pinMode(dirPin, OUTPUT);
pinMode(pot, INPUT);
}

void loop() {
//digitalWrite(dirPin, HIGH); // dirección motor cw 
//for(int x = 0; x < pasos; x++) {
usDelay=velocidad(pot,maxtime);
Serial.println(usDelay);
digitalWrite(stepPin, HIGH); 
delayMicroseconds(usDelay); 
digitalWrite(stepPin, LOW); 
delayMicroseconds(usDelay);
//}
/*digitalWrite(dirPin, LOW); // dirección del motor 
for(int x = 0; x < pasos; x++) {
digitalWrite(stepPin, HIGH); 
delayMicroseconds(usDelay); 
digitalWrite(stepPin, LOW); 
delayMicroseconds(usDelay);
}*/
}
float velocidad (int analogPin, int maxtime){
  float time = (analogRead(analogPin)*(maxtime/1023.0));
  return time;
}