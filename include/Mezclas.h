#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Motor.h"
#include "Bombas.h"
#include "Bascula.h"
#include "LCD_Progress.h"
#include "Menus.h"
#include "Boton.h"


#define PIN_BOMBA_ACEITE 51
#define PIN_BOMBA_SOUJI 50
#define PIN_BOMBA_AGUA 49
#define PIN_BOMBA_VACIO 48

// Capacidad total de la mezcla que seria de 5000 g que son 5L
#define CAPACIDAD_TOTAL 300 
#define DENSIDAD_ACEITE 0.92
#define DENSIDAD_SOUJI 0.92

#define PORCENTAJE_ACEITE_ADRESS 20
#define PORCENTAJE_SOUJI_ADRESS 22
#define STATUS_ADRESS 16
#define STATUS_2_ADRESS 18
#define NUM_MEZCLAS_ADRESS 24
#define VOL_ACEITE_ADRESS 60
#define VOL_SOUJI_ADRESS 62
#define VOL_AGUA_ADRESS 64
#define I_MEZCLAS_ADRESS 66
#define PESO_ACEITE_ACTUAL_ADRESS 68
#define PESO_SOUJI_ACTUAL_ADRESS 70
#define PESO_AGUA_ACTUAL_ADRESS 72
#define PESO_RELATIVO_ADDRESS 74

//extern bool enPausa;
class Mezclas
{
    private:

        //// Aceite 
        uint16_t pesoAceiteDeseado = 0;
        uint16_t volumenAceite = 0;

        //// Souji
        uint16_t pesoSoujiDeseado = 0;
        uint16_t volumenSouji = 0;

        //// Agua
        uint16_t volumenAgua = 0;

        //// Mezclas
        uint16_t pesoAgregado = 0;
        uint16_t pesoInicioEtapa = 0;
        uint8_t misPantallasMezc = 0;
        int cantidadDeCincoLitros = 0;
        bool finMezcla = false;

    public:
        Mezclas();
       /* void init();
        void checkStatus();  
        void parado();
        void Pantallamezcla(uint8_t pantallamezcla);
        void laMezcla(int cantidadSouji);  
        void mezclaGeneral(int mezclas);
        void calcularVolumen();
        void hecharLiquido(uint16_t volumen);
        void mezclaVacio();   
        void resetearTodo();
        void subirPorcentajeAceite();
        void subirPorcentajeSouji();
        void bajarPorcentajeAceite();
        void bajarPorcentajeSouji();
        void pausarReanudarMezcla();
        void esperarParaReanudar();*/

        bool enPausa = false;
        int numMezclas;
        int estado;
        int estado2;
        int i_mezclas;
        uint16_t porcentajeAceite;
        uint16_t porcentajeSouji;
        uint16_t pesoLiquido;
};

#endif