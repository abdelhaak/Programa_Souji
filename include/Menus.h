#ifndef MENUS_H
#define MENUS_H

#include <Wire.h> 
#include <LiquidCrystal.h>

#define DAY_ADDRESS 0   // Dirección para el día
#define MONTH_ADDRESS 4 // Dirección para el mes
#define YEAR_ADDRESS 8  // Dirección para el año
#define INIT_CHECK_ADDRESS 12 // Dirección para verificar la inicialización

class Menus
{
    private:
        LiquidCrystal &lcd;
        uint8_t misPantallas=0;
        // Cantidad de Souji posible 
        int Cantidad_Souji[5] = {5, 10, 15, 20, 25};
        // Variable para almacenar el índice del valor seleccionado
        uint32_t Litros_Totales ;
        uint32_t Litros_Mensuales;
        // Los menus que tenemos por ahora : 0 | 1 | 2
        int menuIndex = 0;   
        bool inSubMenu = false;
        int IndexCantidad = 0;
        bool variarCantidad = false;
        bool definirFecha = false;
        bool bascularFecha = false;
        int editIndex = 0;
        int dia = 01, mes=01, anio=2024;

        bool iniciarCalibracion = false;
        bool calibrarPeso = false;
        bool calibrarPeso1 = false ;
        bool conf = false ;
        bool mostrarPeso = false;
        
        // Variables de la EEPROM
        const int dayAddress = 0;
        const int monthAddress = 1;
        const int yearAddress = 2;

    public:
        Menus(LiquidCrystal &display);
        void lcd_init();
        void PantallaSeleccionada(uint8_t pantalla);
        void updateMenuDisplay();
        void updateCantidadSouji();
        void decrementandoIndex();
        void incrementandoIndex();
        void entrarSubMenu();
        void volverMenu();
        void validarMezca();
        void ejecutarMezcla(int Cantidad_Souji);
        void mezcla_5_Litros();
        void mezcla_10_Litros();
        void mezcla_15_Litros();
        void mezcla_20_Litros(); 
        void mezcla_25_Litros();
        void displayFecha();
        void ajustarFecha();
        void validarFecha();
        void pasarFecha();
        void subeFecha();
        void bajaFecha();
        void modificarBotonSel();
        void modificarBotonSet();
        void cargarFecha();
        void escribirFecha();
        void inicializarEEPROM();
        void iniciarCaliBascula(); 
        void talarBascula();  
        void calibrarEscala();   
        void mostrarElPeso();   
};

#endif
