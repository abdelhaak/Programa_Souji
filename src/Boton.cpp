#include <Arduino.h>
#include "Boton.h"

Boton::Boton(byte pin)
{
    this->pin = pin;
    estado = true;
    UltimaLectura = false;
    UltimoTiempoPulsado = 0;
    pulso = false;
    pulsado_largo = false ;
    init();
}

void Boton::init()
{
    pinMode(pin, INPUT);
    update();
}

void Boton::update()
{
    bool lecturaActual = digitalRead(pin);
    unsigned long tiempoActual = millis();

    if (lecturaActual && !UltimaLectura) 
    {
        UltimoTiempoPulsado = tiempoActual;
    }
    
    if (!lecturaActual && UltimaLectura) 
    {
        if ((tiempoActual - UltimoTiempoPulsado) < TIEMPO_PULSO_LARGO) 
        {
            pulso = true;
        }
        else 
        {
            pulso = false;
        }
        pulsado_largo = false;
    }

    if (lecturaActual && UltimaLectura && (tiempoActual - UltimoTiempoPulsado >= TIEMPO_PULSO_LARGO)) 
    {
        pulsado_largo = true;
        pulso = false;
    }
    UltimaLectura = lecturaActual;
}

bool Boton::pulsado()
{
    update();
    bool resultado = pulso;
    if (pulso) pulso = false;
    return resultado;
}

bool Boton::pulsadoLargo()
{
    update();
    bool resultado = pulsado_largo;
    if (pulsado_largo) pulsado_largo = false;
    return resultado;
}