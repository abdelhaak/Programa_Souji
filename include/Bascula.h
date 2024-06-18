#ifndef BASCULA_H
#define BASCULA_H

#include "HX711.h"

#define VALUE_TARA 20
#define PesoConocido 195   // 195 // EL peso del ejemplo en gramos
#define SCALE_ADDRESS 80   // Dirección para la escala
#define TARE_ADRESS 84     // Dirección para la TARA
#define DOUT A5
#define CLK  A4

extern HX711 balanza;

extern uint16_t peso;
extern uint16_t pesoTara;
extern long escala;
extern long adc_lecture;
extern float ultima_tara;


void calibracion();
void balanza_Setup();
void finDeCalibracion();
uint16_t PesoActual();

#endif