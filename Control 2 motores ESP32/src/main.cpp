#include <Arduino.h>
#include "driver/timer.h"

#define POT1 34  // Potenciómetro 1
#define POT2 35  // Potenciómetro 2
#define MOTOR1 32 // Salida motor 1
#define MOTOR2 33 // Salida motor 2

const int freqMin = 10;   // Frecuencia mínima en Hz
const int freqMax = 10000; // Frecuencia máxima en Hz

volatile int interval1 = 500; // Período en microsegundos (inicial)
volatile int interval2 = 500; 

volatile bool state1 = false; // Estado de la señal para el Timer1
volatile bool state2 = false; // Estado de la señal para el Timer2

hw_timer_t *timer1 = NULL;
hw_timer_t *timer2 = NULL;

// Interrupción del Timer 1
void IRAM_ATTR onTimer1() {
    state1 = !state1; 
    digitalWrite(MOTOR1, state1);
    timerAlarmWrite(timer1, interval1, true);
}

// Interrupción del Timer 2
void IRAM_ATTR onTimer2() {
    state2 = !state2; 
    digitalWrite(MOTOR2, state2);
    timerAlarmWrite(timer2, interval2, true);
}

void setup() {
    Serial.begin(115200);
    
    pinMode(MOTOR1, OUTPUT);
    pinMode(MOTOR2, OUTPUT);

    // Configurar Timer 1
    timer1 = timerBegin(0, 80, true); // Timer 0, divisor de 80 → 1 tick = 1 µs
    timerAttachInterrupt(timer1, &onTimer1, true);
    timerAlarmWrite(timer1, interval1, true);
    timerAlarmEnable(timer1);

    // Configurar Timer 2
    timer2 = timerBegin(1, 80, true); // Timer 1, divisor de 80 → 1 tick = 1 µs
    timerAttachInterrupt(timer2, &onTimer2, true);
    timerAlarmWrite(timer2, interval2, true);
    timerAlarmEnable(timer2);
}

void loop() {
    // Leer potenciómetros
    int potValue1 = analogRead(POT1);
    int potValue2 = analogRead(POT2);

    // Mapear a frecuencias deseadas
    int freq1 = map(potValue1, 0, 4095, freqMin, freqMax);
    int freq2 = map(potValue2, 0, 4095, freqMin, freqMax);

    // Calcular intervalo en microsegundos (T = 1/f * 1,000,000)
    interval1 = (1000000 / freq1) / 2;
    interval2 = (1000000 / freq2) / 2;
}