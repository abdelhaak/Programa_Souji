#ifndef BASCULA_H
#define BASCULA_H

#include "HX711.h"

#define VALUE_TARA 20
#define PesoConocido 107   // 107 // EL peso del ejemplo en gramos
#define SCALE_ADDRESS 80   // Dirección para la escala
#define TARE_ADRESS 90     // Dirección para la TARA
#define PESO_ADRESS 32

#define DOUT A5
#define CLK  A4

extern HX711 balanza;

extern int16_t peso;
extern int16_t pesoTara;
extern long escala;
extern long adc_lecture;
extern float ultima_tara;


void calibracion();
void balanza_Setup();
void finDeCalibracion();
int16_t PesoActual();

#endif