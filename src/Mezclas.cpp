#include "Mezclas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,pin_encoder);
Menus menus(lcd,mySerial);


//SoftwareSerial mySerial(rxPin, txPin);

Boton botonPausa(PIN_BOTON_SEL);

int estado = 0;
int estado2 = 0;
int numMezclas = 0;
int i_mezclas=0;
int16_t pesoLiquido = 0;
//bool enPausa = false;


// ERRORES

// El tiempo de error autorizado de la bascula 
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
// El peso minimo del vacio autorizado
//int16_t pesoMinimo = 20;


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
      //estado = 1;
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
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("HACEMOS:");
        lcd.setCursor(0,1);
        lcd.print(mezclas);
        lcd.setCursor(3,1);
        lcd.print("MEZCLAS:");
        delay(6000);
        for(i_mezclas=0; i_mezclas<mezclas; i_mezclas++)
        {
            EEPROM.put(I_MEZCLAS_ADRESS, i_mezclas);
            // echamos la cantidad adecuada para el Aceite
            if(estado2 == 1)
            {
              EEPROM.get(VOL_ACEITE_ADRESS, pesoAceiteDeseado);
              lcd.clear();
              delay(20);
              lcd.setCursor(0,0);
              lcd.print("ECHAMOS ACEITE :");
              delay(4000);
              Pantallamezcla(1);
              delay(20);
              echarLiquido(pesoAceiteDeseado);
              estado2 = 2;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }  
            // echamos la cantidad adecuada para el SOUJI
            if(estado2 == 2)
            { 
              EEPROM.get(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
              lcd.clear();
              delay(20);
              lcd.setCursor(0,0);
              lcd.print("ECHAMOS SOUJI :");
              delay(4000);
              Pantallamezcla(2);
              delay(100);
              echarLiquido(pesoSoujiDeseado);
              estado2 = 3;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la primera etapa de mezcla con el motor 
            if(estado2 == 3)
            { 
              Pantallamezcla(4);
              motorMezclador.ajustarRpms(tiempoMezcla1);
              // Apagar el motor y pasar a la siguiente etapa
              motorMezclador.pararMotor();
              delay(50);
              estado2 = 4;
              EEPROM.put(STATUS_2_ADRESS, estado2);
              delay(50);
            }
            // echamos la cantidad adecuada de AGUA
            if(estado2 == 4)
            {
              EEPROM.get(VOL_AGUA_ADRESS, pesoAguaDeseado);
              delay(50);
              lcd.clear();
              delay(20);
              lcd.setCursor(0,0);
              lcd.print("ECHAMOS AGUA :");
              delay(4000);
              Pantallamezcla(3);
              echarLiquido(pesoAguaDeseado);
              estado2 = 5;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la segunda etapa de mezcla con el motor 
            if(estado2 == 5)
            {
              delay(50);
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
              Pantallamezcla(6);
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
                lcd.setCursor(2,0);
                lcd.print("PASAMOS A LA");
                lcd.setCursor(0,1);
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
                lcd.setCursor(3,0);
                lcd.print("WE MOVE TO");
                lcd.setCursor(2,1);
                lcd.print("THE NEXT MIX");
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
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("ECHAMOS ACEITE :");
          delay(4000);
          Pantallamezcla(1);
          delay(20);
          echarLiquido(pesoAceiteDeseado);
          estado2 = 2;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }  
        // echamos la cantidad adecuada para el SOUJI
        if(estado2 == 2)
        {
          EEPROM.get(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("ECHAMOS SOUJI :");
          delay(4000);
          Pantallamezcla(2);
          delay(100);
          echarLiquido(pesoSoujiDeseado);
          estado2 = 3;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la primera etapa de mezcla con el motor 
        if(estado2 == 3)
        { 
          Pantallamezcla(4);
          motorMezclador.ajustarRpms(tiempoMezcla1);
          // Apagar el motor y pasar a la siguiente etapa
          motorMezclador.pararMotor();
          delay(50);
          estado2 = 4;
          EEPROM.put(STATUS_2_ADRESS, estado2);
          delay(50);
        }
        // echamos la cantidad adecuada de AGUA
        if(estado2 == 4)
        {
          EEPROM.get(VOL_AGUA_ADRESS, pesoAguaDeseado);
          delay(50);
          lcd.clear();
          delay(20);
          lcd.setCursor(0,0);
          lcd.print("ECHAMOS AGUA :");
          delay(30000);
          Pantallamezcla(3);
          echarLiquido(pesoAguaDeseado);
          estado2 = 5;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        } 
        // Pasamos a la segunda etapa de mezcla con el motor 
        if(estado2 == 5)
        {
          delay(50);
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
          Pantallamezcla(6);
          mezclaVacio();
          if(idioma==0)
          {
            lcd.clear();
            delay(10);
            lcd.setCursor(4,0);
            lcd.print("MEZCLA ");
            lcd.setCursor(2,1);
            lcd.print("FINALIZADA");
            finMezcla = true ;
          }
          else
          {
            lcd.clear();
            delay(10);
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
      resetearTodo();
      Pantallamezcla(7);
      delay(8000);
      menus.finalizarCiclo();
      //menus.PantallaSeleccionada(0);
    }
    // Error de verificacion y peso superior a lo normal
    if(estado == 3)
    {
      if(idioma == 0)
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("ERROR DE PESO");
        lcd.setCursor(0,1);
        lcd.print("VACIAR DEPOSITO");
        delay(10000);
      }
      else
      {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("WEIGHT ERROR");
        lcd.setCursor(0,1);
        lcd.print("EMPTY TANK");
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
      lcd.setCursor(0,0);
      lcd.print("AHORA EMPEZAMOS");
      lcd.setCursor(0,1);
      lcd.print("LA MEZCLA");
      lcd.setCursor(12,1);
      lcd.createChar(0, Character2);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("NOW STARTING");
      lcd.setCursor(5,1);
      lcd.print("THE MIX");
      lcd.setCursor(12,1);
      lcd.createChar(0, Character2);
    }
  }

  // Pantalla ECHANDO Aceite
  if (pantallamezcla == 1)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("ECHANDO ACEITE");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("MAKING OIL");
    }
  }

  // Pantalla ECHANDO Souji
  if (pantallamezcla == 2)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("ECHANDO SOUJI");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("MAKING SOUJI");
    }
  }

  // Pantalla ECHANDO Agua
  if (pantallamezcla == 3)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("ECHANDO AGUA");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("PRIMERA MEZCLA");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("SEGUNDA MEZCLA");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("VACIANDO ....");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      lcd.setCursor(3,0);
      lcd.print("ESTAMOS EN");
      lcd.setCursor(5,1);
      lcd.print("PAUSA ..");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("WE ARE IN");
      lcd.setCursor(5,1);
      lcd.print("PAUSE ...");
    }
  }

  // Pantalla de REANUDAR
  if (pantallamezcla == 9)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("ESTAMOS ");
      lcd.setCursor(0,1);
      lcd.print("REANUDANDO ...");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(5,0);
      lcd.print("WE ARE ");
      lcd.setCursor(1,1);
      lcd.print("RESUMING ...");
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
      lcd.setCursor(0,0);
      lcd.print("ADJUSTNIG OIL");
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
      lcd.print("AJUSTANDO SOUJI");
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
      lcd.print("ADJUSTNIG SOUJI");
      lcd.setCursor(5,1);
      lcd.print(porcentajeSouji);
      lcd.setCursor(10,1);
      lcd.print("%");
    }
  }
}

