#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"

class Motor
{
    private:
       uint8_t pin;
       uint8_t pinPot;
       uint8_t PinSensor;
       unsigned long tiempoInicio;
       volatile unsigned long pulsos;

    public:
        Motor(uint8_t pin);
        void init();
        void contarPulsos();
        void ajustarRpms(uint8_t pin);
        void calcularRpms(uint8_t pin);
        
       
};




#endif
