#include "Reloj_RTC.h"

// ERRORES

RTC_DS1307 rtc;
DateTime Tiempo;

int cantidadMezclaMes[13] = {0}; 
int litrosMensuales[13] = {0};
uint8_t litrosTotales = 0 ;
uint8_t segundo, minuto, hora,diaDeLaSemana,dia,mes;
int16_t anio;
String elMes;


void rtc_init()
{
  rtc.begin();
  Tiempo = rtc.now();
  /*dia = Tiempo.day();
  mes = Tiempo.month();
  anio = Tiempo.year();
  EEPROM.put(DAY_ADDRESS, dia); 
  EEPROM.put(MONTH_ADDRESS, mes); 
  EEPROM.put(YEAR_ADDRESS, anio); */
  EEPROM.put(DAY_ADDRESS, 9); // Día inicial
  EEPROM.put(MONTH_ADDRESS, 9); // Mes inicial
  EEPROM.put(YEAR_ADDRESS, 2024); // Año inicial
  
}
String Fecha_actual()
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




 
