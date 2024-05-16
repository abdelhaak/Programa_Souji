#include "Mezclas.h"
#include "Arduino.h"
#include "Menus.h"
#include "Bombas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,PIN_SENSOR);

// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 120000;
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 180000;
// El peso minimo del vacio autorizado
uint64_t pesoMinimo = 30.0;
// El tiempo de error autorizado de la bascula 
// iniciado en 3 minutos => 180000
uint64_t tiempoErrorBascula = 180000;

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
  Serial.println("Apagando Bomba");
  bombaAceite.off();
  Serial.println("Ahora activamos el motor mezclador");
  delay(1000);
  Serial.print("Motor mezclador activo en :  ");
  Serial.print(RPMS_INICIO);
  Serial.println(" RPMs");
  motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla1);
  motorMezclador.parar();
  Serial.println("Mezcla terminada.");
}

void Mezclas::mezclaVacio()
{
  Serial.println("Iniciamos el vacio");
  Serial.println("Activamos la bomba");
  bombaVacio.on();
  unsigned long tiempoInicio = millis(); // Guardar el tiempo de inicio

  while(PesoActual() < pesoMinimo)
  {
    delay(500);
    if (millis() - tiempoInicio > tiempoErrorBascula)
    {
      Serial.println("Error de la bascula : Tiempo máximo de espera alcanzado");
      Serial.println("Apagamos la Bomba ");
      bombaVacio.off();
      return;
    }
  }  
  Serial.println("Bomba apagada");
  bombaVacio.off();
  Serial.println("Proceso finalizado");
}