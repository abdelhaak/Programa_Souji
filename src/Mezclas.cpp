#include "Mezclas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,pin_encoder);
Menus menus(lcd,mySerial);

Boton botonPausa(PIN_BOTON_SEL);

int estado = 0;
int estado2 = 0;
int numMezclas = 0;
int i_mezclas=0;
int16_t pesoLiquido = 0;

int rpms500 = 500;
int rpms1000 = 1000;
int rpms1500 = 1500;
int rpms2000 = 2000;
int rpms200 = 200;


// VALORES DE PESO PARA EL MULTIUSOS Y UNA CANTIDAD DE 2.7 L 
// PARA TENER 5L DE VOLUMEN A LA HORA DE HECHAR

// Peso Aceite porcentajeAceite * cantidad => 30 * 43 = 1260 g
// Peso Aceite porcentajeAceite * cantidad => 30 * 35 = 1050 g
// Peso Aceite porcentajeAceite * cantidad => 30 * 28 = 840 g
int16_t pesoAceiteMultiusos = 1050;

// Peso Souji porcentajeSouji * cantidad => 50 * 43 = 2150 g
// Peso Souji porcentajeSouji * cantidad => 50 * 35 = 1750 g
// Peso Souji porcentajeSouji * cantidad => 50 * 28 = 1400 g
int16_t pesoSoujiMultiusos = 1750;

// Peso Agua porcentajeAgua * cantidad => 20 * 42 = 840 g
// Peso Agua porcentajeAgua * cantidad => 20 * 35 = 700 g
// Peso Agua porcentajeAgua * cantidad => 20 * 28 = 560 g
int16_t pesoAguaMultiusos = 700;

// VALORES DE PESO PARA EL FREGASUELOS Y UNA CANTIDAD DE 3 L 
// PARA TENER 5L DE VOLUMEN A LA HORA DE HECHAR

// Peso Souji   para 3l porcentajeSouji * cantidad => 43.8 * 30 = 1314 g
// Peso Souji   para 4l porcentajeSouji * cantidad => 43.8 * 40 = 1752 g
// Peso Souji   para 4.2l porcentajeSouji * cantidad => 43.8 * 42 = 1839 g
// Peso Souji   para 3.5l porcentajeSouji * cantidad => 43.8 * 35 = 1533 g
// Peso Souji   para 2.8l porcentajeSouji * cantidad => 43.8 * 28 = 1225 g
int16_t pesoSoujiFregasuelos = 1533;  

// Peso Agua para 3L porcentajeAgua * cantidad => 25.1 * 30 = 753 g
// Peso Agua para 3L porcentajeAgua * cantidad => 25.1 * 40 = 1004 g
// Peso Agua para 4.2L porcentajeAgua * cantidad => 25.1 * 42 = 1054 g
// Peso Agua para 3.5L porcentajeAgua * cantidad => 25.1 * 35 = 878 g
// Peso Agua para 2.8L porcentajeAgua * cantidad => 25.1 * 28 = 702 g
int16_t pesoAguaFregasuelos = 878;

// Peso Aceite para 3L porcentajeAceite * cantidad => 31.1 * 30 = 933 g
// Peso Aceite para 4L porcentajeAceite * cantidad => 31.1 * 40 = 1244 g
// Peso Aceite para 4.2L porcentajeAceite * cantidad => 31.1 * 42 = 1306 g
// Peso Aceite para 3.5L porcentajeAceite * cantidad => 31.1 * 35 = 1085 g
// Peso Aceite para 2.8L porcentajeAceite * cantidad => 31.1 * 28 = 870 g
int16_t pesoAceiteFregasuelos = 1085;

// TIEMPOS 
uint64_t tErrorBomba = 400000;
uint64_t tAgotado = 30000;
uint64_t tUltimaVariacion;
uint64_t tPasado = 0;
uint64_t tPasadoAgotado;
int16_t nuevoPesoActual = 0;

// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA DE MULTIUSOS
uint64_t tMixMultiusos1 = 120000; 
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA DE MULTIUSOS
uint64_t tMixMultiusos2 = 180000;  
// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA DE FREGASUELOS
uint64_t tMixFregasuelos1 = 120000; 
// 30 segundos => 30000 ms    ::   PARA LA PRIMERA MEZCLA DE DOSIFICACION FREGASUELOS
uint64_t tMixFregasuelosDosif1 = 30000; 
// 30 segundos => 30000 ms    ::   PARA LA SEGUNDA MEZCLA DE DOSIFICACION FREGASUELOS
uint64_t tMixFregasuelosDosif2 = 30000;
// 2 minutos => 120000 ms    ::   PARA LA TERCERA MEZCLA DE DOSIFICACION FREGASUELOS
uint64_t tMixFregasuelosDosif3 = 120000;
// 60 segundos => 60000 ms   ::   PARA LA ULTIMA MEZCLA DE FREGASUELOS
uint64_t tMixFregasuelos2 = 60000; 

// 3 minutos => 180000 ms    ::   PARA EL VACIO POR AHORA 
uint64_t tiempoVacio = 180000 ; // 180000

unsigned long temp = 0;
unsigned long tempSig = 0;
unsigned long temp1 = 0;
unsigned long tempSig1 = 0;

// Porcentajes de inicio de liquidos
//int16_t porcentajeAceite = 30;
//int16_t porcentajeSouji = 50;

Mezclas::Mezclas(SoftwareSerial& serial) : mySerial(serial)
{
  pesoAgregado = 0;
  tiempoInicioVacio = 0 ;
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
  motorMezclador.pararMotor();
}

