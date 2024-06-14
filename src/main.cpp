#include <Arduino.h>
#include <avr/power.h>
#include "Reloj_RTC.h"
#include "Menus.h"
#include "Boton.h"
#include "Bascula.h"
#include "Mezclas.h"

Boton botonSet(PIN_BOTON_SET);
Boton botonUp(PIN_BOTON_UP);
Boton botonDown(PIN_BOTON_DOWN);
Boton botonSel(PIN_BOTON_SEL);
Boton botonPro(PIN_BOTON_PRO);

Menus menu;
Mezclas mezclas;

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

  menu.lcd_init();

  /*
  //mezclas.resetearTodo();
  Serial.begin(9600);
  Serial.println("Programa iniciado");
  EEPROM.get(IDIOMA_ADRESS, idioma);
  
  rtc_init();
  litrosTotales = 0;
  balanza_Setup();
  menu.PantallaSeleccionada(0);
  */
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
