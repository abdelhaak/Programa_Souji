#ifndef BASCULA_H
#define BASCULA_H

#include "HX711.h"

#define VALUE_TARA 20
#define PesoConocido 195  // EL peso del ejemplo en gramos

#define SCALE_ADDRESS 12   // Dirección para la escala

#define DOUT  A1
#define CLK  A0

extern HX711 balanza;

extern float peso;
extern float pesoTara;
extern long escala;
extern long adc_lecture;



void calibracion();
void balanza_Setup();
void finDeCalibracion();
void PesoActual();

#endif