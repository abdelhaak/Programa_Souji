#include "Mezclas.h"
#include "Arduino.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,PIN_SENSOR);



Mezclas::Mezclas()
{
    init();
}


void Mezclas::init()
{
    parado();
}

void Mezclas::parado()
{

}

