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
uint16_t pesoLiquido = 0;
//bool enPausa = false;


// ERRORES

// El tiempo de error autorizado de la bascula 
// iniciado en 3 minutos => 180000 // POR AHORA 15 SEGUNDOS
uint64_t tiempoErrorBascula = 180000;

uint64_t tiempoPasado = 0;

// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 10000;
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 15000;
// El peso minimo del vacio autorizado
uint16_t pesoMinimo = 30;

// Porcentajes de inicio de liquidos
uint16_t porcentajeAceite = 30;
uint16_t porcentajeSouji = 50;

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
  mySerial.println("Estamos en mezclaGeneral");
  mySerial.print("Estado  : ");
  mySerial.println(estado);
  mySerial.print("Estado 2: ");
  mySerial.println(estado2);
  if(!enPausa)
  {
    EEPROM.get(I_MEZCLAS_ADRESS, i_mezclas);
    EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
    mySerial.print("pesoLiquido : ");
    mySerial.println(pesoLiquido);
    mySerial.println("Vamos a leer la EEPROM :");
    /*for (int i = 0; i <= 91; i++) 
    {
    byte value = EEPROM.read(i); // Lee el byte en la posición 'i'
    mySerial.print("Direccion ");
    mySerial.print(i);
    mySerial.print(": ");
    mySerial.println(value, DEC); // Imprime el valor como decimal
    }
    */
    // Iniciamos la mezcla 
    if(estado == 0)
    {
      mySerial.println("Estamos en el estado 0");
      Pantallamezcla(0);
      delay(2000);
      estado = 1;
      EEPROM.put(STATUS_ADRESS, estado);
    }
    // Calculamos el volumen de cada liquido
    if(estado == 1)
    {
      mySerial.println("Estamos en el estado 1");
      estado = 2;
      estado2 = 1;
      EEPROM.put(STATUS_ADRESS, estado);
      EEPROM.put(STATUS_2_ADRESS, estado2);
      calcularVolumen();
      delay(50);
    }
    // Pasamos a la mezcla general
    if(estado == 2)
    {
      mySerial.println("Estamos en el estado 2");
      if(mezclas > 1)
      {
        mySerial.println("Mezclas es superior a 1");
        for(i_mezclas=1; i_mezclas<mezclas; i_mezclas++)
          {
            EEPROM.put(I_MEZCLAS_ADRESS, i_mezclas);
            // Hechamos la cantidad adecuada para el Aceite
            if(estado2 == 1)
            {
              mySerial.println("Estado 2 && Estado2 1");
              mySerial.print("Esta es la mezcla NUMERO : ");
              mySerial.println(i_mezclas);
              mySerial.println("Bomba Aceite activada : ");
              Pantallamezcla(1);
              bombaAceite.on();
              hecharLiquido(volumenAceite);
              mySerial.println("Apagando Bomba de Aceite");
              bombaAceite.off();
              estado2 = 2;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }  
            // Hechamos la cantidad adecuada para el SOUJI
            if(estado2 == 2)
            { 
              mySerial.println("Estado 2 && Estado2 2");
              mySerial.println("Bomba Souji activada : ");
              Pantallamezcla(2);
              bombaSouji.on();
              hecharLiquido(volumenSouji);
              mySerial.println("Apagando Bomba de Souji");
              bombaSouji.off();
              estado2 = 3;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la primera etapa de mezcla con el motor 
            if(estado2 == 3)
            { 
              mySerial.println("Estado 2 && Estado2 3");
              mySerial.println("Ahora activamos el motor mezclador");
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
              mySerial.println("Estado 2 && Estado2 4");
              mySerial.println("Bomba Agua activada : ");
              Pantallamezcla(3);
              bombaAgua.on();
              hecharLiquido(volumenAgua);
              mySerial.println("Apagando Bomba de Agua");
              bombaAgua.off();
              estado2 = 5;
              EEPROM.put(STATUS_2_ADRESS, estado2);
            }
            // Pasamos a la segunda etapa de mezcla con el motor 
            if(estado2 == 5)
            {
              mySerial.println("Estado 2 && Estado2 5");
              mySerial.println("Ahora activamos el motor mezclador por la segunda vez");
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
              mySerial.println("Estado 2 && Estado2 6");
              mySerial.println("Empezamos el vacio....");
              Pantallamezcla(6);
              mezclaVacio();
              mySerial.println("Vacio finalizado");
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
              }
              delay(2000);   
            }
          }
        finMezcla = true ;
      }
      else
      {
        mySerial.println("Mezcla es 1");
        // Hechamos la cantidad adecuada para el Aceite
        if(estado2 == 1)
        {
          mySerial.println("Estado 2 && Estado2 1");
          mySerial.println("Esta es una sola mezcla ");
          mySerial.println("Bomba Aceite activada ");
          EEPROM.get(VOL_ACEITE_ADRESS, volumenAceite);
          Pantallamezcla(1);
          bombaAceite.on();
          mySerial.print("volumenAceite : ");
          mySerial.println(volumenAceite);
          mySerial.println("Los valores antes de hechar el aceite");
          hecharLiquido(volumenAceite);
          mySerial.println("Los valores despues de hechar el aceite");
          mySerial.println("Apagando Bomba de Aceite");
          bombaAceite.off();
          estado2 = 2;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }  
        // Hechamos la cantidad adecuada para el SOUJI
        if(estado2 == 2)
        {
          mySerial.println("Estado 2 && Estado2 2");
          mySerial.println("Bomba Souji activada : ");
          EEPROM.get(VOL_SOUJI_ADRESS, volumenSouji);
          //EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
          Pantallamezcla(2);
          bombaSouji.on();
          mySerial.print("volumenSouji : ");
          mySerial.println(volumenSouji);
          mySerial.println("Los valores antes de hechar el Souji");
          hecharLiquido(volumenSouji);
          mySerial.println("Los valores despues de hechar el Souji");
          mySerial.println("Apagando Bomba de Souji");
          bombaSouji.off();
          estado2 = 3;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la primera etapa de mezcla con el motor 
        if(estado2 == 3)
        { 
          mySerial.println("Estado 2 && Estado2 3"); 
          mySerial.println("Ahora activamos el motor mezclador");
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
          mySerial.println("Estado 2 && Estado2 4");
          mySerial.println("Bomba Agua activada : ");
          EEPROM.get(VOL_AGUA_ADRESS, volumenAgua);
          Pantallamezcla(3);
          bombaAgua.on();
          mySerial.print("volumenAgua : ");
          mySerial.println(volumenAgua);
          hecharLiquido(volumenAgua);
          mySerial.println("Apagando Bomba de Agua");
          bombaAgua.off();
          estado2 = 5;
          EEPROM.put(STATUS_2_ADRESS, estado2);
        }
        // Pasamos a la segunda etapa de mezcla con el motor 
        if(estado2 == 5)
        {
          mySerial.println("Estado 2 && Estado2 5");
          mySerial.println("Ahora activamos el motor mezclador por la segunda vez");
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
          mySerial.println("Estado 2 && Estado2 6");
          mySerial.println("Empezamos el vacio....");
          Pantallamezcla(6);
          mezclaVacio();
          mySerial.println("Vacio finalizado");
          if(idioma==0)
          {
            lcd.clear();
            delay(20);
            lcd.setCursor(5,0);
            lcd.print("MEZCLA ");
            lcd.setCursor(2,1);
            lcd.print("FINALIZADA");
            finMezcla = true ;
          }
          else
          {
            lcd.clear();
            delay(20);
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
      mySerial.println("Estado 2 && fin de mezcla");
      resetearTodo();
      mySerial.println("Mezcla terminada.");
      Pantallamezcla(7);
      delay(2000);
      menus.PantallaSeleccionada(0);
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

  if (pantallamezcla == 0)
  {
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
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
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("HECHANDO ACEITE");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("HECHANDO SOUJI");
    }
    else
    {
      lcd.clear();
      delay(20);
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
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("HECHANDO AGUA");
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
      lcd.setCursor(1,0);
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
      lcd.setCursor(1,0);
      lcd.print("ESTAMOS EN");
      lcd.setCursor(5,1);
      lcd.print("PAUSA ...");
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

   // Pantalla de PAUSA
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
}

void Mezclas::mezclaVacio()
{
  mySerial.print("pesoLiquido : ");
  mySerial.println(pesoLiquido);
  mySerial.print("pesoActual : ");
  mySerial.println(PesoActual());
  if (PesoActual() <= pesoMinimo)
  {
    // Error de vacio, es obligatorio hacer un vacio de forma individual 
    mySerial.println("Peso actual ya está por debajo del mínimo, no se necesita vaciar más.");
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("PESO MINIMO");
      lcd.setCursor(0,1);
      lcd.print("ADQUIRIDO");
      delay(2500);
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("WHEIGHT LIMIT");
      lcd.setCursor(3,1);
      lcd.print("ARRIVED");
      delay(2500);
    }
    return;
  }
  bombaVacio.on();
  unsigned long tiempoInicio = millis(); 
  uint16_t pesoInicial = PesoActual();
  uint16_t pesoVaciado;
  uint16_t pesoActual; 
  uint16_t pesoTotalAVaciar = pesoInicial - pesoMinimo;

  lcd.clear();
  delay(20);
  lcd.setCursor(1,0);
  lcd.print("PESO ACTUAL");
  lcd.setCursor(0,1);
  lcd.print(PesoActual());
  lcd.setCursor(9,1);
  lcd.print("GRAMOS");
  delay(1500);
  while(PesoActual() > pesoMinimo)
  {
    pesoActual = PesoActual();
    pesoVaciado = pesoInicial - pesoActual; 
    updateProgressBar(pesoVaciado, pesoTotalAVaciar, 1); // Actualizar la barra de progreso
    delay(500);
    if (millis() - tiempoInicio > tiempoErrorBascula)
    {
      mySerial.println("Error de la bascula : Tiempo máximo de espera alcanzado");
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("ERROR BASCULA");
      delay(2000);
      bombaVacio.off();
      return;
    }
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("VACIANDO ....");
    lcd.setCursor(0,1);
    lcd.print("PESO:");
    lcd.setCursor(6,1);
    lcd.print(PesoActual());
    lcd.setCursor(10,1);
    lcd.print("GRAMOS");
    delay(500);
  }  
  bombaVacio.off();

}

void Mezclas::calcularVolumen()
{
  // Calculos de la cantidad de Aceite
  EEPROM.get(PORCENTAJE_ACEITE_ADRESS, porcentajeAceite);
  mySerial.print("porcentajeAceite : ");
  mySerial.println(porcentajeAceite);
  pesoAceiteDeseado = porcentajeAceite*CAPACIDAD_TOTAL / 100.0;
  volumenAceite = pesoAceiteDeseado / DENSIDAD_ACEITE;
  EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);

  mySerial.print("pesoAceiteDeseado : ");
  mySerial.println(pesoAceiteDeseado);
  mySerial.print("volumenAceite : ");
  mySerial.println(volumenAceite);
  // Calculos de la cantidad de Souji
  EEPROM.get(PORCENTAJE_SOUJI_ADRESS, porcentajeSouji);
  mySerial.print("porcentajeSouji : ");
  mySerial.println(porcentajeSouji);
  pesoSoujiDeseado = porcentajeSouji * CAPACIDAD_TOTAL /100.0;
  volumenSouji = pesoSoujiDeseado / DENSIDAD_SOUJI;
  EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);

  mySerial.print("pesoSoujiDeseado : ");
  mySerial.println(pesoSoujiDeseado);
  mySerial.print("volumenSouji : ");
  mySerial.println(volumenSouji);
  // Calculos de la cantidad de Agua que es lo que queda
  volumenAgua = CAPACIDAD_TOTAL - (pesoAceiteDeseado + pesoSoujiDeseado);
  EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
  mySerial.print("volumenAgua : ");
  mySerial.println(volumenAgua);
}

void Mezclas::hecharLiquido(uint16_t volumen)
{
  pesoInicioEtapa = PesoActual();
  //float pesoRelativo = 0;
  pesoLiquido = PesoActual();
  //EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  mySerial.print("pesoLiquido : ");
  mySerial.println(pesoLiquido);
  uint16_t pesoRelative = 0;
  while(pesoRelative < volumen)
  {
    pesoRelative = PesoActual() - pesoInicioEtapa;
    pesoRelative = PesoActual() - pesoLiquido;
    updateProgressBar(pesoRelative, volumen, 1);  
    delay(100);
  }
  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  mySerial.print("peso Final : ");
  mySerial.println(pesoLiquido);

  /*
  mySerial.println("Estamos en hecharLiquido");
  mySerial.print("enPausa : ");
  mySerial.println(enPausa);
  uint16_t pesoRelativo = 0;

  EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.get(PESO_RELATIVO_ADDRESS, pesoRelativo); 
 
  mySerial.print("pesoLiquido : ");
  mySerial.println(pesoLiquido);
  mySerial.print("pesoActual : ");
  mySerial.println(PesoActual());
  
  uint16_t pesoBase = PesoActual() - pesoRelativo;

  while(pesoRelativo < volumen)
  {
    if(botonPausa.pulsado())
    {
      pausarReanudarMezcla();
    }
    if(enPausa)
    {
      esperarParaReanudar();
    }
    else
    {
      pesoRelativo = PesoActual() - pesoBase;
      EEPROM.put(PESO_RELATIVO_ADDRESS, pesoRelativo);
      updateProgressBar(pesoRelativo, volumen, 1);  
      delay(100);
    }
  }
  mySerial.println("Estamos despues de hecharliquido");
  mySerial.print("enPausa : ");
  mySerial.println(enPausa);
  enPausa = false;
  pesoLiquido = PesoActual();
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  EEPROM.put(PESO_RELATIVO_ADDRESS, 0);
  mySerial.print("peso Final : ");
  mySerial.println(pesoLiquido);
  */
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
  mySerial.println("Reset...");
  estado = 0;
  estado2 = 0;
  numMezclas = 0;
  volumenAceite = 0;
  volumenSouji = 0;
  volumenAgua = 0;
  pesoLiquido = 0;
  EEPROM.put(VOL_AGUA_ADRESS, volumenAgua);
  EEPROM.put(VOL_SOUJI_ADRESS, volumenSouji);
  EEPROM.put(VOL_ACEITE_ADRESS, volumenAceite);
  EEPROM.put(STATUS_ADRESS, estado);
  EEPROM.put(STATUS_2_ADRESS, estado2);
  EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
  EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, pesoLiquido);
  finMezcla = false;
  enPausa = false;
  mySerial.println("Reset finalizado");
}

void Mezclas::pausarReanudarMezcla()
{
  mySerial.println("Estamos en pausarReanudarMezcla");
  mySerial.print("enPausa : ");
  mySerial.println(enPausa);
  enPausa = !enPausa;
  if (enPausa)
  {
    mySerial.println("Pausado");
    Pantallamezcla(8);
  }
  else
  {
    mySerial.println("Reanudado");
    Pantallamezcla(9);
    delay(1000);
  }
}

void Mezclas::esperarParaReanudar()
{
  mySerial.println("Estamos en esperarParaReanudar");
  mySerial.print("enPausa : ");
  mySerial.println(enPausa);
  while (enPausa)
  {
    // Esperar hasta que se reanude la mezcla
    if (botonPausa.pulsado())
    {
      pausarReanudarMezcla();
    }
  }
}
