#include "Motor.h"
//#include "Arduino.h"

volatile uint64_t Motor::pulsos = 0;

Motor::Motor(uint8_t pin,uint8_t pinSensor)
{
    this->pin = pin;
    tiempoInicio = 0;
    frecuencia = 0 ;
    rpms = 0 ;
    pulsos = 0;
    pulsos_ultimo_segundo = 0 ;
    init();
}

void Motor::init()
{
    //attachInterrupt(digitalPinToInterrupt(pinSensor), contarPulsos, RISING); // Adjunta la función de interrupción al pin para contar los pulsos
    attachInterrupt(digitalPinToInterrupt(pinSensor), contarPulsos, RISING); 

    // Funcion para parar las bombas o el motors
    parar();
}

void Motor::parar()
{
    digitalWrite(pin, false);
}        

void Motor::contarPulsos()
{
    pulsos++;
}
        
void Motor::ajustarRpms(int rpms, uint64_t tiempoMezcla)
{
    int valorPwm = map(rpms,0,3300,0,255); 
    analogWrite(pin,valorPwm);
    uint64_t tiempoInicio = millis();
    while(millis() - tiempoInicio < tiempoMezcla)
    {
        delay(500);
    }
    parar();
}
        
float Motor::calcularFrecuencia()
{
  tiempoInicio = millis(); // Registra el tiempo de inicio
  if (millis() - tiempoInicio >= 1000) 
  { // Verifica si ha pasado 1 segu ndo
    detachInterrupt(digitalPinToInterrupt(pinSensor)); // Detiene las interrupciones
    pulsos_ultimo_segundo = pulsos; // Lee el contador de pulsos
    pulsos = 0; // Reinicia el contador de pulsos
    attachInterrupt(digitalPinToInterrupt(pinSensor), contarPulsos, RISING); // Vuelve a adjuntar la función de interrupción
    tiempoInicio = millis(); // Reinicia el tiempo de inicio

    // Calcula la frecuencia en Hz (pulsos por segundo)
    frecuencia = pulsos_ultimo_segundo / 1.0;
    //rpms = abs((int(frecuencia)*60)/600);

    // Imprime la frecuencia medida
    Serial.print("Frecuencia: ");
    Serial.print(frecuencia);
    Serial.print(" Hz  ||  ");
    Serial.print("RPMS : ");
    Serial.println(rpms);
  }
  return frecuencia;
}

int Motor::calcularRpms()
{
    rpms = abs((int(frecuencia)*60)/600);
    Serial.print(" Hz  ||  ");
    Serial.print("RPMS : ");
    Serial.println(rpms);
    return rpms;
}