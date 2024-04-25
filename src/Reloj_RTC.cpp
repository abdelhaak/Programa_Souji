#include "Reloj_RTC.h"
#include "Menus.h"
#include <LiquidCrystal.h>

RTC_DS1307 rtc;
DateTime Tiempo;

void relojBegin()
{
  rtc.begin();
  if (!rtc.isrunning()) {
    Serial.println("RTC no está funcionando. Ajustando fecha y hora...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
} 
String Fecha_actual(uint8_t dia, int8_t mes, long anio)
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



 
