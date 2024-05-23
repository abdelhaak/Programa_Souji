#ifndef MENUS_H
#define MENUS_H

#include <Wire.h> 
#include <LiquidCrystal.h>

#define DAY_ADDRESS 0   // Dirección para el día
#define MONTH_ADDRESS 4 // Dirección para el mes
#define YEAR_ADDRESS 8  // Dirección para el año
//#define INIT_CHECK_ADDRESS   // Dirección para verificar la inicialización
#define LITROS_TOTALES_DIRECCION 12  // Direccion de los litros totales

class Menus
{
    private:
        LiquidCrystal &lcd;
        bool mezclar5Litros = false;
        // Variables del menu principal 
        uint8_t misPantallas=0;
        int menuIndex = 0;   
        bool inSubMenu = false;
        bool menuPrincipal = false;
        // Variables de la cantidad SOUJI
        int IndexCantidad = 0;
        bool variarCantidad = false;        
        int Cantidad_Souji[5] = {5, 10, 15, 20, 25};
        bool mostrarLitrosMensuales = false;
        // Variables de la gestion de fecha
        bool definirFecha = false;
        bool bascularFecha = false;
        int editIndex = 0;
        // Variables de la gestion de la galga
        bool iniciarCalibracion = false;
        bool calibrarPeso = false;
        bool calibrarPeso1 = false ;
        bool mostrarPeso = false;
        float elPeso = 0.0;
        // Variables del vacio automatico
        bool vacioAutomatico = false;
        bool iniciarVacio = false;

        // Variables del menu programador
        uint8_t misPantallasProg=0;
        bool menuProgramador = false;
        bool SubMenuProgamador = false;
        int menuProgIndex = 0;
        bool inSubMenuProg = false;
        bool accederRpms  = false;
        bool resetearTodoVerif = false;
        bool resetearTodo = false;

        
    public:
        Menus(LiquidCrystal &display);
        void lcd_init();

        // Variables del menu principal
        void PantallaSeleccionada(uint8_t pantalla);
        void updateMenuDisplay();
        void updateCantidadSouji();
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
        void subeFechaRapido();
        void bajaFechaRapido();
        void bajaFecha();
        void modificarBotonSel();
        void modificarBotonSet();
        void inicializarEEPROM();
        void iniciarCaliBascula(); 
        void talarBascula();  
        void calibrarEscala();   
        void mostrarElPeso();   
        void incrementarCantidad(int cantidad);
        void displayLitrosMensuales();
        void displayLitrosTotales();
        String elegirMes(uint8_t mes);
        void funcionVacio();
        void vaciando();

        // Variables del menu programador
        void PantallaProgramador(uint8_t pantallaProg);
        void modificarProg();
        void entrarSubMenuProg();
        void updateMenuProgDisplay();
        void mostrarRpms();
        void ReseteoTotalVerif();
        void ReseteoTotal();
        void resetearLitrosMensuales();


        void decrementandoIndex();
        void decrementandoIndexRapido();
        void incrementandoIndexRapido();
        void incrementandoIndex();
        
};

#endif
