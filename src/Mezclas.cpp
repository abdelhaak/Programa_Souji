#include "Mezclas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,pin_encoder);
Menus menus(lcd);

int estado = 0;
int estado2 = 0;
int numMezclas = 0;
int i_mezclas=0;
uint16_t pesoLiquido = 0;
// ERRORES

// El tiempo de error autorizado de la bascula 
// iniciado en 3 minutos => 180000 // POR AHORA 15 SEGUNDOS
uint64_t tiempoErrorBascula = 15000;

uint64_t tiempoPasado = 0;

// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 5000;
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 5000;
// El peso minimo del vacio autorizado
uint16_t pesoMinimo = 30;

// Porcentajes de inicio de liquidos
uint16_t porcentajeAceite = 30;
uint16_t porcentajeSouji = 50;

Mezclas::Mezclas()
{
  pesoAceiteDeseado = 0;
  pesoSoujiDeseado = 0;
  volumenAceite = 0;
  volumenSouji = 0;
  volumenAgua = 0;
  pesoAgregado = 0;
  porcentajeAceite = 30;
  porcentajeSouji = 50;
  init();
}

void Mezclas::init()
{
  parado();
}

/*
void Mezclas::checkStatus()
{
  Serial.println("Estamos en checstatus");
  //EEPROM.get(STATUS_ADRESS, estado);
  //EEPROM.get(STATUS_2_ADRESS, estado2);
  //EEPROM.get(NUM_MEZCLAS_ADRESS, numMezclas);
  if (estado == 0)
  {
    Serial.println("estado == 0");
    menus.PantallaSeleccionada(0);
  }
  else if(estado > 0)
  {
    Serial.println("estado > 0");
    mezclaGeneral(numMezclas);
  }
}
*/

void Mezclas::parado()
{
  bombaAceite.off();
  bombaSouji.off();
  bombaAgua.off();
  bombaVacio.off();
  motorMezclador.pararMotor();
}

