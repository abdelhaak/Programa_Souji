#include "Mezclas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,pin_encoder);
Menus menus(lcd);


// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 5000;
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 5000;
// El peso minimo del vacio autorizado
uint64_t pesoMinimo = 30.0;
// El tiempo de error autorizado de la bascula 
// iniciado en 3 minutos => 180000
uint64_t tiempoErrorBascula = 180000;

uint64_t tiempoPasado = 0;
// Porcentajes de inicio de liquidos
float porcentajeAceite = 30.0;
float porcentajeSouji = 50.0;

Mezclas::Mezclas()
{
  pesoAceiteDeseado = 0;
  pesoSoujiDeseado = 0;
  volumenAceite = 0.0;
  volumenSouji = 0.0;
  volumenAgua = 0.0 ;
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

// La MEZCLA COMPLETA 
void Mezclas::mezclaGeneral(int mezclas)
{
  Pantallamezcla(0);
  delay(1000);
  calcularVolumen();
  for(int i=0; i<mezclas; i++)
  {
    Serial.print("Esta es la mezcla NUMERO : ");
    Serial.println(i);
    // Hechamos la cantidad adecuada para el Aceite
    Serial.println("Bomba Aceite activada : ");
    Pantallamezcla(1);
    bombaAceite.on();
    hecharLiquido(volumenAceite);
    Serial.println("Apagando Bomba de Aceite");
    bombaAceite.off();

    // Hechamos la cantidad adecuada para el SOUJI
    Serial.println("Bomba Souji activada : ");
    Pantallamezcla(2);
    bombaSouji.on();
    hecharLiquido(volumenSouji);
    Serial.println("Apagando Bomba de Souji");
    bombaSouji.off();

    // Pasamos a la primera etapa de mezcla con el motor 
    Serial.println("Ahora activamos el motor mezclador");
    delay(1000);
    Serial.print("Motor mezclador activo en :  ");
    Serial.print(RPMS_INICIO);
    Serial.println(" RPMs");
    Pantallamezcla(4);
    // Activamos el motor con los RPMs guardados y el tiempo adecuado
    motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla1);
    // Apagar el motor y pasar a la siguiente etapa
    motorMezclador.parar();

    // Hechamos la cantidad adecuada de AGUA
    Serial.println("Bomba Agua activada : ");
    Pantallamezcla(3);
    bombaAgua.on();
    hecharLiquido(volumenAgua);
    Serial.println("Apagando Bomba de Agua");
    bombaAgua.off();

    // Pasamos a la segunda etapa de mezcla con el motor 
    Serial.println("Ahora activamos el motor mezclador por la segunda vez");
    delay(1000);
    Serial.print("Motor mezclador activo en :  ");
    Serial.print(RPMS_INICIO);
    Serial.println(" RPMs");
    Pantallamezcla(5);
    // Activamos el motor con los RPMs guardados y el tiempo adecuado
    motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla2);
    // Apagar el motor y pasar a la siguiente etapa
    motorMezclador.parar();

    // Pasamos a la etapa del vacio
    Serial.println("Empezamos el vacio....");
    Pantallamezcla(6);
    mezclaVacio();
    Serial.println("Vacio finalizado");
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("MEZCLA NUM : ");
      lcd.setCursor(14,0);
      lcd.print(i+1);
      lcd.setCursor(2,1);
      lcd.print("FINALIZADA");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("MIX NUMBER : ");
      lcd.setCursor(14,0);
      lcd.print(i+1);
      lcd.setCursor(2,1);
      lcd.print("COMPLETED");
    }
    delay(2000);   
  }
  Serial.println("Mezcla terminada.");
  Pantallamezcla(7);
  delay(2000);
  menus.PantallaSeleccionada(0);
}

