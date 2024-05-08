#include "Bombas.h"
#include "Arduino.h"


Bomba::Bomba(byte pin)
{
    this -> pin = pin ;
    init();
};


void Bomba::init()
{
    pinMode(pin, OUTPUT);
    off();
}

void Bomba::on()
{
    digitalWrite(pin,true);
}

void Bomba::off()
{
    digitalWrite(pin,false);
}
 

