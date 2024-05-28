#ifndef BOMBAS_H
#define BOMBAS_H

#include "Arduino.h"
class Bomba
{
    private: 
        byte pin;

    public:
        Bomba(byte pin);
        void init();
        void on();
        void off();
};

#endif