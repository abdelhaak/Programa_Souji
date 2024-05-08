#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include "Arduino.h"
#include "Motor.h"
#include "Bombas.h"



#define PIN_MOTOR 10
#define PIN_SENSOR 13

#define PIN_BOMBA_ACEITE 40
#define PIN_BOMBA_SOUJI 41
#define PIN_BOMBA_AGUA 42 
#define PIN_BOMBA_VACIO 43


class Mezclas
{
    private:


    public:
        Mezclas();
        void init();
        void parado();
        void vacioOn();
        void vacioOff();

};











#endif