void Mezclas::Pantallamezcla(uint8_t pantallamezcla)
{
  // Limpieza de la pantalla 
  if(pantallamezcla != misPantallasMezc)
  {
    lcd.clear();
    misPantallasMezc = pantallamezcla;
  }

  if (pantallamezcla == 0)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("AHORA EMPEZAMOS");
      lcd.setCursor(0,1);
      lcd.print("LA MEZCLA");
      lcd.setCursor(12,1);
      lcd.createChar(0, Character2);
    }
    else
    {
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("NOW STARTING");
      lcd.setCursor(3,1);
      lcd.print("THE MIX");
      lcd.setCursor(12,1);
      lcd.createChar(0, Character2);
    }
  }

  // Pantalla hechando Aceite
  if (pantallamezcla == 1)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HECHANDO ACEITE");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MAKING OIL");
    }
  }

  // Pantalla hechando Souji
  if (pantallamezcla == 2)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HECHANDO SOUJI");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MAKING SOUJI");
    }
  }

  // Pantalla hechando Agua
  if (pantallamezcla == 3)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("HECHANDO AGUA");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("MAKING WATER");
    }
  }

  // Pantalla Mezclando Primera Mezcla
  if (pantallamezcla == 4)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("PRIMERA MEZCLA");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("FIRST SHAKE");
    }
  }

  // Pantalla Mezclando Primera Mezcla
  if (pantallamezcla == 5)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SEGUNDA MEZCLA");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SECOND SHAKE");
    }
  }

  // Pantalla del VACIO
  if (pantallamezcla == 6)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("VACIANDO ....");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("EMPTYING ...");
    }
  }

  // Pantalla de fin de mezcla
  if (pantallamezcla == 7)
  {
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("MEZCLA TOTAL");
      lcd.setCursor(3,1);
      lcd.print("COMPLETADA");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("TOTAL MIX");
      lcd.setCursor(3,1);
      lcd.print("COMPLETED");
    }
  }

  // Pantalla Ajustar porcentaje Aceite
  if (pantallamezcla == 8)
  {
    EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("AJUSTANDO ACEITE");
      lcd.setCursor(5,1);
      lcd.print(porcentajeAceite);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SETTING OIL");
      lcd.setCursor(5,1);
      lcd.print(porcentajeAceite);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
  }

  // Pantalla Ajustar porcentaje Souji
  if (pantallamezcla == 9)
  {
    EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
    if(idioma==0)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("AJUSTANDO SOUJI");
      lcd.setCursor(5,1);
      lcd.print(porcentajeSouji);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("SETTING SOUJI");
      lcd.setCursor(5,1);
      lcd.print(porcentajeSouji);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
  }
}

void Mezclas::mezclaVacio()
{
  bombaVacio.on();
  unsigned long tiempoInicio = millis(); 
  float pesoInicial = PesoActual();
  float pesoVaciado;
  float pesoActual; 
  float pesoTotalAVaciar = pesoInicial - pesoMinimo;

  while(PesoActual() > pesoMinimo)
  {
    pesoActual = PesoActual();
    pesoVaciado = pesoInicial - pesoActual; 
    updateProgressBar(pesoVaciado, pesoTotalAVaciar, 1); // Actualizar la barra de progreso
    delay(500);
    if (millis() - tiempoInicio > tiempoErrorBascula)
    {
      Serial.println("Error de la bascula : Tiempo máximo de espera alcanzado");
      bombaVacio.off();
      return;
    }
  }  
  bombaVacio.off();
  //Serial.println("Proceso finalizado");
}

void Mezclas::calcularVolumen()
{
  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  pesoAceiteDeseado = porcentajeAceite * CAPACIDAD_TOTAL / 100.0;
  volumenAceite = pesoAceiteDeseado / DENSIDAD_ACEITE;

  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  pesoSoujiDeseado = porcentajeSouji * CAPACIDAD_TOTAL /100.0;
  volumenSouji = pesoSoujiDeseado / DENSIDAD_SOUJI;

  // Calculos de la cantidad de Agua que es lo que queda
  volumenAgua = CAPACIDAD_TOTAL - (pesoAceiteDeseado + pesoSoujiDeseado);
}

void Mezclas::hecharLiquido(float volumen)
{
  pesoInicioEtapa = PesoActual();
  float pesoRelativo = 0;
  while(pesoRelativo < volumen)
  {
    pesoRelativo = PesoActual() - pesoInicioEtapa;
    updateProgressBar(pesoRelativo, volumen, 1);  
    delay(100);
  }
}

void Mezclas::subirPorcentajeAceite()
{
  porcentajeAceite++;
  EEPROM.put(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  Pantallamezcla(8);
}
void Mezclas::subirPorcentajeSouji()
{
  porcentajeSouji++;
  EEPROM.put(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  Pantallamezcla(9);
}

void Mezclas::bajarPorcentajeAceite()
{
  porcentajeAceite--;
  EEPROM.put(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  Pantallamezcla(8);
}

void Mezclas::bajarPorcentajeSouji()
{
  porcentajeSouji--;
  EEPROM.put(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  Pantallamezcla(9);
}
