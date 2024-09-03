#include "Motor.h"

volatile uint64_t Motor::pulsos = 0;
int pin_encoder = 23;
int rpms=1500;
//int pin_sensor =  23;

Mezclas mezclado(mySerial);
Menus menuus(lcd,mySerial);
Boton botonPausado(PIN_BOTON_SEL);
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
    analogWrite(pin, 0);
}        

void Motor::contarPulsos()
{
    pulsos++;
}
        
void Motor::ajustarRpms(uint64_t tiempoMezcla)
{
    //int valorPwm = map(rpms,0,3300,0,255); 
    
    analogWrite(pin,130);
    
    //Timer1.setPwmDuty(pin, 512);
    tiempoInicio = millis();
    uint64_t tiempoPasado;
    while(millis() - tiempoInicio < tiempoMezcla)
    {
        if(botonPausado.pulsado())
        {
            unsigned long tiempoPulsado = millis();

            while(botonPausado.pulsado())
            {
                if (millis() - tiempoPulsado > 3000) 
                {
                    lcd.clear();
                    delay(20);
                    lcd.setCursor(0,0);
                    lcd.print("CANCELADO...");
                    delay(4000);
                    cancelar();
                    return;
                }
                delay(20);
            }

            pausado();

            // Espera a que se salga del estado de pausa
            while (enPausa) 
            {
                if (botonPausado.pulsado()) 
                {
                    delay(20); // Pequeño delay para evitar rebotes
                    enPausa = false; // Reanuda el proceso
                }
                delay(100); // Pequeño delay para evitar un loop rápido y sin sentido
            }
            // Actualiza el tiempo de inicio para compensar el tiempo pausado
            tiempoInicio += (millis() - tiempoInicio) - tiempoPasado;
            mezclado.Pantallamezcla(4);
            analogWrite(pin,130);
        }
        tiempoPasado = millis() - tiempoInicio;
        updateProgressBar(tiempoPasado, tiempoMezcla, 1);  
        delay(200);
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
            delay(20);
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
            delay(20);
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
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("AJUSTANDO RPMS:");
        lcd.setCursor(7,1);
        lcd.print(rpms);
    }
    else
    {
        lcd.clear();
        delay(20);
        lcd.setCursor(0,0);
        lcd.print("ADJUSTING RPMS:");
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
            delay(20);
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
            delay(20);
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
            delay(20);
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
            delay(20);
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

void Motor::cargarRpms()
{
    EEPROM.get(RPMS_ADRESS, rpms);
}

int Motor::rpmS()
{
    return rpms;
}

void Motor::pausado()
{
    if(idioma==0)
    {
        lcd.clear();
        delay(20);
        lcd.setCursor(5,0);
        lcd.print("MEZCLA");
        lcd.setCursor(4,1);
        lcd.print("PAUSADA");
    }
    else
    {
        lcd.clear();
        delay(20);
        lcd.setCursor(3,0);
        lcd.print("MIXING IS");
        lcd.setCursor(5,1);
        lcd.print("PAUSED");
    }
    pararMotor();
    enPausa = true;
}

void Motor::cancelar()
{
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("MEZCLA");
      lcd.setCursor(5,1);
      lcd.print("CANCELADA");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("MIXING IS");
      lcd.setCursor(5,1);
      lcd.print("CANCELLED");
    }
    pararMotor();
    mezclado.resetearTodo();
    menuus.PantallaSeleccionada(0);
}