void Mezclas::mezclaVacio()
{
  int16_t elPesoMinimo = 120;
  if (PesoActual() <= elPesoMinimo)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("PESO MINIMO");
      lcd.setCursor(0,1);
      lcd.print("ADQUIRIDO");
      delay(8000);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("WHEIGHT LIMIT");
      lcd.setCursor(3,1);
      lcd.print("ARRIVED");
      delay(8000);
    }
    return;
  }
  unsigned long tiempoInicio = millis(); 
  int16_t pesoInicial = PesoActual();
  int16_t pesoVaciado;
  int16_t pesoActual; 
  int16_t pesoTotalAVaciar = pesoInicial - elPesoMinimo;
  lcd.clear();
  delay(20);
  lcd.setCursor(0,0);
  lcd.print("VOL POR VACIAR:");
  lcd.setCursor(0,1);
  lcd.print(pesoInicial);
  lcd.setCursor(6,1);
  lcd.print("ML");
  delay(8000);
  
  bombaVacio.on();
  
  while(PesoActual() > elPesoMinimo)
  {
    deteccionPulso();

    lcd.setCursor(0,0);
    lcd.print("VACIANDO ....");
    pesoActual = PesoActual();
    pesoVaciado = pesoInicial - pesoActual; 

    if (millis() - tiempoInicio > tiempoErrorBomba)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("ERROR BOMBA");
      lcd.setCursor(1,0);
      lcd.print("TIEMPO MAXIMO");
      delay(8000);
      bombaVacio.off();
      return;
    }
  
    updateProgressBar(pesoVaciado, pesoTotalAVaciar, 1); // Actualizar la barra de progreso
    delay(2000);
    deteccionPulso();
    lcd.clear();
    delay(20);
  }  
  bombaVacio.off();
}

void Mezclas::calcularVolumen()
{ 
  // EL 70% de la cantidad de 5 litros por ahora que es 3500 --> 35

  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  volumenAceite= porcentajeAceite * 37.5;
  pesoAceiteDeseado  = volumenAceite * DENSIDAD_ACEITE;
  EEPROM.put(VOL_ACEITE_ADRESS, pesoAceiteDeseado);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% DE ACEITE: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeAceite);
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% OF OIL: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeAceite);
  }
  
  delay(6000);

  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  volumenSouji = porcentajeSouji * 37.5;
  pesoSoujiDeseado = volumenSouji * DENSIDAD_SOUJI;
  EEPROM.put(VOL_SOUJI_ADRESS, pesoSoujiDeseado);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% DE SOUJI: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeSouji);
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% OF SOUJI: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeSouji);
  }
  delay(6000);

  // Calculos de la cantidad de Agua que es lo que queda
  int16_t porcentajeAgua = 0;
  porcentajeAgua  = 100 - (porcentajeAceite + porcentajeSouji);
  volumenAgua = porcentajeAgua * 38;
  pesoAguaDeseado = volumenAgua;
  EEPROM.put(VOL_AGUA_ADRESS, pesoAguaDeseado);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% DE AGUA: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeAgua);
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("% OF WATER: ");
    lcd.setCursor(13,0);
    lcd.print(porcentajeAgua);
  }
  delay(6000);
}