// LA MEZCLA COMPLETA DE MULTIUSOS
void Mezclas::mezclaMultiusos(int mezclas)
{
  if(!enPausa)
  {
    //EEPROM.get(I_MEZCLAS_ADRESS, i_mezclas);
    //EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);

    Pantallamezcla(10);
    delay(4000);
    // Iniciamos la mezcla 
    if(estado == 0)
    {
      Pantallamezcla(0);
      delay(4000);
      verificarPeso();
      EEPROM.put(STATUS_ADRESS, estado);
    }
    // Calculamos el volumen de cada liquido
    if(estado == 1)
    {
      estado = 2;
      estado2 = 1;
      EEPROM.put(STATUS_ADRESS, estado);
      EEPROM.put(STATUS_2_ADRESS, estado2);
      //calcularVolumen();
    }
    // Pasamos a la mezcla general
    if(estado == 2)
    {
      if(mezclas > 1)
      {
        Pantallamezcla(12);
        for(i_mezclas=0; i_mezclas<mezclas; i_mezclas++)
        {
          EEPROM.put(I_MEZCLAS_ADRESS, i_mezclas);
          // echamos la cantidad adecuada para el Aceite
          if(estado2 == 1)
            {
              Pantallamezcla(12);
              echarLiquido(pesoAceiteMultiusos);
              estado2 = 2;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }  
          // echamos la cantidad adecuada para el SOUJI
          if(estado2 == 2)
            { 
              Pantallamezcla(12);
              echarLiquido(pesoSoujiMultiusos);
              estado2 = 3;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
          // Pasamos a la primera etapa de mezcla con el motor 
          if(estado2 == 3)
            { 
              Pantallamezcla(12);
              motorMezclador.ajustarRpms(tMixMultiusos1, rpms1500);
              estado2 = 4;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
          // echamos la cantidad adecuada de AGUA
          if(estado2 == 4)
            {
              delay(15000);
              Pantallamezcla(12);
              echarLiquido(pesoAguaMultiusos);
              estado2 = 5;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
          // Pasamos a la segunda etapa de mezcla con el motor 
          if(estado2 == 5)
            {
              Pantallamezcla(12);
              motorMezclador.ajustarRpms(tMixMultiusos2, rpms1500);
              delay(2000);
              estado2 = 6;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
          // Pasamos a la etapa del vacio
          if(estado2 == 6)
            {
              Pantallamezcla(13);
              mezclaVacio();
              if(idioma==0)
              {
                lcd.clear();
                delay(20);
                lcd.setCursor(1,0);
                lcd.print("MEZCLA NUM : ");
                lcd.setCursor(14,0);
                lcd.print(i_mezclas+1);
                lcd.setCursor(2,1);
                lcd.print("FINALIZADA");
                estado2 = 1;
                EEPROM.put(STATUS_2_ADRESS, estado2);
                delay(5000);
              }
              else
              {
                lcd.clear();
                delay(20);
                lcd.setCursor(1,0);
                lcd.print("MIX NUMBER : ");
                lcd.setCursor(14,0);
                lcd.print(i_mezclas+1);
                lcd.setCursor(2,1);
                lcd.print("COMPLETED");
                estado2 = 1;
                EEPROM.put(STATUS_2_ADRESS, estado2);
                delay(5000);
              }
            }
        }
        finMezcla = true ;
      }
      else
      {
        motorMezclador.pararMotor();
        // echamos la cantidad adecuada para el Aceite
        if(estado2 == 1)
        {
          Pantallamezcla(12);
          echarLiquido(pesoAceiteMultiusos);
          estado2 = 2;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }  
        // echamos la cantidad adecuada para el SOUJI
        if(estado2 == 2)
        {
          Pantallamezcla(12);
          echarLiquido(pesoSoujiMultiusos);
          estado2 = 3;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la primera etapa de mezcla con el motor 
        if(estado2 == 3)
        { 
          Pantallamezcla(12);
          motorMezclador.ajustarRpms(tMixMultiusos1, rpms1500);
          estado2 = 4;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // echamos la cantidad adecuada de AGUA
        if(estado2 == 4)
        {
          delay(15000);
          Pantallamezcla(12);
          echarLiquido(pesoAguaMultiusos);
          estado2 = 5;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        } 
        // Pasamos a la segunda etapa de mezcla con el motor 
        if(estado2 == 5)
        {
          Pantallamezcla(12);
          // Activamos el motor con los RPMs guardados y el tiempo adecuado
          motorMezclador.ajustarRpms(tMixMultiusos2, rpms1500);
          delay(2000);
          estado2 = 6;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la etapa del vacio
        if(estado2 == 6)
        {
          Pantallamezcla(13);
          mezclaVacio();
          finMezcla = true;
          delay(1000);   
        }
      } 
    }
    // Finalizamos la mezcla
    if(estado == 2 && finMezcla)
    {
      lcd.clear();
      delay(20);
      resetearTodo();
      Pantallamezcla(14);
      menus.finalizarCiclo();
    }
    // Error de verificacion y peso superior a lo normal
    if(estado == 3)
    {
      if(idioma == 0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(1,0);
        lcd.print("ERROR DE  PESO");
        lcd.setCursor(0,1);
        lcd.print("VACIAR  DEPOSITO");
        delay(5000);
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(2,0);
        lcd.print("WEIGHT ERROR");
        lcd.setCursor(1,1);
        lcd.print("EMPTY CLEANER");
        delay(5000);
      }
      resetearTodo();
      lcd.clear();
      delay(20);
      menus.finalizarCiclo();
    }
  }
}

// LA MEZCLA COMPLETA DE FREGASUELOS
void Mezclas::mezclaFRIEGASUELOS(int mezclas)
{
  if(!enPausa)
  {
    //EEPROM.get(I_MEZCLAS_ADRESS, i_mezclas);
    //EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);

    Pantallamezcla(11);
    delay(4000);

    // Iniciamos la mezcla 
    if(estado == 0)
    {
      Pantallamezcla(0);
      delay(2000);
      verificarPeso();
      EEPROM.put(STATUS_ADRESS, estado);
    }
    // Calculamos el volumen de cada liquido
    if(estado == 1)
    {
      estado = 2;
      estado2 = 1;
      EEPROM.put(STATUS_ADRESS, estado);
      EEPROM.put(STATUS_2_ADRESS, estado2);
      //calcularVolumen();
    }
    // Pasamos a la mezcla general
    if(estado == 2)
    {
      if(mezclas > 1)
      {
        motorMezclador.pararMotor();
        Pantallamezcla(12);
        for(i_mezclas=0; i_mezclas<mezclas; i_mezclas++)
        {
            //EEPROM.put(I_MEZCLAS_ADRESS, i_mezclas);
            // echamos la cantidad adecuada para el Souji
            if(estado2 == 1)
            {
              Pantallamezcla(12);
              echarLiquido(pesoSoujiFregasuelos);
              estado2 = 2;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }  
            // echamos la cantidad adecuada para el Agua
            if(estado2 == 2)
            { 
              Pantallamezcla(12);
              echarLiquido(pesoAguaFregasuelos);
              estado2 = 3;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la primera etapa de mezcla con el motor 
            if(estado2 == 3)
            { 
              Pantallamezcla(12);
              motorMezclador.ajustarRpms(tMixFregasuelos1,rpms500);
              estado2 = 4;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // echamos la cantidad adecuada de Aceite
            if(estado2 == 4)
            {
              Pantallamezcla(12);
              echarLiquido(pesoAceiteFregasuelos);
              estado2 = 5;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la segunda etapa de mezcla con el motor 
            if(estado2 == 5)
            {
              Pantallamezcla(12);
              // Activamos el motor con los RPMs guardados y el tiempo adecuado
              motorMezclador.ajustarRpms(tMixFregasuelosDosif1,rpms1000);
              delay(2000);
              motorMezclador.ajustarRpms(tMixFregasuelosDosif2,rpms1500);
              delay(2000);
              motorMezclador.ajustarRpms(tMixFregasuelosDosif3,rpms2000);
              delay(2000);
              motorMezclador.ajustarRpms(tMixFregasuelos2,rpms200);
              delay(2000);
              estado2 = 6;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la etapa del vacio
            if(estado2 == 6)
            {
              Pantallamezcla(13);
              mezclaVacio();
              if(idioma==0)
              {
                lcd.clear();
                delay(20);
                lcd.setCursor(1,0);
                lcd.print("MEZCLA NUM : ");
                lcd.setCursor(14,0);
                lcd.print(i_mezclas+1);
                lcd.setCursor(2,1);
                lcd.print("FINALIZADA");
                estado2 = 1;
                EEPROM.put(STATUS_2_ADRESS, estado2);
                delay(4000);
              }
              else
              {
                lcd.clear();
                delay(20);
                lcd.setCursor(1,0);
                lcd.print("MIX NUMBER : ");
                lcd.setCursor(14,0);
                lcd.print(i_mezclas+1);
                lcd.setCursor(2,1);
                lcd.print("COMPLETED");
                estado2 = 1;
                EEPROM.put(STATUS_2_ADRESS, estado2);
                delay(4000);
              }
            }
        }
        finMezcla = true ;
      }
      else
      {
        // echamos la cantidad adecuada para el Souji
        if(estado2 == 1)
        {
          Pantallamezcla(12);
          echarLiquido(pesoSoujiFregasuelos);
          estado2 = 2;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }  
        // echamos la cantidad adecuada para el Agua
        if(estado2 == 2)
        {
          Pantallamezcla(12);
          echarLiquido(pesoAguaFregasuelos);
          estado2 = 3;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la primera etapa de mezcla con el motor 
        if(estado2 == 3)
        { 
          Pantallamezcla(12);
          motorMezclador.ajustarRpms(tMixFregasuelos1,rpms500);
          estado2 = 4;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // echamos la cantidad adecuada del Aceite
        if(estado2 == 4)
        {
          delay(15000);
          Pantallamezcla(12);
          echarLiquido(pesoAceiteFregasuelos);
          estado2 = 5;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        } 
        // Pasamos a la segunda etapa de mezcla con el motor 
        if(estado2 == 5)
        {
          Pantallamezcla(12);
          // Activamos el motor con los RPMs guardados y el tiempo adecuado
          motorMezclador.ajustarRpms(tMixFregasuelosDosif1,rpms1000);
          delay(2000);
          motorMezclador.ajustarRpms(tMixFregasuelosDosif2,rpms1500);
          delay(2000);
          motorMezclador.ajustarRpms(tMixFregasuelosDosif3,rpms2000);
          delay(2000);
          motorMezclador.ajustarRpms(tMixFregasuelos2,rpms200);
          delay(2000);
          estado2 = 6;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la etapa del vacio
        if(estado2 == 6)
        {
          Pantallamezcla(13);
          mezclaVacio();
          finMezcla = true; 
        }
      } 
    }
    // Finalizamos la mezcla
    if(estado == 2 && finMezcla)
    {
      lcd.clear();
      delay(20);
      resetearTodo();
      Pantallamezcla(14);
      menus.finalizarCiclo();
    }
    // Error de verificacion y peso superior a lo normal
    if(estado == 3)
    {
      if(idioma == 0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(1,0);
        lcd.print("ERROR DE  PESO");
        lcd.setCursor(0,1);
        lcd.print("VACIAR  DEPOSITO");
        delay(4000);
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(2,0);
        lcd.print("WEIGHT ERROR");
        lcd.setCursor(1,1);
        lcd.print("EMPTY CLEANER");
        delay(4000);
      }
      resetearTodo();
      menus.finalizarCiclo();
    }
  }
}

void Mezclas::Pantallamezcla(uint8_t pantallamezcla)
{
  // Limpieza de la pantalla 
  if(pantallamezcla != misPantallasMezc)
  {
    lcd.clear();
    delay(20);
    misPantallasMezc = pantallamezcla;
  }

  // Pantalla de inicio de mezcla
  if (pantallamezcla == 0)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("COMIENZA");
      lcd.setCursor(2,1);
      lcd.print("LA MAGIA !!!");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("THE  MAGIC");
      lcd.setCursor(3,1);
      lcd.print("BEGINS !!!");
    }
  }

  // Pantalla de DISPENSANDO
  if (pantallamezcla == 1)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("DISPENSANDO ...");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("DISPENSING ...");
    }
  }

  // Pantalla de DEPOSITO DISPENSADO
  if (pantallamezcla == 2)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("LIMPIADOR");
      lcd.setCursor(1,1);
      lcd.print("LISTO POR USAR");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("CLEANER");
      lcd.setCursor(2,1);
      lcd.print("READY TO USE");
    }
  }

  // Pantalla de fin de mezcla
  if (pantallamezcla == 7)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("MEZCLA TOTAL");
      lcd.setCursor(3,1);
      lcd.print("COMPLETADA");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("TOTAL MIX");
      lcd.setCursor(3,1);
      lcd.print("COMPLETED");
    }
  }

  // Pantalla de PAUSA
  if (pantallamezcla == 8)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("EN PAUSA");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("IN PAUSE");
    }
  }

  // Pantalla de MULTIUSOS
  if (pantallamezcla == 10)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("MEZCLA LIMPIADOR");
      lcd.setCursor(0,1);
      lcd.print("MULTISUPERFICIES");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(9,0);
      lcd.print("MIX");
      lcd.setCursor(1, 1);
      lcd.print("MULTI PURPOSE");
    }
  }

  // Pantalla de FRIEGASUELOS
  if (pantallamezcla == 11)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("MEZ. LAVAVJILLAS");
      lcd.setCursor(5, 1);
      lcd.print("MANUAL");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(9,0);
      lcd.print("MIX");
      lcd.setCursor(1, 1);
      lcd.print("FLOOR WASHING");
    }
  }
  

  // Pantalla Mezclando
  if (pantallamezcla == 12)
  {  
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("MEZCLANDO ....");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("MIXING ....");
    }
  }

  // Pantalla LIMPIAR O VACIAR
  if (pantallamezcla == 13)
  {  
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("FABRICANDO");
      lcd.setCursor(2,1);
      lcd.print("EL LIMPIADOR");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(5,0);
      lcd.print("MAKING");
      lcd.setCursor(2,1);
      lcd.print("THE  CLEANER");
    }
  }

  // Pantalla LA MAGIA ESTA HECHA
  if (pantallamezcla == 14)
  {  
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("LA MAGIA ESTA");
      lcd.setCursor(5,1);
      lcd.print("HECHA !!");
      delay(4000);
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("EL LIMPIADOR");
      lcd.setCursor(1,1);
      lcd.print("ESTA LISTO  !!");
      delay(4000);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("THE MAGIC IS");
      lcd.setCursor(4,1);
      lcd.print("DONE  !!");
      delay(4000);
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("THE  CLEANER");
      lcd.setCursor(2,1);
      lcd.print("IS READY  !!");
      delay(4000);
    }
  }
}

