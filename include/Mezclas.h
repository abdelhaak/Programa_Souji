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
#include "SoftwareSerial.h"

extern SoftwareSerial mySerial;

#define PIN_BOMBA_ACEITE 13
#define PIN_BOMBA_SOUJI 12
#define PIN_BOMBA_AGUA 10
#define PIN_BOMBA_VACIO 11

// Capacidad total de la mezcla que seria de 5000 g que son 5L
#define CAPACIDAD_TOTAL 5000 // 5000
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
        int16_t pesoAceiteDeseado = 0;
        int16_t volumenAceite = 0;

        //// Souji
        int16_t pesoSoujiDeseado = 0;
        int16_t volumenSouji = 0;

        //// Agua
        int16_t pesoAguaDeseado = 0;
        int16_t volumenAgua = 0;

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
        void mezclaGeneral(int mezclas);
        void calcularVolumen();
        void echarLiquido(int16_t pesoPorechar);
        void mezclaVacio();   
        void resetearTodo();
        void subirPorcentajeAceite();
        void subirPorcentajeSouji();
        void bajarPorcentajeAceite();
        void bajarPorcentajeSouji();
        void pausarReanudarMezcla();
        void esperarParaReanudar();
        void vacioGeneral();
        void verificarPeso();
        void cancelarMezcla();
        void apagarBombas();
        void encenderBombaCorrespondiente();
        void actualizarPantallaLCD(int estado2, int idioma);
        void mostrarLiquido();
        void mostrarAgotado();
        

        bool enPausa = false;
        int numMezclas;
        int estado;
        int estado2;
        int i_mezclas;
        int16_t porcentajeAceite;
        int16_t porcentajeSouji;
        int16_t pesoLiquido;
        int16_t pesoRelative;
};

#endif