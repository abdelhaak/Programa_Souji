#ifndef BASCULA_H
#define BASCULA_H

#include "HX711.h"

#define VALUE_TARA 20
#define PesoConocido 195  // EL peso del ejemplo en gramos

#define SCALE_ADDRESS 10   // Dirección para la escala

#define DOUT  A1
#define CLK  A0

extern HX711 balanza;

extern bool iniciarCalibracion;
extern float peso;
extern float pesoTara;
extern long escala;
extern long adc_lecture;
extern bool calibrarPeso;
extern bool calibrarPeso1;
extern bool conf;
extern bool mostrarPeso;


void calibracion1();
void calibracion2();
void balanza_Setup();
void finDeCalibracion();
void escribirEscala();
void cargarEscala();
float PesoActual();

#endif