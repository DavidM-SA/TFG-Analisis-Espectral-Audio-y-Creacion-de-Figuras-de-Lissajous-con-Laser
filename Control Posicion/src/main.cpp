#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Definición de pines para los motores
#define MOTOR1 9 // Pin de salida para motor 1 (Timer1)
#define MOTOR2 5 // Pin de salida para motor 2 (Timer3)
#define MicroStep 3

// Rango de frecuencias permitidas en Hz
const int freqMin = 150;    // Frecuencia mínima en Hz
const int freqMax = 2000;   // Frecuencia máxima en Hz

// Variables globales de frecuencia (se almacenan en microsegundos)
volatile int time1 = 600;  // Frecuencia para motor 1
volatile int time2 = 650;  // Frecuencia para motor 2

// Frecuencias deseadas para los motores
int frecuenciaMotor1 = 600;  // Frecuencia para motor 1 (Hz)
int frecuenciaMotor2 = 650;  // Frecuencia para motor 2 (Hz)

volatile bool state1 = false; // Estado de la señal para motor 1
volatile bool state2 = false; // Estado de la señal para motor 2

unsigned long startMillis = 0; // Variable para almacenar el tiempo de inicio
const unsigned long duration = 5000; // Duración en milisegundos (5 segundos)

void setup() {
    Serial.begin(115200);
    pinMode(MOTOR1, OUTPUT);
    pinMode(MOTOR2, OUTPUT);
    pinMode(MicroStep, OUTPUT);

    digitalWrite(MicroStep, HIGH); // Ponemos el driver en modo micropaso de 1/16

    cli(); // Desactivar interrupciones mientras configuramos

    // Configurar Timer1 (16 bits) para Motor 1
    TCCR1A = 0;  // Modo normal
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC, Prescaler de 8
    OCR1A = 500; // Valor inicial de comparación
    TIMSK1 |= (1 << OCIE1A); // Habilitar interrupción Timer1

    // Configurar Timer3 (16 bits) para Motor 2
    TCCR3A = 0;  
    TCCR3B = (1 << WGM32) | (1 << CS31); // CTC, Prescaler de 8
    OCR3A = 500;  
    TIMSK3 |= (1 << OCIE3A); // Habilitar interrupción Timer3

    sei(); // Habilitar interrupciones

    startMillis = millis(); // Guardar el tiempo de inicio
}

ISR(TIMER1_COMPA_vect) {
    state1 = !state1;
    digitalWrite(MOTOR1, state1);
    OCR1A = time1; // Actualizar intervalo para motor 1
}

ISR(TIMER3_COMPA_vect) {
    state2 = !state2;
    digitalWrite(MOTOR2, state2);
    OCR3A = time2; // Actualizar intervalo para motor 2
}

void loop() {
    // Verificar si han pasado 5 segundos
    if (millis() - startMillis < duration) {
        // Calcular el intervalo de tiempo para el motor 1 (en microsegundos)
        if (frecuenciaMotor1 >= freqMin && frecuenciaMotor1 <= freqMax) {
            noInterrupts();
            time1 = (2000000 / (2 * frecuenciaMotor1)) - 1; // Formula para calcular el tiempo entre pulsos
            interrupts();
        }

        // Calcular el intervalo de tiempo para el motor 2 (en microsegundos)
        if (frecuenciaMotor2 >= freqMin && frecuenciaMotor2 <= freqMax) {
            noInterrupts();
            time2 = (2000000 / (2 * frecuenciaMotor2)) - 1; // Formula para calcular el tiempo entre pulsos
            interrupts();
        }
    } else {
        // Detener los motores después de 5 segundos
        TIMSK1 &= ~(1 << OCIE1A);  // Deshabilita la interrupción de comparación A del Timer1
        TIMSK3 &= ~(1 << OCIE3A);  // Deshabilita la interrupción de comparación A del Timer3
    }
}
