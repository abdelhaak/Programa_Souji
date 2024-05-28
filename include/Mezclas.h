#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include "Arduino.h"
#include "Motor.h"
#include "Bombas.h"
#include "Bascula.h"
#include "LCD_Progress.h"
#include "Menus.h"
#include "EEPROM.h"

#define PIN_MOTOR 53
#define PIN_SENSOR 52

#define PIN_BOMBA_ACEITE 51
#define PIN_BOMBA_SOUJI 50
#define PIN_BOMBA_AGUA 49
#define PIN_BOMBA_VACIO 48

// Capacidad total de la mezcla que seria de 5000 g que son 5L
#define CAPACIDAD_TOTAL 500  
#define DENSIDAD_ACEITE 0.92
#define DENSIDAD_SOUJI 0.92
#define RPMS_INICIO 1500

#define PORCENTAJE_ACEITE_ADRESS 20
#define PORCENTAJE_SOUJI_ADRESS 24

class Mezclas
{
    private:
        //// Aceite 
        
        float pesoAceiteDeseado = 0.0;
        float volumenAceite = 0.0;

        //// Souji
        float pesoSoujiDeseado = 0.0;
        float volumenSouji = 0.0;

        //// Agua
        float volumenAgua = 0.0;

        //// Mezclas
        float pesoAgregado = 0.0;
        float pesoInicioEtapa = 0.0;
        uint8_t misPantallasMezc = 0;
        int cantidadDeCincoLitros = 0;
         
    public:
        Mezclas();
        void init();
        void parado();
        void Pantallamezcla(uint8_t pantallamezcla);
        void laMezcla(int cantidadSouji);  
        void mezclaGeneral(int cantidadTotal);
        void calcularVolumen();
        void hecharLiquido(float volumen);
        void mezclaVacio();   

        void ajustarPorcenajeSouji();
        void ajustarPorcenajeAceite();

        void subirPorcentajeAceite();
        void subirPorcentajeSouji();
        void bajarPorcentajeAceite();
        void bajarPorcentajeSouji();
};

#endif