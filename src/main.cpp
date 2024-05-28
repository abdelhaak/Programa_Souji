#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
#include "Reloj_RTC.h"
#include "Menus.h"
#include "Boton.h"
#include "EEPROM.h"
#include "Bascula.h"
#include "Motor.h"

Boton botonSet(PIN_BOTON_SET);
Boton botonUp(PIN_BOTON_UP);
Boton botonDown(PIN_BOTON_DOWN);
Boton botonSel(PIN_BOTON_SEL);
Boton botonPro(PIN_BOTON_PRO);

Menus menu(lcd);

void setup()
{
  Serial.begin(9600);
  Serial.println("Programa iniciado");
  menu.lcd_init();
  rtc.begin();
  litrosTotales = 0;
  balanza_Setup();  
  menu.PantallaSeleccionada(0);
}

void loop()
{
  if (botonUp.pulsado()) 
  {
    menu.incrementandoIndex();
  }
  if (botonDown.pulsado()) 
  {
    menu.decrementandoIndex();
  }
  if (botonSet.pulsado()) 
  {
    menu.modificarBotonSet();
  }
  if (botonSel.pulsado()) 
  { 
    menu.modificarBotonSel();
  }
  if(botonPro.pulsado())
  {
    menu.entrarMenuProg();
  }
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
  }
}
