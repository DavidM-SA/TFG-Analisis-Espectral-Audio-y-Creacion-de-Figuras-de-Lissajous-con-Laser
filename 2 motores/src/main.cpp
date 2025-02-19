#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#define POT1 A0  // Potenciómetro 1
#define POT2 A1  // Potenciómetro 2
#define MOTOR1 9 // Pin de salida para motor 1 (Timer1)
#define MOTOR2 5 // Pin de salida para motor 2 (Timer3)
#define MicroStep 3

const int freqMin = 10;    // Frecuencia mínima en Hz
const int freqMax = 5000;  // Frecuencia máxima en Hz

volatile uint16_t time1 = 500;  // Período en microsegundos
volatile uint16_t time2 = 500;  
volatile bool state1 = false; // Estado de la señal
volatile bool state2 = false; 

void setup() {
    Serial.begin(115200);

    pinMode(MOTOR1, OUTPUT);
    pinMode(MOTOR2, OUTPUT);
    pinMode(MicroStep, OUTPUT);

    digitalWrite(MicroStep, HIGH); //Ponemos el driver en modo micropaso de 1/16

    cli(); // Desactivar interrupciones mientras configuramos

    // Configurar Timer1 (16 bits)
    TCCR1A = 0;  // Modo normal
    TCCR1B = (1 << WGM12) | (1 << CS11); // CTC, Prescaler de 8
    OCR1A = 500; // Valor inicial de comparación
    TIMSK1 |= (1 << OCIE1A); // TIMSK1 (Timer Interrupt Mask Register 1) y OCIE1A (Output Compare Interrupt Enable 1A)

    // Configurar Timer3 (16 bits)
    TCCR3A = 0;  
    TCCR3B = (1 << WGM32) | (1 << CS31); // CTC, Prescaler de 8
    OCR3A = 500;  
    TIMSK3 |= (1 << OCIE3A); // Habilitar interrupción por comparación OCIE3A

    sei(); // Habilitar interrupciones
}

// Interrupción Timer1 (Motor 1)
ISR(TIMER1_COMPA_vect) {
    state1 = !state1;
    digitalWrite(MOTOR1, state1);
    OCR1A = time1; // Actualizar intervalo
}

// Interrupción Timer3 (Motor 2)
ISR(TIMER3_COMPA_vect) {
    state2 = !state2;
    digitalWrite(MOTOR2, state2);
    OCR3A = time2; 
}

void loop() {
    // Leer potenciómetros
    int potValue1 = analogRead(POT1);
    int potValue2 = analogRead(POT2);

    // Mapear a frecuencias deseadas
    int freq1 = map(potValue1, 0, 1023, freqMin, freqMax);
    int freq2 = map(potValue2, 0, 1023, freqMin, freqMax);

    // Calcular intervalo en ticks (1 tick = 0.5us con prescaler de 8)
    time1 = (1000000 / (2*freq1)) -1;
    time2 = (1000000 / (2*freq2)) -1;

    Serial.print("Freq1: "); Serial.print(freq1); Serial.print(" Hz, ");
    Serial.print("Freq2: "); Serial.print(freq2); Serial.println(" Hz");
  }