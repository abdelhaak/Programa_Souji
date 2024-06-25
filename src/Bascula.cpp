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
    /*static uint16_t lecturas[LECTURAS_MAXIMAS] = {0};
    static uint8_t iindex = 0;
    static uint8_t lecturasTotales = 0;
    
    uint16_t peso = balanza.get_units(10); // Obtener una sola lectura
    if (peso < 0 || peso > 30000)
    {
        peso = 0;
    }
    else
    {
        // Añadir la nueva lectura al array de lecturas
        lecturas[iindex] = peso;
        iindex = (iindex + 1) % LECTURAS_MAXIMAS;
        if (lecturasTotales < LECTURAS_MAXIMAS)
        {
            lecturasTotales++;
        }
        
        // Calcular la media de las lecturas
        uint32_t sum = 0;
        for (uint8_t i = 0; i < lecturasTotales; i++)
        {
            sum += lecturas[i];
        }
        uint16_t average = sum / lecturasTotales;
        
        // Verificar si la nueva lectura es válida
        if (abs(peso - average) > LIMITE)
        {
            // La lectura es anómala, descartarla
            peso = average;
        }
    }
    delay(10);
    return peso; */ 
    peso = balanza.get_units(10); // Obtener una sola lectura
    if (peso < 0 || peso > 30000)
    {
        peso = 0;
    }
    delay(10);
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

