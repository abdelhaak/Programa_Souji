// include the library code:
#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
#include "Reloj_RTC.h"
#include "Menus.h"
#include "Boton.h"
#include "EEPROM.h"
#include "Bascula.h"
#include "Motor.h"

#define VALUE_TARA 20
#define DOUT  A1
#define CLK  A0


LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Definición de Botones
const uint8_t PIN_BOTON_SET = 6;
const uint8_t PIN_BOTON_UP = 7;
const uint8_t PIN_BOTON_DOWN = 8;
const uint8_t PIN_BOTON_SEL = 9;

Boton botonSet(PIN_BOTON_SET);
Boton botonUp(PIN_BOTON_UP);
Boton botonDown(PIN_BOTON_DOWN);
Boton botonSel(PIN_BOTON_SEL);

Menus menu(lcd);

void setup()
{
  Serial.begin(9600);
  Serial.println("Programa iniciado");
  menu.lcd_init();
  rtc.begin();
  litrosTotales = 0;
  balanza_Setup();
}

void loop()
{
    if (botonUp.pulsado()) 
    {
      Serial.println("He pulsado en UP: ");
      menu.incrementandoIndex();
    }
    if (botonDown.pulsado()) 
    {
      Serial.println("He pulsado en DOWN: ");
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
    /*
    if(botonUp.mantenerPulsado())
    {
      Serial.println("He mantenido en UP: ");
      menu.botonUpMantenido();
    }
    */
}
