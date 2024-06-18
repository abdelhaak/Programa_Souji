#include <Arduino.h>
#include <avr/power.h>
#include "Reloj_RTC.h"
#include "Menus.h"
#include "Boton.h"
#include "Bascula.h"
#include "Mezclas.h"
#include <SoftwareSerial.h>

#define rxPin 3
#define txPin 4
SoftwareSerial mySerial(rxPin, txPin);

Boton botonSet(PIN_BOTON_SET);
Boton botonUp(PIN_BOTON_UP);
Boton botonDown(PIN_BOTON_DOWN);
Boton botonSel(PIN_BOTON_SEL);
Boton botonPro(PIN_BOTON_PRO);

const int pindled = A7;
//Menus menu;
Menus menu(lcd,mySerial);
Mezclas mezclas(mySerial);

void setup()
{  
  //Configuro el reloj:
  #if F_CPU == 8000000
    delay(100);
    clock_prescale_set(clock_div_1);
  #endif

  //Desactivo el JTAG.
  MCUCR |= (1 << JTD);
  MCUCR |= (1 << JTD);

  mySerial.begin(4800);
  delay(100);
  mySerial.println("Programa INIICADO");
  menu.lcd_init();
  //mezclas.resetearTodo();
  rtc_init();
  balanza_Setup();
  menu.inicializarEEPROM();
  delay(100);
  menu.PantallaSeleccionada(0);
}

void loop()
{
  if (botonUp.pulsado()) 
  {
    mySerial.println("Boton Up Pulsado");
    menu.incrementandoIndex(); 
    //menu.PantallaSeleccionada(1);
  }
  if (botonDown.pulsado()) 
  {
    mySerial.println("Boton Down Pulsado");
    menu.decrementandoIndex();
    //menu.PantallaSeleccionada(2);
  }
  if (botonSet.pulsado()) 
  {
    mySerial.println("Boton Set Pulsado");
    menu.modificarBotonSet();
  }
  if (botonSel.pulsado()) 
  { 
    menu.modificarBotonSel();
  }
  if(botonPro.pulsado())
  {
    menu.entrarMenuProg();
  }/*
  if (botonUp.pulsadoLargo()) 
  {
    menu.incrementandoIndexRapido();
  }
  if (botonDown.pulsadoLargo()) 
  {
    menu.decrementandoIndexRapido();
  }
  if(botonPro.pulsadoLargo())
  {
    menu.salirMenuProg();
  }*/
}
