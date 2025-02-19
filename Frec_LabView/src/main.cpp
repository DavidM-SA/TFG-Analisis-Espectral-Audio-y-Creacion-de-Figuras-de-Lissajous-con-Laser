#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

// Función para leer frecuencias desde LabVIEW
void leerFrecuencias(volatile int* freq1, volatile int* freq2);

// Definición de pines para los motores
#define MOTOR1 9 // Pin de salida para motor 1 (Timer1)
#define MOTOR2 5 // Pin de salida para motor 2 (Timer3)
#define MicroStep 3

// Rango de frecuencias permitidas en Hz
const int freqMin = 10;    // Frecuencia mínima en Hz
const int freqMax = 5000;  // Frecuencia máxima en Hz

// Variables globales de frecuencia (se almacenan en microsegundos)
volatile int frecuency1 = 500;  
volatile int frecuency2 = 500;  
volatile bool state1 = false; // Estado de la señal para motor 1
volatile bool state2 = false; // Estado de la señal para motor 2

String str1, str2; // Variables para almacenar los datos recibidos por Serial


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
    OCR1A = frecuency1; // Actualizar intervalo
}

// Interrupción Timer3 (Motor 2)
ISR(TIMER3_COMPA_vect) {
    state2 = !state2;
    digitalWrite(MOTOR2, state2);
    OCR3A = frecuency2; 
}


void loop() {
    leerFrecuencias(&frecuency1, &frecuency2);
}

void leerFrecuencias(volatile int* freq1, volatile int* freq2) {
  if (Serial.available() > 0) {  
        String input = Serial.readStringUntil('\n');  // Lee hasta encontrar '\n'
        int separatorIndex = input.indexOf(',');      // Encuentra la coma (delimitador)

        if (separatorIndex != -1) {
            str1 = input.substring(0, separatorIndex);   // Extrae la primera frecuencia
            str2 = input.substring(separatorIndex + 1);  // Extrae la segunda freciemcia
        }
      int f1 =str1.toInt();
      int f2 = str2.toInt();

      // Validar rangos y actualizar punteros
      if (f1 >= freqMin && f1 <= freqMax) {
          *freq1 = (1000000 / (2*f1)) - 1;
      }
      if (f2 >= freqMin && f2 <= freqMax) {
          *freq2 = (1000000 / (2*f2)) - 1;
      }

      // Enviar confirmación a LabVIEW
      Serial.print("Set Freq1: "); Serial.print(f1); Serial.print(" Hz, ");
      Serial.print("Set Freq2: "); Serial.print(f2); Serial.println(" Hz");
  }
}