void Mezclas::mezclaVacio()
{
  Pantallamezcla(13);
  int16_t elPesoMinimo = 5;

  if (PesoActual() <= elPesoMinimo)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("PESO MINIMO");
      delay(8000);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("WHEIGHT LIMIT");
      delay(8000);
    }
    return;
  }
  unsigned long tiempoInicio = millis(); 
  
  bombaVacio.on();

  while(PesoActual() > elPesoMinimo)
  {
    deteccionPulso();
    Pantallamezcla(13);
    
    /*temp1 = millis();
    if(temp1-tempSig1 >= 1000)
    {
      lcd.clear();
      delay(20);
      tempSig1 = temp1;
    }
    */
    if (millis() - tiempoInicio > tErrorBomba)
    {
      if(idioma==0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(1,0);
        lcd.print("ERROR BOMBA");
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(3,0);
        lcd.print("POMP ERROR");
      }
      delay(2000);
      bombaVacio.off();
      return;
    }
    deteccionPulso();
  } 
  bombaVacio.off();
}

/*
void Mezclas::calcularVolumen()
{ 
  // EL 70% de la cantidad de 5 litros por ahora que es 3500 --> 35

  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  volumenAceite= porcentajeAceite * 27;
  pesoAceiteMultiusos  = volumenAceite * DENSIDAD_ACEITE;
  EEPROM.put(VOL_ACEITE_ADRESS, pesoAceiteMultiusos);
  delay(1000);

  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  volumenSouji = porcentajeSouji * 27;
  pesoSoujiMultiusos = volumenSouji * DENSIDAD_SOUJI;
  EEPROM.put(VOL_SOUJI_ADRESS, pesoSoujiMultiusos);
  delay(1000);

  // Calculos de la cantidad de Agua que es lo que queda
  int16_t porcentajeAgua = 0;
  porcentajeAgua  = 100 - (porcentajeAceite + porcentajeSouji);
  volumenAgua = porcentajeAgua * 24;
  pesoAguaMultiusos = volumenAgua;
  EEPROM.put(VOL_AGUA_ADRESS, pesoAguaMultiusos);
  delay(1000);
}
*/

