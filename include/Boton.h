#ifndef MI_BOTON_H
#define MI_BOTON_H

#include <Arduino.h>

// Definición de Botones
const uint8_t PIN_BOTON_SET = 21;
const uint8_t PIN_BOTON_UP = 20;
const uint8_t PIN_BOTON_DOWN = 19;
const uint8_t PIN_BOTON_SEL = 18;
const uint8_t PIN_BOTON_PRO = 14;

class Boton
{
    private:
        byte pin;
        bool estado;
        bool UltimaLectura;
        bool pulso;
        bool pulsado_largo;
        uint64_t  ultimoTiempoPulsado;
        const unsigned long TIEMPO_PULSO_LARGO = 2000; // 2 segundos

    public:
        Boton(byte pin);
        void init();
        void update();
        bool pulsado();
};

#endif