void Mezclas::echarLiquido(int16_t pesoPorechar)
{
  // Encender la bomba correspondiente
  encenderBombaCorrespondiente();

  //tiempoInicioMezcla = millis(); 
  tiempoUltimaVariacion = millis();
  pesoLiquido = PesoActual();
  pesoRelative = 0;

  EEPROM.get(PESO_RELATIVO_ADDRESS, pesoRelative);
  mostrarLiquido();
  
  while(pesoRelative < pesoPorechar)
  { 
    deteccionPulso();
    mostrarLiquido();  
    /*
    if (millis() - tiempoInicioMezcla > tiempoErrorBomba)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("ERROR BOMBA");
      delay(8000);
      apagarBombas();
      return;
    }
    */
    nuevoPesoActual = PesoActual();
    if (abs(nuevoPesoActual - pesoLiquido) > 50) 
    {
      tiempoUltimaVariacion = millis(); // Reiniciar el temporizador si hay un cambio significativo
      pesoRelative += nuevoPesoActual - pesoLiquido;
      pesoLiquido = nuevoPesoActual;
      EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelative);
      //tiempoPasadoMezcla = millis() - tiempoInicioMezcla;
      tiempoPasadoAgotado = millis() -  tiempoUltimaVariacion;
      updateProgressBar(pesoRelative, pesoPorechar, 1);
    }

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
    delay(700);
    deteccionPulso();
  }

  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, 0);
  
  // Apagar la bomba correspondiente
  apagarBombas();
  delay(1000);
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
  numMezclas = 0;
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
      //tiempoInicioMezcla += (millis() - tiempoInicioMezcla) - tiempoPasadoMezcla;
      tiempoUltimaVariacion  += (millis() - tiempoUltimaVariacion) - tiempoPasadoAgotado;
      encenderBombaCorrespondiente();
  }
}

void Mezclas::pausarMezcla()
{
  Pantallamezcla(8);
  apagarBombas();
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
  }
}

void Mezclas::mostrarLiquido()
{
  lcd.clear();
  delay(5);
  switch(estado2)
  {
    case 1:
      idioma == 0 ? lcd.print("ECHANDO ACEITE") : lcd.print("MAKING OIL");
      break;
    case 2:
      idioma == 0 ? lcd.print("ECHANDO SOUJI") : lcd.print("MAKING SOUJI");
      break;
    case 4:
      idioma == 0 ? lcd.print("ECHANDO AGUA") : lcd.print("MAKING WATER");
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
      idioma == 0 ? lcd.print("ACEITE AGOTADO") : lcd.print("OUT OF OIL");
      break;
    case 2:
      idioma == 0 ? lcd.print("SOUJI AGOTADO") : lcd.print("OUT OF SOUJI");
      break;
    case 4:
      idioma == 0 ? lcd.print("AGUA AGOTADO") : lcd.print("OUT OF WATER");
      break;
  }
}

void Mezclas::vacioGeneral()
{
  lcd.setCursor(0,0);
  lcd.print("VACIANDO ....");
  tiempoInicioVacio = millis();
  uint64_t tiempoPasado = 0;
  bombaVacio.on();
  while (tiempoPasado < tiempoVacio)
  {
    lcd.setCursor(0,0);
    lcd.print("VACIANDO ....");
    tiempoPasado = millis() - tiempoInicioVacio;
    if (botonPausa.pulsado())
    {
      bombaVacio.off(); 
      lcd.clear();
      delay(20); 
      lcd.setCursor(0, 0);
      lcd.print("VACIADO");
      lcd.setCursor(0, 1);
      lcd.print("FINALIZADO");
      delay(4000);
      menus.PantallaSeleccionada(4);  
      menus.inSubMenu = false;
      menus.updateMenuDisplay();
      //menus.vacioAutomatico = false;
      break;
    }
    updateProgressBar(tiempoPasado, tiempoVacio, 1);
    delay(1000);
    lcd.clear();
    delay(20);
  }
  bombaVacio.off();
  lcd.clear();
  delay(20); 
  lcd.setCursor(0, 0);
  lcd.print("VACIADO");
  lcd.setCursor(0, 1);
  lcd.print("FINALIZADO");
  delay(4000);  
}

void Mezclas::verificarPeso()
{
  peso=PesoActual();
  if(peso>200)
  {
    estado = 3;
  }
  else
  {
    estado = 1;
  }
}