#ifndef RELOJ_RTC_H
#define RELOJ_RTC_H

#include "RTClib.h"

#define LITROS_MENSUALES_DIRECCION 32
#define TAMANIO_DATOS_MENSUALES 2

extern RTC_DS1307 rtc;
extern DateTime Tiempo;

// Variables de la actualización de la hora
extern unsigned long tiempoActualizacion;
extern const unsigned long intervaloActualizacion;

extern char numeroDia[7][12];
extern char numeroMes[12][12];

extern uint8_t segundo, minuto, hora, dia, mes, diaDeLaSemana;
extern String elMes;

extern uint16_t anio;
extern String La_fecha;
extern String La_hora;

extern uint8_t litrosTotales;
extern int cantidadMezclaMes[13]; 
extern int litrosMensuales[13];

void rtc_init();
String Fecha_actual(uint8_t dia, int8_t mes, uint16_t anio);
String Hora_actual();

#endif