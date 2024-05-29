#include "Reloj_RTC.h"

RTC_DS1307 rtc;
DateTime Tiempo;

int cantidadMezclaMes[13] = {0}; 
int litrosMensuales[13] = {0};
uint8_t litrosTotales =0 ;
uint8_t segundo, minuto, hora, dia, mes, diaDeLaSemana;
uint16_t anio;
String elMes;

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




 
