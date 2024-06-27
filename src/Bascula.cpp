#include "Arduino.h"
#include "Bascula.h"
#include "EEPROM.h"

uint16_t peso = 0 ;
uint16_t pesoTara = 0;
long escala = 0;
long adc_lecture=0;
float ultima_tara = 0.0 ;

#define LECTURAS_MAXIMAS 5
#define LIMITE 1000  // Ajusta este valor según la tolerancia aceptable para tu sistema

// ERRORES 


HX711 balanza;

void balanza_Setup()
{
    balanza.begin(DOUT,CLK);
    EEPROM.get(SCALE_ADDRESS, escala);
    EEPROM.get(TARE_ADRESS, ultima_tara);
    balanza.set_scale(escala);
    balanza.tare(ultima_tara);
}

uint16_t PesoActual()
{
    peso = balanza.get_units(10); // Obtener una sola lectura
    if (peso < 0 || peso > 30000)
    {
        peso = 0;
    }
    delay(20);
    return peso;  
}

void calibracion()
{
    balanza.read();
    //La escala por defecto es 1
    balanza.set_scale(); 
    //El peso actual es considerado zero.
    balanza.tare(20); 
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
}