// LA MEZCLA COMPLETA 
void Mezclas::mezclaGeneral(int mezclas)
{
  Serial.println("Estamos en mezclaGeneral");
  EEPROM.get(I_MEZCLAS_ADRESS, i_mezclas);
  for (int i = 60; i <= 80; i++) 
  {
    byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
    Serial.print("Direccion ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value, DEC); // Imprime el valor como decimal
  }
  EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  // Iniciamos la mezcla 
  if(estado == 0)
  {
    Serial.println("Estamos en el estado 0");
    Pantallamezcla(0);
    delay(2000);
    estado = 1;
    EEPROM.put(STATUS_ADRESS, estado);
  }
  // Calculamos el volumen de cada liquido
  if(estado == 1)
  {
    Serial.println("Estamos en el estado 1");
    estado = 2;
    estado2 = 1;
    EEPROM.put(STATUS_ADRESS, estado);
    EEPROM.put(STATUS_2_ADRESS, estado2);
    calcularVolumen();
    delay(50);
    for (int i = 60; i <= 80; i++) 
    {
    byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
    Serial.print("Direccion ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(value, DEC); // Imprime el valor como decimal
    }
  }
  // Pasamos a la mezcla general
  if(estado == 2)
  {
    Serial.println("Estamos en el estado 2");
    for (int i = 60; i <= 80; i++) 
    {
      byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
      Serial.print("Direccion ");
      Serial.print(i);
      Serial.print(": ");
      Serial.println(value, DEC); // Imprime el valor como decimal
    }
    if(mezclas > 1)
    {
      Serial.println("Mezclas es superior a 1");
      for(i_mezclas=1; i_mezclas<mezclas; i_mezclas++)
        {
          EEPROM.put(I_MEZCLAS_ADRESS, i_mezclas);
          // Hechamos la cantidad adecuada para el Aceite
          if(estado2 == 1)
          {
            Serial.println("Estado 2 && Estado2 1");
            Serial.print("Esta es la mezcla NUMERO : ");
            Serial.println(i_mezclas);
            Serial.println("Bomba Aceite activada : ");
            Pantallamezcla(1);
            bombaAceite.on();
            hecharLiquido(volumenAceite);
            Serial.println("Apagando Bomba de Aceite");
            bombaAceite.off();
            estado2 = 2;
            EEPROM.put(STATUS_2_ADRESS, estado2);
          }  
          // Hechamos la cantidad adecuada para el SOUJI
          if(estado2 == 2)
          { 
            Serial.println("Estado 2 && Estado2 2");
            Serial.println("Bomba Souji activada : ");
            Pantallamezcla(2);
            bombaSouji.on();
            hecharLiquido(volumenSouji);
            Serial.println("Apagando Bomba de Souji");
            bombaSouji.off();
            estado2 = 3;
            EEPROM.put(STATUS_2_ADRESS, estado2);
          }
          // Pasamos a la primera etapa de mezcla con el motor 
          if(estado2 == 3)
          { 
            Serial.println("Estado 2 && Estado2 3");
            Serial.println("Ahora activamos el motor mezclador");
            delay(1000);
            Pantallamezcla(4);
            // Activamos el motor con los RPMs guardados y el tiempo adecuado
            motorMezclador.ajustarRpms(tiempoMezcla1);
            // Apagar el motor y pasar a la siguiente etapa
            motorMezclador.pararMotor();
            estado2 = 4;
            EEPROM.put(STATUS_2_ADRESS, estado2);
          }
          // Hechamos la cantidad adecuada de AGUA
          if(estado2 == 4)
          {
            Serial.println("Estado 2 && Estado2 4");
            Serial.println("Bomba Agua activada : ");
            Pantallamezcla(3);
            bombaAgua.on();
            hecharLiquido(volumenAgua);
            Serial.println("Apagando Bomba de Agua");
            bombaAgua.off();
            estado2 = 5;
            EEPROM.put(STATUS_2_ADRESS, estado2);
          }
          // Pasamos a la segunda etapa de mezcla con el motor 
          if(estado2 == 5)
          {
            Serial.println("Estado 2 && Estado2 5");
            Serial.println("Ahora activamos el motor mezclador por la segunda vez");
            delay(1000);
            Pantallamezcla(5);
            // Activamos el motor con los RPMs guardados y el tiempo adecuado
            motorMezclador.ajustarRpms(tiempoMezcla2);
            // Apagar el motor y pasar a la siguiente etapa
            motorMezclador.pararMotor();
            estado2 = 6;
            EEPROM.put(STATUS_2_ADRESS, estado2);
          }
          // Pasamos a la etapa del vacio
          if(estado2 == 6)
          {
            Serial.println("Estado 2 && Estado2 6");
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
              lcd.print(i_mezclas+1);
              lcd.setCursor(2,1);
              lcd.print("FINALIZADA");
              estado2 = 1;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            else
            {
              lcd.clear();
              lcd.setCursor(1,0);
              lcd.print("MIX NUMBER : ");
              lcd.setCursor(14,0);
              lcd.print(i_mezclas+1);
              lcd.setCursor(2,1);
              lcd.print("COMPLETED");
              estado2 = 1;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            delay(2000);   
          }
        }
      finMezcla = true ;
    }
    else
    {
      Serial.println("Mezcla es 1");
      // Hechamos la cantidad adecuada para el Aceite
      if(estado2 == 1)
      {
        Serial.println("Estado 2 && Estado2 1");
        Serial.println("Esta es una sola mezcla ");
        Serial.println("Bomba Aceite activada ");
        EEPROM.get(VOL_ACEITE_ADRESS, volumenAceite);
        EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
        Pantallamezcla(1);
        bombaAceite.on();
        Serial.print("volumenAceite : ");
        Serial.println(volumenAceite);
        Serial.println("Los valores antes de hechar el aceite");
        for (int i = 60; i <= 80; i++)
        {
          byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
          Serial.print("Direccion ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(value, DEC); // Imprime el valor como decimal
        }
        hecharLiquido(volumenAceite);
        Serial.println("Los valores despues de hechar el aceite");
        for (int i = 60; i <= 80; i++)
        {
          byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
          Serial.print("Direccion ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(value, DEC); // Imprime el valor como decimal
        }
        Serial.println("Apagando Bomba de Aceite");
        bombaAceite.off();
        estado2 = 2;
        EEPROM.put(STATUS_2_ADRESS, estado2);
      }  
      // Hechamos la cantidad adecuada para el SOUJI
      if(estado2 == 2)
      {
        Serial.println("Estado 2 && Estado2 2");
        Serial.println("Bomba Souji activada : ");
        EEPROM.get(VOL_SOUJI_ADRESS, volumenSouji);
        EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
        Pantallamezcla(2);
        bombaSouji.on();
        Serial.print("volumenSouji : ");
        Serial.println(volumenSouji);
        Serial.println("Los valores antes de hechar el Souji");
        for (int i = 60; i <= 80; i++)
        {
          byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
          Serial.print("Direccion ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(value, DEC); // Imprime el valor como decimal
        }
        hecharLiquido(volumenSouji);
        Serial.println("Los valores despues de hechar el Souji");
        for (int i = 60; i <= 80; i++)
        {
          byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
          Serial.print("Direccion ");
          Serial.print(i);
          Serial.print(": ");
          Serial.println(value, DEC); // Imprime el valor como decimal
        }
        Serial.println("Apagando Bomba de Souji");
        bombaSouji.off();
        estado2 = 3;
        EEPROM.put(STATUS_2_ADRESS, estado2);
      }
      // Pasamos a la primera etapa de mezcla con el motor 
      if(estado2 == 3)
      { 
        Serial.println("Estado 2 && Estado2 3"); 
        Serial.println("Ahora activamos el motor mezclador");
        delay(1000);
        Pantallamezcla(4);
        // Activamos el motor con los RPMs guardados y el tiempo adecuado
        motorMezclador.ajustarRpms(tiempoMezcla1);
        // Apagar el motor y pasar a la siguiente etapa
        motorMezclador.pararMotor();
        estado2 = 4;
        EEPROM.put(STATUS_2_ADRESS, estado2);
      }
      // Hechamos la cantidad adecuada de AGUA
      if(estado2 == 4)
      {
        Serial.println("Estado 2 && Estado2 4");
        Serial.println("Bomba Agua activada : ");
        EEPROM.get(VOL_AGUA_ADRESS, volumenAgua);
        Pantallamezcla(3);
        bombaAgua.on();
        Serial.print("volumenAgua : ");
        Serial.println(volumenAgua);
        hecharLiquido(volumenAgua);
        Serial.println("Apagando Bomba de Agua");
        bombaAgua.off();
        estado2 = 5;
        EEPROM.put(STATUS_2_ADRESS, estado2);
      }
      // Pasamos a la segunda etapa de mezcla con el motor 
      if(estado2 == 5)
      {
        Serial.println("Estado 2 && Estado2 5");
        Serial.println("Ahora activamos el motor mezclador por la segunda vez");
        delay(1000);
        Pantallamezcla(5);
        // Activamos el motor con los RPMs guardados y el tiempo adecuado
        motorMezclador.ajustarRpms(tiempoMezcla2);
        // Apagar el motor y pasar a la siguiente etapa
        motorMezclador.pararMotor();
        estado2 = 6;
        EEPROM.put(STATUS_2_ADRESS, estado2);
      }
      // Pasamos a la etapa del vacio
      if(estado2 == 6)
      {
        Serial.println("Estado 2 && Estado2 6");
        Serial.println("Empezamos el vacio....");
        Pantallamezcla(6);
        mezclaVacio();
        Serial.println("Vacio finalizado");
        if(idioma==0)
        {
          lcd.clear();
          lcd.setCursor(5,0);
          lcd.print("MEZCLA ");
          lcd.setCursor(2,1);
          lcd.print("FINALIZADA");
          finMezcla = true ;
        }
        else
        {
          lcd.clear();
          lcd.setCursor(1,0);
          lcd.print("THE MIX IS ");
          lcd.setCursor(2,1);
          lcd.print("COMPLETED");
          finMezcla = true ;
        }
        delay(2000);   
      }
    } 
  }
  // Finalizamos la mezcla
  if(estado == 2 && finMezcla)
  {
    Serial.println("Estado 2 && fin de mezcla");
    estado = 0;
    estado2 = 0;
    numMezclas = 0;
    volumenAceite = 0;
    volumenSouji = 0;
    volumenAgua = 0;
    EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
    EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);
    EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);
    EEPROM.put(STATUS_ADRESS, estado);
    EEPROM.put(STATUS_2_ADRESS, estado2);
    EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
    finMezcla = false;
    Serial.println("Mezcla terminada.");
    Pantallamezcla(7);
    delay(2000);

    menus.PantallaSeleccionada(0);
  }
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
    //EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
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
    //EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
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
  uint16_t pesoInicial = PesoActual();
  uint16_t pesoVaciado;
  uint16_t pesoActual; 
  uint16_t pesoTotalAVaciar = pesoInicial - pesoMinimo;

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
}

void Mezclas::calcularVolumen()
{
  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  Serial.print("porcentajeAceite : ");
  Serial.println(porcentajeAceite);
  pesoAceiteDeseado = porcentajeAceite*CAPACIDAD_TOTAL / 100.0;
  volumenAceite = pesoAceiteDeseado / DENSIDAD_ACEITE;
  EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);

  Serial.print("pesoAceiteDeseado : ");
  Serial.println(pesoAceiteDeseado);
  Serial.print("volumenAceite : ");
  Serial.println(volumenAceite);
  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  Serial.print("porcentajeSouji : ");
  Serial.println(porcentajeSouji);
  pesoSoujiDeseado = porcentajeSouji * CAPACIDAD_TOTAL /100.0;
  volumenSouji = pesoSoujiDeseado / DENSIDAD_SOUJI;
  EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);

  Serial.print("pesoSoujiDeseado : ");
  Serial.println(pesoSoujiDeseado);
  Serial.print("volumenSouji : ");
  Serial.println(volumenSouji);
  // Calculos de la cantidad de Agua que es lo que queda
  volumenAgua = CAPACIDAD_TOTAL - (pesoAceiteDeseado + pesoSoujiDeseado);
  EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
  Serial.print("volumenAgua : ");
  Serial.println(volumenAgua);
}

void Mezclas::hecharLiquido(uint16_t volumen)
{
  pesoLiquido = PesoActual();
  //EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  Serial.print("pesoLiquido : ");
  Serial.println(pesoLiquido);
  uint16_t pesoRelativo = 0;
  while(pesoRelativo < volumen)
  {
    pesoRelativo = PesoActual() - pesoLiquido;
    updateProgressBar(pesoRelativo, volumen, 1);  
    delay(100);
  }
  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  Serial.print("peso Final : ");
  Serial.println(pesoLiquido);
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


void Mezclas::resetearTodo()
{
  Serial.println("Reset...");
  estado = 0;
  estado2 = 0;
  numMezclas = 0;
  volumenAceite = 0;
  volumenSouji = 0;
  volumenAgua = 0;
  EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
  EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);
  EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);
  EEPROM.put(STATUS_ADRESS, estado);
  EEPROM.put(STATUS_2_ADRESS, estado2);
  EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
  finMezcla = false;
}