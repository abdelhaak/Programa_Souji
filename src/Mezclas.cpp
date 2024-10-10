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

// ERRORES

// iniciado en 3.5 minutos => 500000 // POR AHORA 15 SEGUNDOS
uint64_t tiempoErrorBascula = 800000;
uint64_t tiempoErrorBomba = 800000;
uint64_t tiempoAgotado = 60000;
uint64_t tiempoUltimaVariacion;
uint64_t tiempoPasado = 0;
uint64_t tiempoPasadoAgotado;
int16_t nuevoPesoActual = 0;

// 2 minutos => 240000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 240000  ; // 
// 3 minutos => 360000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 360000 ; //10000   ;
// 3 minutos => 360000 ms    ::   PARA EL VACIO POR AHORA 
uint64_t tiempoVacio = 360000 ; // 10000

unsigned long temp = 0;
unsigned long tempSig = 0;
unsigned long temp1 = 0;
unsigned long tempSig1 = 0;
// Porcentajes de inicio de liquidos
int16_t porcentajeAceite = 30;
int16_t porcentajeSouji = 50;

Mezclas::Mezclas(SoftwareSerial& serial) : mySerial(serial)
{
  pesoAceiteDeseado = 0;
  pesoSoujiDeseado = 0;
  volumenAceite = 0;
  volumenSouji = 0;
  volumenAgua = 0;
  pesoAgregado = 0;
  porcentajeAceite = 30;
  porcentajeSouji = 50;
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

// LA MEZCLA COMPLETA 
void Mezclas::mezclaGeneral(int mezclas)
{
  if(!enPausa)
  {
    EEPROM.get(I_MEZCLAS_ADRESS, i_mezclas);
    EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
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
      calcularVolumen();
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
              EEPROM.get(VOL_ACEITE_ADRESS, pesoAceiteDeseado);
              Pantallamezcla(12);
              echarLiquido(pesoAceiteDeseado);
              estado2 = 2;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }  
            // echamos la cantidad adecuada para el SOUJI
            if(estado2 == 2)
            { 
              EEPROM.get(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
              Pantallamezcla(12);
              echarLiquido(pesoSoujiDeseado);
              estado2 = 3;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la primera etapa de mezcla con el motor 
            if(estado2 == 3)
            { 
              Pantallamezcla(12);
              motorMezclador.ajustarRpms(tiempoMezcla1);
              delay(50);
              estado2 = 4;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // echamos la cantidad adecuada de AGUA
            if(estado2 == 4)
            {
              EEPROM.get(VOL_AGUA_ADRESS, pesoAguaDeseado);
              delay(30000);
              Pantallamezcla(12);
              echarLiquido(pesoAguaDeseado);
              estado2 = 5;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la segunda etapa de mezcla con el motor 
            if(estado2 == 5)
            {
              Pantallamezcla(12);
              // Activamos el motor con los RPMs guardados y el tiempo adecuado
              motorMezclador.ajustarRpms(tiempoMezcla2);
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
                delay(10000);
                lcd.clear();
                delay(20);
                lcd.setCursor(0,0);
                lcd.print("SIGUIENTE MEZCLA");
                delay(8000);
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
                delay(10000);
                lcd.clear();
                delay(20);
                lcd.setCursor(4,0);
                lcd.print("NEXT MIX");
                delay(8000);
              }
            }
        }
        finMezcla = true ;
      }
      else
      {
        // echamos la cantidad adecuada para el Aceite
        if(estado2 == 1)
        {
          EEPROM.get(VOL_ACEITE_ADRESS, pesoAceiteDeseado);
          Pantallamezcla(12);
          echarLiquido(pesoAceiteDeseado);
          estado2 = 2;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }  
        // echamos la cantidad adecuada para el SOUJI
        if(estado2 == 2)
        {
          EEPROM.get(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
          Pantallamezcla(12);
          echarLiquido(pesoSoujiDeseado);
          estado2 = 3;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la primera etapa de mezcla con el motor 
        if(estado2 == 3)
        { 
          Pantallamezcla(12);
          motorMezclador.ajustarRpms(tiempoMezcla1);
          delay(50);
          estado2 = 4;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // echamos la cantidad adecuada de AGUA
        if(estado2 == 4)
        {
          EEPROM.get(VOL_AGUA_ADRESS, pesoAguaDeseado);
          delay(30000);
          Pantallamezcla(12);
          echarLiquido(pesoAguaDeseado);
          estado2 = 5;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        } 
        // Pasamos a la segunda etapa de mezcla con el motor 
        if(estado2 == 5)
        {
          Pantallamezcla(12);
          // Activamos el motor con los RPMs guardados y el tiempo adecuado
          motorMezclador.ajustarRpms(tiempoMezcla2);
          estado2 = 6;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la etapa del vacio
        if(estado2 == 6)
        {
          Pantallamezcla(13);
          mezclaVacio();
          finMezcla = true;
          delay(2000);   
        }
      } 
    }
    // Finalizamos la mezcla
    if(estado == 2 && finMezcla)
    {
      resetearTodo();
      Pantallamezcla(14);
      Pantallamezcla(7);
      delay(8000);
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
        delay(10000);
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(2,0);
        lcd.print("WEIGHT ERROR");
        lcd.setCursor(1,1);
        lcd.print("EMPTY CLEANER");
        delay(10000);
      }
      resetearTodo();
      lcd.clear();
      delay(20);
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

  // Pantalla de Ajuste de PORCENTAJE ACEITE
  if (pantallamezcla == 10)
  {
    //menus.validarAjusteAceite = true;
    EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("ADJUSTING  OIL");
      lcd.setCursor(5,1);
      lcd.print(porcentajeAceite);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
  }

  // Pantalla de Ajuste de PORCENTAJE SOUJI
  if (pantallamezcla == 11)
  {
    
    EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("AJUSTANDO  SOUJI");
      lcd.setCursor(5,1);
      lcd.print(porcentajeSouji);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("ADJUSTING  SOUJI");
      lcd.setCursor(5,1);
      lcd.print(porcentajeSouji);
      lcd.setCursor(10,1);
      lcd.print("%");
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
      lcd.setCursor(4,1);
      lcd.print("HECHA !!");
      delay(10000);
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("EL LIMPIADOR");
      lcd.setCursor(1,1);
      lcd.print("ESTA LISTO  !!");
      delay(10000);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("THE MAGIC IS");
      lcd.setCursor(4,1);
      lcd.print("DONE  !!");
      delay(10000);
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("THE  CLEANER");
      lcd.setCursor(2,1);
      lcd.print("IS READY  !!");
      delay(10000);
    }
  }
}

void Mezclas::mezclaVacio()
{
  Pantallamezcla(13);
  int16_t elPesoMinimo = 120;
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

  lcd.clear();
  delay(20);

  while(PesoActual() > elPesoMinimo)
  {
    deteccionPulso();
    Pantallamezcla(13);

    if (millis() - tiempoInicio > tiempoErrorBomba)
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
      delay(8000);
      bombaVacio.off();
      return;
    }
    /*temp1 = millis();
    if(temp1 - tempSig1 >= 3000)
    {
      deteccionPulso();
      tempSig1 = temp1;
    }
    lcd.clear();
    delay(20);*/
    deteccionPulso();
  }  
  bombaVacio.off();
}

void Mezclas::calcularVolumen()
{ 
  // EL 70% de la cantidad de 5 litros por ahora que es 3500 --> 35

  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  volumenAceite= porcentajeAceite * 27;
  pesoAceiteDeseado  = volumenAceite * DENSIDAD_ACEITE;
  EEPROM.put(VOL_ACEITE_ADRESS, pesoAceiteDeseado);
  delay(1000);

  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  volumenSouji = porcentajeSouji * 27;
  pesoSoujiDeseado = volumenSouji * DENSIDAD_SOUJI;
  EEPROM.put(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
  delay(1000);

  // Calculos de la cantidad de Agua que es lo que queda
  int16_t porcentajeAgua = 0;
  porcentajeAgua  = 100 - (porcentajeAceite + porcentajeSouji);
  volumenAgua = porcentajeAgua * 24;
  pesoAguaDeseado = volumenAgua;
  EEPROM.put(VOL_AGUA_ADRESS, pesoAguaDeseado);
  delay(1000);
}

/*
void Mezclas::echarLiquido(int16_t pesoPorechar)
{
  encenderBombaCorrespondiente();

  tiempoUltimaVariacion = millis();
  pesoLiquido = PesoActual();
  pesoRelative = 0;

  EEPROM.get(PESO_RELATIVO_ADDRESS, pesoRelative);
  Pantallamezcla(12);

  unsigned long tiempoUltimaLectura = millis(); 
  const unsigned long intervaloLectura = 2000;
  const int16_t maxPesoEsperado = 50;

  while(pesoRelative < pesoPorechar)
  { 
    deteccionPulso();
    Pantallamezcla(12);
    
    unsigned long tiempoActual = millis();
    
    if (tiempoActual - tiempoUltimaLectura >= intervaloLectura)
    {
      if(PesoActual() - pesoRelative < maxPesoEsperado)
      {
        nuevoPesoActual = PesoActual(); 
      }
      else
      {
        nuevoPesoActual = pesoRelative;
      }
      
      tiempoUltimaLectura = tiempoActual;  

      if (abs(nuevoPesoActual - pesoLiquido) > 50) 
      {
        tiempoUltimaVariacion = millis();  
        pesoRelative += nuevoPesoActual - pesoLiquido;  
        pesoLiquido = nuevoPesoActual; 
        EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative); 
        tiempoPasadoAgotado = millis() - tiempoUltimaVariacion;
      }
    }

    if (millis() - tiempoUltimaVariacion > tiempoAgotado) 
    {
      mostrarAgotado();
      apagarBombas();

      while (!botonPausa.pulsado())
      {
        delay(300);
      }

      encenderBombaCorrespondiente();
      tiempoUltimaVariacion = millis();
    }

    deteccionPulso();  

  }

  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, 0);  
  
  apagarBombas();
  delay(1000); 
}
*/
void Mezclas::echarLiquido(int16_t pesoPorechar)
{
  // Encender la bomba correspondiente
  encenderBombaCorrespondiente();

  tiempoUltimaVariacion = millis();
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
      tiempoUltimaVariacion = millis(); 
      pesoRelative += nuevoPesoActual - pesoLiquido;
      pesoLiquido = nuevoPesoActual;
      EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative);
      tiempoPasadoAgotado = millis() -  tiempoUltimaVariacion;
    }
    deteccionPulso();
    // Verificar si no ha habido cambios significativos durante más de 30 segundos
    if (millis() - tiempoUltimaVariacion > tiempoAgotado) 
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
      tiempoUltimaVariacion = millis();
    }
    deteccionPulso();
  }

  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, 0);
  
  apagarBombas();

  /*lcd.clear();
  delay(20);
  lcd.setCursor(0,1);
  lcd.print("PESO : ");
  lcd.setCursor(8,1);      
  lcd.print(pesoLiquido);
  delay(6000);*/
}

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


