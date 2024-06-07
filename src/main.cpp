#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
#include "Reloj_RTC.h"
#include "Menus.h"
#include "Boton.h"
#include "EEPROM.h"
#include "Bascula.h"
#include "Motor.h"
#include "Mezclas.h"

Boton botonSet(PIN_BOTON_SET);
Boton botonUp(PIN_BOTON_UP);
Boton botonDown(PIN_BOTON_DOWN);
Boton botonSel(PIN_BOTON_SEL);
Boton botonPro(PIN_BOTON_PRO);

Menus menu(lcd);
Mezclas mezclas;
void setup()
{
  Serial.begin(9600);
  Serial.println("Programa iniciado");
  EEPROM.get(IDIOMA_ADRESS, idioma);
  menu.lcd_init();
  rtc_init();
  litrosTotales = 0;
  balanza_Setup();  
  /*
  estado =0;
  estado2=0;
  numMezclas=0;
  EEPROM.put(STATUS_ADRESS, estado);
  EEPROM.put(STATUS_2_ADRESS, estado2);
  EEPROM.put(NUM_MEZCLAS_ADRESS, numMezclas);
  */
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