void Mezclas::echarLiquido(int16_t pesoPorechar)
{
  // Encender la bomba correspondiente
  encenderBombaCorrespondiente();

  tUltimaVariacion = millis();
  pesoLiquido = PesoActual();
  pesoRelative = 0;

  EEPROM.get(PESO_RELATIVO_ADDRESS, pesoRelative);
  Pantallamezcla(12);

  while(pesoRelative < pesoPorechar)
  { 
    deteccionPulso();
    Pantallamezcla(12);
    nuevoPesoActual = PesoActual();
    
    if(estado2 == 4)
    {
      delay(50);
    }
    else
    {
      temp = millis();
      if(temp - tempSig >= 3000)
      {
        deteccionPulso();
        tempSig = temp;
      }
    }
    deteccionPulso();
    if (abs(nuevoPesoActual - pesoLiquido) > 50) 
    {
      tUltimaVariacion = millis(); 
      pesoRelative += nuevoPesoActual - pesoLiquido;
      pesoLiquido = nuevoPesoActual;
      EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative);
      tPasadoAgotado = millis() -  tUltimaVariacion;
    }
    deteccionPulso();
    // Verificar si no ha habido cambios significativos durante más de 30 segundos
    if (millis() - tUltimaVariacion > tAgotado) 
    {
      mostrarAgotado();
      apagarBombas();

      // Esperar hasta que el usuario presione el botón de inicio para continuar
      while (!botonPausa.pulsado())
      {
        delay(300);
      }

      // Reiniciar la bomba correspondiente
      encenderBombaCorrespondiente();

      // Reiniciar el tiempo de última variación
      tUltimaVariacion = millis();
    }
    deteccionPulso();
  }

  apagarBombas();

  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, 0);

  lcd.clear();
  delay(20);
  lcd.setCursor(0,1);
  lcd.print("PESO : ");
  lcd.setCursor(8,1);      
  lcd.print(pesoRelative);
  delay(2000);
}

