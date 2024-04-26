#include "Arduino.h"
#include "Bascula.h"
#include "EEPROM.h"

float peso = 0 ;
float pesoTara = 0;
long escala = 0;
long adc_lecture=0;
bool iniciarCalibracion = false;
bool calibrarPeso = false;
bool calibrarPeso1 = false ;
bool conf = false ;
bool mostrarPeso = false;

HX711 balanza;

void balanza_Setup()
{
    balanza.begin(DOUT,CLK);
    cargarEscala();
    balanza.set_scale(escala);
    balanza.tare(VALUE_TARA);
    iniciarCalibracion = false;
    calibrarPeso = false;
    calibrarPeso1 = false ;
    conf = false ;
    mostrarPeso = false;
}

float PesoActual()
{
    peso = balanza.get_units(10);
    if(peso < 1.0)
    {
        return 0.0;                    
    }
    else 
    {
        return peso;
    }
}

void calibracion1()
{
    balanza.read();
    balanza.set_scale(); //La escala por defecto es 1
    balanza.tare(VALUE_TARA);  //El peso actual es considerado zero.
}
    
   
void finDeCalibracion()
{
    //Lee el valor del HX711
    adc_lecture = balanza.get_value(100);
    //Calcula la escala con el valor leido dividiendo el peso conocido
    escala = adc_lecture / PesoConocido;
    escribirEscala();
    delay(2000);
    balanza.set_scale(escala); // Establecemos la escala
    balanza.tare(VALUE_TARA);  //El peso actual de la base es considerado zero.
    iniciarCalibracion = false;
    calibrarPeso = false;
    calibrarPeso1 = false ;
    conf = false; 
}


void cargarEscala()
{
    escala = EEPROM.get(SCALE_ADDRESS, escala);
}

void escribirEscala()
{
    EEPROM.put(SCALE_ADDRESS, escala);
}

