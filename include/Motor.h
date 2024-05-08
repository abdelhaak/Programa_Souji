#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"

#define pinPot A2

class Motor
{
    private:
        uint8_t pin;
        uint8_t pinSensor;
        //uint8_t pinPot;
        unsigned long pulsos_ultimo_segundo;
        float frecuencia;
        int rpms;
        unsigned long tiempoInicio;
        static volatile unsigned long pulsos;
       
    public:
        Motor(uint8_t pin,uint8_t pinSensor);
        void init();
        void parar();
        static void contarPulsos();
        void ajustarRpms();
        float calcularFrecuencia();
        int calcularRpms();        
};


#endif