/*
void Mezclas::subirPorcentajeAceite()
{
  porcentajeAceite++;
  EEPROM.put(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  delay(100);
  menus.validarAjusteAceite = true;
  Pantallamezcla(10);
}

void Mezclas::subirPorcentajeSouji()
{
  porcentajeSouji++;
  EEPROM.put(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  delay(100);
  menus.validarAjusteSouji = true;
  Pantallamezcla(11);
}

void Mezclas::bajarPorcentajeAceite()
{
  porcentajeAceite--;
  EEPROM.put(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  delay(100);
  menus.validarAjusteAceite = true;
  Pantallamezcla(10);
}

void Mezclas::bajarPorcentajeSouji()
{
  porcentajeSouji--;
  EEPROM.put(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  delay(100);
  menus.validarAjusteSouji = true;
  Pantallamezcla(11);
}
*/

void Mezclas::resetearTodo()
{
  estado = 0;
  estado2 = 0;
  numMezclas = 1;
  pesoLiquido = 0;
  pesoRelative = 0;
  EEPROM.put(STATUS_ADRESS, estado);
  EEPROM.put(STATUS_2_ADRESS, estado2);
  //EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
  //EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  //EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative);
  finMezcla = false;
  enPausa = false;
}

void Mezclas::deteccionPulso()
{
  if(botonPausa.pulsado())
  {
    pausarMezcla();

    while (enPausa) 
    {
      if (botonPausa.pulsado()) 
      {
        delay(20); 
        enPausa = false; 
      }
      delay(100); 
    }
    tUltimaVariacion  += (millis() - tUltimaVariacion) - tPasadoAgotado;
    encenderBombaCorrespondiente();
  }
}

