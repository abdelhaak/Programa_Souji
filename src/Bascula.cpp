#include "Arduino.h"
#include "Bascula.h"
#include "EEPROM.h"

uint16_t peso = 0 ;
uint16_t pesoTara = 0;
long escala = 0;
long adc_lecture=0;

// ERRORES 


HX711 balanza;

void balanza_Setup()
{
    balanza.begin(DOUT,CLK);
    EEPROM.get(SCALE_ADDRESS, escala);
    balanza.set_scale(escala);
    balanza.tare(VALUE_TARA);
}

uint16_t PesoActual()
{ 
    peso = balanza.get_units(10); 
    if(peso < 0)
    {
        peso = 0;                    
    }
    return peso;
}

void calibracion()
{
    balanza.read();
    //La escala por defecto es 1
    balanza.set_scale(); 
    //El peso actual es considerado zero.
    balanza.tare(VALUE_TARA); 
}
    
void finDeCalibracion()
{
    //Lee el valor del HX711
    adc_lecture = balanza.get_value(100);
    //Calcula la escala con el valor leido dividiendo el peso conocido
    escala = adc_lecture / PesoConocido; 
    EEPROM.put(SCALE_ADDRESS, escala);
    // Escribir en la EEPROM
    delay(1000);
    balanza.set_scale(escala); // Establecemos la escala
    balanza.tare(VALUE_TARA);  //El peso actual de la base es considerado zero.
}

