#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include "Arduino.h"
#include "Motor.h"
#include "Bombas.h"
#include "Bascula.h"
#include <LiquidCrystal.h>


#define PIN_MOTOR 53
#define PIN_SENSOR 52

#define PIN_BOMBA_ACEITE 51
#define PIN_BOMBA_SOUJI 50
#define PIN_BOMBA_AGUA 49
#define PIN_BOMBA_VACIO 48

// Capacidad total de la mezcla que seria de 5000 g que son 5L
#define CAPACIDAD_TOTAL 100  
#define DENSIDAD_ACEITE 0.92
#define DENSIDAD_SOUJI 0.92
#define RPMS_INICIO 1500



class Mezclas
{
    private:
        //// Aceite 
        
        float pesoAceiteDeseado = 0;
        float volumenAceite = 0.0;

        //// Souji
        float pesoSoujiDeseado = 0;

        //// Agua
        float pesoAguaDeseado = 0;

        //// Mezclas
        float pesoAgregado = 0.0;

    public:
        Mezclas();
        void init();
        void parado();
        void mezclaGeneral();



        
};











#endif