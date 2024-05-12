#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include "Arduino.h"
#include "Motor.h"
#include "Bombas.h"

#define PIN_MOTOR A3
#define PIN_SENSOR A2

#define PIN_BOMBA_ACEITE 
#define PIN_BOMBA_SOUJI 
#define PIN_BOMBA_AGUA  
#define PIN_BOMBA_VACIO 


class Mezclas
{
    private:


    public:
        Mezclas();
        void init();
        void parado();

};











#endif