void Mezclas::pausarMezcla()
{
  apagarBombas();
  Pantallamezcla(8);
  enPausa = true;
}

void Mezclas::esperarParaReanudar()
{
  while (enPausa)
  {
    // Esperar hasta que se reanude la mezcla
    if (botonPausa.pulsado())
    {
      lcd.clear();
      delay(20);    
    }
  }
}

void Mezclas::encenderBombaCorrespondiente()
{
  if(tipoMezcla == 0)
  {
    switch (estado2)
    {
      case 1:
        bombaAgua.on();
        break;
      case 2:
        bombaSouji.on();
        break;
      case 4:
        bombaAceite.on();
        break;
      case 6:
        bombaVacio.on();
        break;
    }
  }
  else
  {
    switch (estado2)
    {
      case 1:
        bombaSouji.on();
        break;
      case 2:
        bombaAceite.on();
        break;
      case 4:
        bombaAgua.on();
        break;
      case 6:
        bombaVacio.on();
        break;
    }
  }
}

void Mezclas::apagarBombas()
{
  if(tipoMezcla == 0)
  {
    switch (estado2)
    {
      case 1:
        bombaAgua.off();
        break;
      case 2:
        bombaSouji.off();
        break;
      case 4:
        bombaAceite.off();
        break;
      case 6:
        bombaVacio.off();
        break;
    }
  }
  else
  {
    switch (estado2)
    {
      case 1:
        bombaSouji.off();
        break;
      case 2:
        bombaAceite.off();
        break;
      case 4:
        bombaAgua.off();
        break;
      case 6:
        bombaVacio.off();
        break;
    }
  }
}

