#ifndef RELOJ_RTC_H
#define RELOJ_RTC_H

#include "RTClib.h"

extern RTC_DS1307 rtc;
extern DateTime Tiempo;

// Variables de la actualización de la hora
extern unsigned long tiempoActualizacion;
extern const unsigned long intervaloActualizacion;

extern char numeroDia[7][12];
extern char numeroMes[12][12];
extern uint8_t segundo, minuto, hora, dia, mes, diaDeLaSemana;
extern long anio;
extern String La_fecha;
extern String La_hora;

void relojBegin();

String Fecha_actual(uint8_t dia, int8_t mes, long anio);
String Hora_actual();

#endif