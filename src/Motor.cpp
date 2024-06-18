#include "Motor.h"

volatile uint64_t Motor::pulsos = 0;
int pin_encoder = 22;
int rpms=2500;

// ERRORES

Motor::Motor(uint8_t pin,uint8_t pin_encoder)
{
    this->pin = pin;
    tiempoInicio = 0;
    frecuencia = 0 ;
    rpms = 2500 ;
    pulsos = 0;
    pulsos_ultimo_segundo = 0 ;
    init();
}

void Motor::init()
{
    pinMode(pin_encoder, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(pin_encoder), contarPulsos, RISING); 
    pararMotor();
}

void Motor::pararMotor()
{
    digitalWrite(pin, false);
}        

void Motor::contarPulsos()
{
    pulsos++;
}
        
void Motor::ajustarRpms(uint64_t tiempoMezcla)
{
    //int valorPwm = map(rpms,0,3300,0,255); 
    
    analogWrite(pin,130);
    tiempoInicio = millis();
    uint64_t tiempoPasado;
    while(millis() - tiempoInicio < tiempoMezcla)
    {
        tiempoPasado = millis() - tiempoInicio;
        updateProgressBar(tiempoPasado, tiempoMezcla, 1);  
        delay(500);
    }
    pararMotor();
}

void Motor::mostrarRpms(int pin_encoder)
{
    
    if (millis() - tiempoInicio >= 1000) 
    {  
        detachInterrupt(digitalPinToInterrupt(pin_encoder)); 
        pulsos_ultimo_segundo = pulsos; 
        pulsos = 0; 
        attachInterrupt(digitalPinToInterrupt(pin_encoder), contarPulsos, RISING); 
        tiempoInicio = millis();

        // Calcula la frecuencia en Hz (pulsos por segundo)
        frecuencia = pulsos_ultimo_segundo / 1.0;
        rpms = abs((int(frecuencia)*60)/600);
        if(idioma==0)
        {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("FREQ:");
            lcd.setCursor(7,0);
            lcd.print(frecuencia);
            lcd.setCursor(0,1);
            lcd.print("RPMS:");
            lcd.setCursor(7,1);
            lcd.print(rpms);
        }
        else
        {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("FREQ:");
            lcd.setCursor(7,0);
            lcd.print(frecuencia);
            lcd.setCursor(0,1);
            lcd.print("RPMS:");
            lcd.setCursor(7,1);
            lcd.print(rpms);  
        }
    }  
}

void Motor::modificarRpms()
{   
    cargarRpms();
    if(idioma==0)
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("AJUSTAR RPMS:");
        lcd.setCursor(7,1);
        lcd.print(rpms);
    }
    else
    {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("SETTING RPMS:");
        lcd.setCursor(7,1);
        lcd.print(rpms);
    }
}

void Motor::bajarRpms()
{
    if(rpms == 0)
    {
        if(idioma==0)
        {
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("VALOR DE RPMS");
            lcd.setCursor(0,1);
            lcd.print("MINIMA:");
            lcd.setCursor(8,1);
            lcd.print(rpms);
        }
        else
        {
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("IS THE MINIMUM");
            lcd.setCursor(0,1);
            lcd.print("RPMS:");
            lcd.setCursor(8,1);
            lcd.print(rpms);
        }
        delay(2000);
        modificarRpms(); 
    }
    else
    {
        rpms-=50;
        EEPROM.put(RPMS_ADRESS, rpms); 
        modificarRpms();
    }
}

void Motor::subirRpms()
{
    if(rpms == RPMS_MAX)
    {
        if(idioma==0)
        {
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("VALOR DE RPMS");
            lcd.setCursor(0,1);
            lcd.print("MAXIMA:");
            lcd.setCursor(8,1);
            lcd.print(rpms);
        }
        else
        {
            lcd.clear();
            lcd.setCursor(1,0);
            lcd.print("IS THE MAXIMUM");
            lcd.setCursor(0,1);
            lcd.print("RPMS:");
            lcd.setCursor(8,1);
            lcd.print(rpms);
        }
        delay(2000);
        modificarRpms(); 
    }
    else
    {
        rpms+=50;
        EEPROM.put(RPMS_ADRESS, rpms);
        modificarRpms();
    }
}

int Motor::rpmS()
{
    return rpms;
}

void Motor::cargarRpms()
{
    EEPROM.get(RPMS_ADRESS, rpms);
}