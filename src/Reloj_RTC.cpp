#include "Reloj_RTC.h"
#include "Menus.h"
#include <LiquidCrystal.h>
#include "EEPROM.h"

RTC_DS1307 rtc;
DateTime Tiempo;

#define LITROS_MENSUALES_DIRECCION 32


#define TAMANIO_DATOS_MENSUALES 4

int cantidadMezclaMes[12] = {0}; 
int litrosMensuales[12] = {0};
uint32_t litrosTotales =0 ;
String elMes;


void guardarLitrosMensualesEnEEPROM() {
  for (int i = 0; i < 12; ++i) {
    int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
    EEPROM.put(direccion, litrosMensuales[i]);
  }
}

void cargarLitrosMensualesDesdeEEPROM() {
  for (int i = 0; i < 12; ++i) {
    int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
    EEPROM.get(direccion, litrosMensuales[i]);
  }
}

String Fecha_actual(uint8_t dia, int8_t mes, uint16_t anio)
{
  Tiempo = rtc.now();
  dia = Tiempo.day();
  mes = Tiempo.month();
  anio = Tiempo.year();
  String diaStr = (dia < 10) ? "0" + String(dia) : String(dia);
  String mesStr = (mes < 10) ? "0" + String(mes) : String(mes);
  String anioStr = String(anio);
  return diaStr + "/" + mesStr + "/" + anioStr;
}

String Hora_actual() 
{
  Tiempo = rtc.now();
  int segundo = Tiempo.second();
  int minuto = Tiempo.minute();
  int hora = Tiempo.hour();
  String segundoStr = (segundo < 10) ? "0" + String(segundo) : String(segundo);
  String minutoStr = (minuto < 10) ? "0" + String(minuto) : String(minuto);
  String horaStr = (hora < 10) ? "0" + String(hora) : String(hora);
  return horaStr + ":" + minutoStr + ":" + segundoStr;
}




 