void Mezclas::resetearTodo()
{
  estado = 0;
  estado2 = 0;
  numMezclas = 1;
  volumenAceite = 0;
  volumenSouji = 0;
  volumenAgua = 0;
  pesoLiquido = 0;
  pesoRelative = 0;
  EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
  EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);
  EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);
  EEPROM.put(STATUS_ADRESS, estado);
  EEPROM.put(STATUS_2_ADRESS, estado2);
  EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative);
  finMezcla = false;
  enPausa = false;
}

void Mezclas::deteccionPulso()
{
  if(botonPausa.pulsado())
  {
      pausarMezcla();

      // Espera a que se salga del estado de pausa
      while (enPausa) 
      {
        if (botonPausa.pulsado()) 
        {
          delay(20); 
          enPausa = false; 
        }
        delay(100); 
      }
      tiempoUltimaVariacion  += (millis() - tiempoUltimaVariacion) - tiempoPasadoAgotado;
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
  switch (estado2)
  {
    case 1:
      bombaAceite.on();
      break;
    case 2:
      bombaSouji.on();
      break;
    case 4:
      bombaAgua.on();
      break;
    case 6:
      bombaVacio.on();
      break;
  }
}

void Mezclas::apagarBombas()
{
   switch (estado2)
  {
    case 1:
      bombaAceite.off();
      break;
    case 2:
      bombaSouji.off();
      break;
    case 4:
      bombaAgua.off();
      break;
    case 6:
      bombaVacio.off();
      break;
  }
}


void Mezclas::mostrarAgotado()
{
  lcd.clear();
  delay(20);
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

void Mezclas::vacioGeneral()
{
  Pantallamezcla(1);
  tiempoInicioVacio = millis();
  uint64_t tiempoPasado = 0;
  bombaVacio.on();
  while (tiempoPasado < tiempoVacio)
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

    tiempoPasado = millis() - tiempoInicioVacio;
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
    //updateProgressBar(tiempoPasado, tiempoVacio, 1);
    //delay(1000);
  }
  bombaVacio.off();
  Pantallamezcla(2);
  delay(8000);  
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