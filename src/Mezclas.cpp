#include "Mezclas.h"
#include "Arduino.h"
#include "Menus.h"
#include "Bombas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,PIN_SENSOR);

//Menus menus;

// 2 minutos => 120000 milisegundos
uint64_t tiempoMezcla1 = 5000;
// 3 minutos => 180000 milisegundos
uint64_t tiempoMezcla2 = 10000; 
// 5 minutos de Vacio => 300000 milisegundos
uint64_t tiempoVacio = 15000;

float porcentajeAceite = 30.0;
float porcentajeSouji = 50.0;
float porcentajeAgua = 20.0;

Mezclas::Mezclas()
{
  pesoAceiteDeseado = 0;
  volumenAceite = 0.0;
  pesoSoujiDeseado = 0;
  pesoAguaDeseado = 0;
  pesoAgregado = 0.0;
  init();
}
void Mezclas::init()
{
    parado();
}

void Mezclas::parado()
{
    bombaAceite.off();
    bombaSouji.off();
    bombaAgua.off();
    bombaVacio.off();
    motorMezclador.parar();
}

void Mezclas::mezclaGeneral()
{
  pesoAceiteDeseado = porcentajeAceite * CAPACIDAD_TOTAL / 100.0;
  volumenAceite = pesoAceiteDeseado / DENSIDAD_ACEITE;
  //menus.PantallasMezcla(0);
  bombaAceite.on();
  while(pesoAgregado <volumenAceite)
  {
    pesoAgregado = PesoActual();
    //menus.PantallasMezcla(1);
    Serial.print("El peso es : ");
    Serial.println(pesoAgregado);
    delay(100);
  }
  Serial.println("Cantidad deseada hecha");
  Serial.println("Apagando Bomba");
  bombaAceite.off();
  Serial.println("Terminado");
  Serial.println("Ahora activamos el motor mezclador");
  delay(1000);
  Serial.print("Motor mezclador activo en : ");
  Serial.print(RPMS_INICIO);
  Serial.print(" RPMs");
  motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla1);
  motorMezclador.parar();
  Serial.println("Mezcla terminada.");
}

