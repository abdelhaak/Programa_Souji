#ifndef MI_BOTON_H
#define MI_BOTON_H

#include <Arduino.h>

class Boton
{
    private:
        byte pin;
        bool estado;
        bool UltimaLectura;
        bool pulso;
        uint64_t  UltimoTiempoPulsado;
        uint64_t TiempoPulsado = 1000;
        uint64_t t_mantenido = 0;
        uint64_t t_pulsado = 0;
        uint16_t mantener = 2500;
        uint16_t mantenerDelay = 200;

    public:
        Boton(byte pin);
        void init();
        void update();
        bool pulsado();
};

#endif