#ifndef MY_MEZCLADOR_H
#define MY_MEZCLADOR_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Motor.h"
#include "Bombas.h"
#include "Bascula.h"
#include "Menus.h"
#include "Boton.h"
#include "SoftwareSerial.h"

extern SoftwareSerial mySerial;

// Pines de las bombas
#define PIN_BOMBA_ACEITE 13
#define PIN_BOMBA_SOUJI 12
#define PIN_BOMBA_AGUA 10
#define PIN_BOMBA_VACIO 11

// Capacidad total de la mezcla que seria de 5000 g que son 5L
#define CAPACIDAD_TOTAL 5000 // 5000
#define DENSIDAD_ACEITE 0.92
#define DENSIDAD_SOUJI 0.92

#define STATUS_ADRESS 16
#define STATUS_2_ADRESS 18
#define NUM_MEZCLAS_ADRESS 24
#define I_MEZCLAS_ADRESS 66
#define PESO_ACEITE_ACTUAL_ADRESS 68
#define PESO_RELATIVO_ADDRESS 74

//extern bool enPausa;
class Mezclas
{
    private:
        //// Mezclas
        int16_t pesoAgregado = 0;
        int16_t pesoInicioEtapa = 0;
        uint8_t misPantallasMezc = 0;
        int cantidadDeCincoLitros = 0;
        bool finMezcla = false;
        uint64_t tiempoInicioVacio=0;
        SoftwareSerial& mySerial;

    public:
        Mezclas(SoftwareSerial& serial);
        void init();

        void checkStatus();  
        void parado();
        void Pantallamezcla(uint8_t pantallamezcla);
        void laMezcla(int cantidadSouji);  
        void mezclaMultiusos(int mezclas);
        void mezclaFregasuelos(int mezclas);
        void echarLiquido(int16_t pesoPorechar);
        void mezclaVacio();   
        void resetearTodo();
        
        /*
        void calcularVolumen();
        void subirPorcentajeAceite();
        void subirPorcentajeSouji();
        void bajarPorcentajeAceite();
        void bajarPorcentajeSouji();
        */
        
        void pausarMezcla();
        void esperarParaReanudar();
        void vacioGeneral();
        void verificarPeso();
        void cancelarMezcla();
        void apagarBombas();
        void encenderBombaCorrespondiente();
        void actualizarPantallaLCD(int estado2, int idioma);
        void mostrarLiquido();
        void mostrarAgotado();
        void deteccionPulso();

        bool enPausa = false;
        bool mezclando = false;
        int numMezclas = 0;
        int estado = 0;
        int estado2 = 0;
        int i_mezclas = 0;
        int16_t pesoLiquido = 0;
        int16_t pesoRelative = 0;
};

#endif