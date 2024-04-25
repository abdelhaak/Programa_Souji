#include <Arduino.h>
#include "Boton.h"

Boton::Boton(byte pin)
{
    this->pin = pin;
    estado = true;
    UltimaLectura = true;
    UltimoTiempoPulsado = 0;
    init();
}

void Boton::init()
{
    pinMode(pin, INPUT);
    update();
}

void Boton::update()
{
    estado = digitalRead(pin);
    if(estado && !UltimaLectura && (millis() - UltimaLectura > UltimoTiempoPulsado))
    {
        pulso = true;
        UltimaLectura = millis();
    }
    else if (UltimaLectura)
    {
        pulso = false;
    }
    UltimaLectura = estado;
}

bool Boton::pulsado()
{
    update();
    return pulso;
}

