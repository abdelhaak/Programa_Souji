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
        uint64_t  UltimoTiempoPulsado;
        const unsigned long TIEMPO_PULSO_LARGO = 2000; // 2 segundos
        uint64_t TiempoPulsado = 1000;
        uint64_t t_mantenido = 0;
        uint64_t t_pulsado = 0;
        uint16_t mantener = 2500;
        uint16_t mantenerDelay = 200;
        //uint16_t ultimoCambioTiempo;

    public:
        Boton(byte pin);
        void init();
        void update();
        bool pulsado();
        bool pulsadoLargo();
};

#endif