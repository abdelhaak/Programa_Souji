#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "LCD_Progress.h"

#define pinPot A2

class Motor
{
    private:
        uint8_t pin;
        uint8_t pinSensor;
        //uint8_t pinPot;
        uint64_t pulsos_ultimo_segundo;
        float frecuencia;
        int rpms;
        uint64_t tiempoInicio;
        static volatile uint64_t pulsos;
       
    public:
        Motor(uint8_t pin,uint8_t pinSensor);
        void init();
        void parar();
        static void contarPulsos();
        void ajustarRpms(int rpms,uint64_t tiempoMezcla);
        float calcularFrecuencia();
        int calcularRpms();        
};


#endif