void Mezclas::mostrarAgotado()
{
  lcd.clear();
  delay(20);
  if(tipoMezcla == 0)
  {
    switch(estado2)
    {
      case 1:
      if(idioma==0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("LLENAR DEPOSITO");
        lcd.setCursor(2,1);
        lcd.print("ACEITE USADO");
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(3,0);
        lcd.print("FILL USED");
        lcd.setCursor(4,1);
        lcd.print("OIL TANK");
      }
      break;
      case 2:
      if(idioma==0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("LLENAR DEPOSITO");
        lcd.setCursor(4,1);
        lcd.print("DE SOUJI");
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(3,0);
        lcd.print("FILL SOUJI");
        lcd.setCursor(6,1);
        lcd.print("TANK");
      }
      break;
      case 4:
      if(idioma==0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("LLENAR DEPOSITO");
        lcd.setCursor(4,1);
        lcd.print("DE AGUA");
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(3,0);
        lcd.print("FILL WATER");
        lcd.setCursor(6,1);
        lcd.print("TANK");
      }
      break;
    }
  }
  else
  {
    switch(estado2)
    {
      case 1:
        if(idioma==0)
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("LLENAR DEPOSITO");
          lcd.setCursor(4,1);
          lcd.print("DE SOUJI");
        }
        else
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(3,0);
          lcd.print("FILL SOUJI");
          lcd.setCursor(6,1);
          lcd.print("TANK");
        }
        break; 
      case 2:
        if(idioma==0)
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("LLENAR DEPOSITO");
          lcd.setCursor(4,1);
          lcd.print("DE AGUA");
        }
        else
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(3,0);
          lcd.print("FILL WATER");
          lcd.setCursor(6,1);
          lcd.print("TANK");
        }
        break;
      case 4:
        if(idioma==0)
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("LLENAR DEPOSITO");
          lcd.setCursor(2,1);
          lcd.print("ACEITE USADO");
        }
        else
        {
          lcd.clear();
          delay(20);
          lcd.setCursor(3,0);
          lcd.print("FILL USED");
          lcd.setCursor(4,1);
          lcd.print("OIL TANK");
        }
        break;
    }
  }
}

void Mezclas::vacioGeneral()
{
  Pantallamezcla(1);
  tiempoInicioVacio = millis();
  uint64_t tPasado = 0;
  bombaVacio.on();
  while (tPasado < tiempoVacio)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("DISPENSANDO ...");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("DISPENSING ...");
    }

    tPasado = millis() - tiempoInicioVacio;
    if (botonPausa.pulsado())
    {
      bombaVacio.off(); 
      Pantallamezcla(2);
      delay(4000);
      menus.PantallaSeleccionada(4);  
      menus.inSubMenu = false;
      menus.updateMenuDisplay();
      break;
    }
    delay(1000);
    lcd.clear();
    delay(20);
    //updateProgressBar(tPasado, tiempoVacio, 1);
    //delay(1000);
  }
  bombaVacio.off();
  Pantallamezcla(2);
  delay(4000);  
}

void Mezclas::verificarPeso()
{
  peso=PesoActual();
  if(peso>300)
  {
    estado = 3;
  }
  else
  {
    estado = 1;
  }
}