#ifndef MOTOR_H
#define MOTOR_H

#include "Arduino.h"
#include "LCD_Progress.h"
#include "Menus.h"
#include "Mezclas.h"


#define RPMS_MAX 3100
#define PIN_MOTOR 53
#define RPMS_ADRESS 28

extern int pin_encoder;

class Motor
{
    private:
        uint8_t pin;
        uint8_t pinSensor;
        uint64_t pulsos_ultimo_segundo;
        float frecuencia;
        uint64_t tiempoInicio;
        static volatile uint64_t pulsos;
       
    public:
        //int rpms;
        Motor(uint8_t pin,uint8_t pin_encoder);
        void init();
        void parar();
        static void contarPulsos();
        void ajustarRpms(int rpms,uint64_t tiempoMezcla);
        void mostrarRpms(int pin_encoder);
        void modificarRpms();   
        void bajarRpms();
        void subirRpms();   
        int rpmS();  
        void cargarRpms();
};

#endif
