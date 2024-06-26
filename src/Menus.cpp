#include <Arduino.h>
#include "Menus.h"
#include "Reloj_RTC.h"
#include "Boton.h"
#include "EEPROM.h"
#include "Bascula.h"
#include "Motor.h"

// MENUS GENERALES

Menus::Menus(LiquidCrystal &display) : lcd(display) 
{
  misPantallas = 20;
  lcd_init();
}
 
void Menus::lcd_init()
{  
  lcd.begin(16,2);
  lcd.clear();
  PantallaSeleccionada(0);
}


///////////////// Pantallas del MENU Principal   ////////////////////////
void Menus::PantallaSeleccionada(uint8_t pantalla)
{
  // Limpieza de la pantalla 
  if(pantalla != misPantallas)
  {
    lcd.clear();
    misPantallas = pantalla;
  }

  // Pantalla de Cantidad de Souji
  if (pantalla == 0)
  {
    menuPrincipal = true;
    menuProgramador = false;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CANTIDAD SOUJI");
    lcd.setCursor(0,1);
    lcd.print("LOS LITROS");
    lcd.setCursor(12,1);
    lcd.print(Cantidad_Souji[IndexCantidad]);
  }

  // Pantalla de Litros Mensuales
  if (pantalla == 1)
  {
    mostrarLitrosMensuales = false;
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("MOSTRAR");
    lcd.setCursor(0,1);
    lcd.print("LITROS MENSUALES");
  }

  // Pantalla de Litros Totales
  if (pantalla == 2)
  {
    EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LITROS TOTALES");
    lcd.setCursor(0,1);
    lcd.print("LOS LITROS");
    lcd.setCursor(12,1);
    lcd.print(litrosTotales);
  }

  // El Menu de la FECHA
  if (pantalla == 3)
  { 
    EEPROM.get(DAY_ADDRESS, dia);
    EEPROM.get(MONTH_ADDRESS, mes);
    EEPROM.get(YEAR_ADDRESS, anio);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LA FECHA ES :");
    lcd.setCursor(0,1);
    lcd.print(dia);
    lcd.print("/");
    lcd.print(mes);
    lcd.print("/");
    lcd.print(anio);
  }

  // El Menu de Limpieza Automatica
  if (pantalla == 4)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("LA LIMPIEZA ");
    lcd.setCursor(3,1);
    lcd.print("AUTOMATICA");
  }

  // El Menu de Vaciar Deposito
  if (pantalla == 5)
  {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("VACIAR");
    lcd.setCursor(4,1);
    lcd.print("DEPOSITO");
  }
  
  // El Menu de Calibracion de Bascula
  if (pantalla == 6)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("CALIBRACION");
    lcd.setCursor(4,1);
    lcd.print("BASCULA");
  }
  
  // El Menu de Lenguaje
  if (pantalla == 7)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("SELECCIONAR");
    lcd.setCursor(4,1);
    lcd.print("LENGUAJE");
  }

  // El SubMenu de Cantidad Souji
  if (pantalla == 8)
  {
    inSubMenu = true;
    variarCantidad = true;

    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("AJUSTAR:");
    lcd.setCursor(4,1);
    lcd.print(Cantidad_Souji[IndexCantidad]);
  }

  // El SubMenu de Litros Mensuales
  if (pantalla == 9)
  {
    mostrarLitrosMensuales = true;
    displayLitrosMensuales();
  }

   // El SubMenu de Litros Totales
  if (pantalla == 10)
  {
    EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
    displayLitrosTotales();
  }
  
  // El SubMenu de Fecha
  if (pantalla == 11)
  {
    EEPROM.get(DAY_ADDRESS, dia);
    EEPROM.get(MONTH_ADDRESS, mes);
    EEPROM.get(YEAR_ADDRESS, anio);
    definirFecha = true;
    editIndex = -1 ;
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("AJUSTAR FECHA ");
    lcd.setCursor(3,1);
    lcd.print("PULSE SEL ");
  }
  
  // El SubMenu de Limpieza
  if (pantalla == 12)
  {
    limpiezaAutomatica = true ;
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("PUSLE SEL ");
    lcd.setCursor(0,1);
    lcd.print("PARA LA LIMPIEZA");
  }
  
  // El SubMenu de Vaciar Deposito
  if (pantalla == 13)
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("INICIAR");
    lcd.setCursor(5,1);
    lcd.print("VACIO");
    mostrarPeso = true;
  }
  
  // El SubMenu de Calibracion
  if (pantalla == 14)
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("INICIAR CALIB.");
    lcd.setCursor(3,1);
    lcd.print("PULSA SEL");
    iniciarCalibracion = true;
  }
  
  // El SubMenu de lenguaje
  if (pantalla == 15)
  {
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("ESPANOL");
    lcd.setCursor(4,1);
    lcd.print("INGLES");
  }
}

void Menus::updateMenuDisplay()
{
  PantallaSeleccionada(menuIndex);
}

void Menus::entrarSubMenu()
{
  if(!inSubMenu && menuPrincipal)
  {
  inSubMenu = true;
  lcd.clear();
  switch (menuIndex) 
  {
    case 0:
      Menus::PantallaSeleccionada(8);
      break;
    case 1:
      Menus::PantallaSeleccionada(9);
      break;
    case 2:
      Menus::PantallaSeleccionada(10);
      break;
    case 3:
      Menus::PantallaSeleccionada(11);
      break;
    case 4:
      Menus::PantallaSeleccionada(12);
      break;
    case 5:
      Menus::PantallaSeleccionada(13);
      break;
    case 6:
      Menus::PantallaSeleccionada(14);
      break;
    case 7:
      Menus::PantallaSeleccionada(15);
      break;
  }
  }
  else
  {
    inSubMenu = false;
    variarCantidad = false;
    definirFecha = false;
    updateMenuDisplay();
  }
}


//////////////// PANTALLAS DEL MODO PROGRAMADOR  ///////////////////////
void Menus::PantallaProgramador(uint8_t pantallaProg)
{
  // Limpieza de la pantalla 
  if(pantallaProg != misPantallasProg)
  {
    lcd.clear();
    misPantallasProg = pantallaProg;
  }

  // Pantalla de inicio del modo PROGRAMADOR
  if (pantallaProg == 0)
  {
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("ESTAMOS EN EL");
    lcd.setCursor(0,1);
    lcd.print("MODO PROGRAMADOR");
  }
  
  // Pantalla de Ajustar la cantidad del ACEITE
  if (pantallaProg == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AJUSTAR CANTIDAD");
    lcd.setCursor(2,1);
    lcd.print("DE : ACEITE");
  }

  // Pantalla de Ajustar la cantidad del SOUJI
  if (pantallaProg == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AJUSTAR CANTIDAD");
    lcd.setCursor(2,1);
    lcd.print("DE : SOUJI");
  }

  // Pantalla de RESETEAR LA CANTIDAD DE LITROS ACUMULADA
  if (pantallaProg == 3)
  {
    lcd.clear();
    lcd.setCursor(2,0);
    lcd.print("RESETEAR LOS");
    lcd.setCursor(1,1);
    lcd.print("LITROS TOTALES");
  }

  // Pantalla de Ajustar LA VELOCIDAD DEL MOTOR
  if (pantallaProg == 4)
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("AJUSTAR RPMS");
    lcd.setCursor(5,1);
    lcd.print("DEL MOTOR");
  }

  // SUBMENU de Ajustar la cantidad del ACEITE
  if (pantallaProg == 5)
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("AJUSTANDO");
    lcd.setCursor(5,1);
    lcd.print("ACEITE...");
  }

  // SUBMENU de Ajustar la cantidad del SOUJI
  if (pantallaProg == 6)
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("AJUSTANDO");
    lcd.setCursor(5,1);
    lcd.print("SOUJI...");
  }

  // SUBMENU de RESETEAR LA CANTIDAD DE LITROS ACUMULADA
  if (pantallaProg == 7)
  {
    resetearTodo = true;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PARA BORRAR TODO");
    lcd.setCursor(4,1);
    lcd.print("PULSE SEL");
  }

  // Pantalla de Ajustar LA VELOCIDAD DEL MOTOR
  if (pantallaProg == 8)
  {
    accederRpms = true ;
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("PULSE SEL");
    lcd.setCursor(2,1);
    lcd.print("MODIFICAR");
  }
}

void Menus::modificarProg()
{
  if(menuProgramador)
  {
    PantallaSeleccionada(0);
  }
  else if(!menuProgramador)
  {
    menuPrincipal = false;
    menuProgramador = true;
    PantallaProgramador(0);
  }
  
}

void Menus::entrarSubMenuProg()
{
  if(!SubMenuProgamador)
  {
    SubMenuProgamador = true;
    lcd.clear();
    switch (menuProgIndex) 
    {
      case 0:
        Menus::PantallaProgramador(4);
        break;
      case 1:
        Menus::PantallaProgramador(5);
        break;
      case 2:
        Menus::PantallaProgramador(6);
        break;
      case 3:
        Menus::PantallaProgramador(7);
        break;
    }
  }
  else
  {
    SubMenuProgamador = false;
    updateMenuProgDisplay();
  }
}

void Menus::updateMenuProgDisplay()
{
  PantallaProgramador(menuProgIndex);
}


////////////////  Manejar los botones de entrada  ///////////////////////
void Menus::decrementandoIndex() 
{
    if (!inSubMenu && menuPrincipal && menuIndex < 7) 
    {
      menuIndex++;
      updateMenuDisplay();
    }
    else if(inSubMenu && variarCantidad && IndexCantidad > 0)
    {                      
      IndexCantidad--; 
      updateCantidadSouji();
    }
    else if(definirFecha)
    {
      bajaFecha();
      displayFecha(); 
    }
    else if (mostrarLitrosMensuales)
    {
      mes++;
      if (mes > 12) 
      {
        mes = 1;
      }
      displayLitrosMensuales();
    }
    else if(menuProgramador && !inSubMenuProg && menuProgIndex < 4)
    {
      menuProgIndex++;
      updateMenuProgDisplay(); 
    }
    else{}
}

void Menus::incrementandoIndex() 
{
    if (!inSubMenu && menuPrincipal && menuIndex > 0)
    {
      menuIndex--;
      updateMenuDisplay(); 
    }
    else if(inSubMenu && variarCantidad && IndexCantidad < 4)
    {
      IndexCantidad++;
      updateCantidadSouji();
    }
    else if(definirFecha)
    {
      subeFecha();
      displayFecha();
    }
    else if (mostrarLitrosMensuales)
    {
      mes--;
      if (mes < 1) 
      {
        mes = 12;
      }
      displayLitrosMensuales();
    }
    else if(menuProgramador && !inSubMenuProg && menuProgIndex > 0)
    {
      menuProgIndex--;
      updateMenuProgDisplay(); 
    }
    else{}
}

void Menus::modificarBotonSet()
{
  if (menuPrincipal)
  {
    if(bascularFecha)
    {
      validarFecha();
    }
    else
    {
      entrarSubMenu();
    }
  }
  else if(menuProgramador)
  {
    entrarSubMenuProg();
  }
  else{}
}

void Menus::modificarBotonSel()
{
  if (menuPrincipal)
  {
    if(definirFecha)
    {
      bascularFecha = true;
      ajustarFecha();
    } 
    else if(bascularFecha)
    {
      pasarFecha();
    }
    else if(iniciarCalibracion)
    {
      iniciarCaliBascula();
    }
    else if(calibrarPeso)
    {
      talarBascula();
    }
    else if(calibrarPeso1)
    {
      calibrarEscala();
    }
    else if(mostrarPeso)
    {
      mostrarElPeso();
    }
    else if(limpiezaAutomatica)
    {
      iniciarLimpieza();
    }
    else
    {
      validarMezca();
    }
  }  
  else if(menuProgramador)
  {
    if(resetearTodoVerif)
    {
      ReseteoTotalVerif();
    }
    else if(resetearTodo)
    {
      ReseteoTotal();
    }
    else if(accederRpms)
    {
      mostrarRpms();
    }
    
  } 
}


////////////////   Programas de las mezclas     /////////////////////////
void Menus::updateCantidadSouji()
{
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("AJUSTAR:");
  lcd.setCursor(4,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
}

void Menus::volverMenu()
{
  lcd.clear();
  inSubMenu = false;
  variarCantidad = false;
  PantallaSeleccionada(menuIndex);
}

void Menus::incrementarCantidad(int cantidad) 
{
  EEPROM.get(DAY_ADDRESS, dia);
  EEPROM.get(MONTH_ADDRESS, mes);
  EEPROM.get(YEAR_ADDRESS, anio);

  int mesActual = mes - 1; 
  cantidadMezclaMes[mesActual] += cantidad; 
  litrosMensuales[mesActual] += cantidad; 
  litrosTotales += cantidad;
  EEPROM.put(LITROS_TOTALES_DIRECCION, litrosTotales);
  guardarLitrosMensualesEnEEPROM();
}

void Menus::mezcla_5_Litros()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EJECUTANDO...:");
  lcd.setCursor(2,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
  lcd.setCursor(5,1);
  lcd.print("LITROS");
}
void Menus::mezcla_10_Litros()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EJECUTANDO...:");
  lcd.setCursor(2,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
  lcd.setCursor(5,1);
  lcd.print("LITROS");
}
void Menus::mezcla_15_Litros()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EJECUTANDO...:");
  lcd.setCursor(2,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
  lcd.setCursor(5,1);
  lcd.print("LITROS");
}
void Menus::mezcla_20_Litros()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EJECUTANDO...:");
  lcd.setCursor(2,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
  lcd.setCursor(5,1);
  lcd.print("LITROS");
}
void Menus::mezcla_25_Litros()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EJECUTANDO...:");
  lcd.setCursor(2,1);
  lcd.print(Cantidad_Souji[IndexCantidad]);
  lcd.setCursor(5,1);
  lcd.print("LITROS");
}

void Menus::displayLitrosMensuales()
{
  cargarLitrosMensualesDesdeEEPROM();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EN ");
  lcd.print(elegirMes(mes)); // Muestra el nombre del mes actual
  lcd.print(" HAY");
  lcd.setCursor(3,1);
  lcd.print(litrosMensuales[mes - 1]); // Muestra los litros mensuales del mes actual
  lcd.setCursor(8,1);
  lcd.print("LITROS");
}

void Menus::displayLitrosTotales()
{
  EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("HASTA AHORA HAY");
  lcd.setCursor(2,1);
  lcd.print(litrosTotales);
  lcd.setCursor(8,1);
  lcd.print("LITROS");
}

void Menus::validarMezca()
{
  if(variarCantidad)
  {
    ejecutarMezcla(Cantidad_Souji[IndexCantidad]);
  }
}

String Menus::elegirMes(uint8_t mes)
{
  switch (mes) 
  {
    case 1:
      elMes = "ENERO";
      break;
    case 2:
      elMes = "FEBRERO";
      break;
    case 3:
      elMes = "MARZO";
      break;
    case 4:
      elMes = "ABRIL";
      break;
    case 5:
      elMes = "MAYO";
      break;
    case 6:
      elMes = "JUNIO";
      break;
    case 7:
      elMes = "JULIO";
      break;
    case 8:
      elMes = "AGOSTO";
      break;
    case 9:
      elMes = "SEPTIEMBRE";
      break;
    case 10:
      elMes = "OCTUBRE";
      break;
    case 11:
      elMes = "NOVIEMBRE";
      break;
    case 12:
      elMes = "DICIEMBRE";
      break;
  }
  return elMes;
}

void Menus::ejecutarMezcla(int Cantidad_Souji)
{
  switch (Cantidad_Souji)
  {
  case 5:
    Menus::mezcla_5_Litros();
    incrementarCantidad(5);
  break;
  case 10:
    Menus::mezcla_10_Litros();
    incrementarCantidad(10);

  break;
  case 15:
    Menus::mezcla_15_Litros();
    incrementarCantidad(15);

  break;
  case 20:
    Menus::mezcla_20_Litros();
    incrementarCantidad(20);

  break;
  case 25:
    Menus::mezcla_25_Litros();
    incrementarCantidad(25);
  break;
  }
}

void Menus::resetearLitrosMensuales()
{
  for (int i = 0; i < 12; ++i) {
    litrosMensuales[i] = 0; // Reseteamos los litros mensuales a cero
    int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
    EEPROM.put(direccion, litrosMensuales[i]); // Guardamos los litros mensuales en la EEPROM
  }
}

//////////////////////  CONTROL DE LA FECHA  ///////////////////////////

void Menus::displayFecha()
{  
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LA FECHA ES:");
  lcd.setCursor(3,1);
  lcd.print(dia); 
  lcd.print("/"); 
  lcd.print(mes); 
  lcd.print("/"); 
  lcd.print(anio);
}

void Menus::ajustarFecha()
{
  bascularFecha = true;
  displayFecha(); 
}

void Menus::validarFecha()
{
  EEPROM.put(DAY_ADDRESS, dia);
  EEPROM.put(MONTH_ADDRESS, mes);
  EEPROM.put(YEAR_ADDRESS, anio);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Fecha ajustada a");
  lcd.setCursor(0, 1);
  lcd.print(dia); 
  lcd.print("/"); 
  lcd.print(mes); 
  lcd.print("/"); 
  lcd.print(anio);
  delay(2000);
  bascularFecha = false;
  definirFecha = false;
  inSubMenu = false;
  editIndex = 0;
  PantallaSeleccionada(3);
}

void Menus::pasarFecha()
{
  if(editIndex<2)
  {
    editIndex++;
  }
  else
  {
    editIndex=0;
  } 
}

void Menus::subeFecha()
{
  switch (editIndex)
  {
  case 0 :
    if(dia<31)
    {dia++;}
    else
    {dia = 1;}
    break;
  case 1 :
    if(mes<12)
    {mes++;}
    else
    {mes = 1;}
    break;
  case 2 :
    if(anio<3000)
    {anio++;}
    else
    {anio = 2024;}
    break;
  }
  EEPROM.put(DAY_ADDRESS, dia);
  EEPROM.put(MONTH_ADDRESS, mes);
  EEPROM.put(YEAR_ADDRESS, anio);
  displayFecha();
}

void Menus::bajaFecha()
{
  switch (editIndex)
  {
  case 0 :
    if(dia>1)
    {dia--;}
    else
    {dia = 31;}
    break;
  case 1 :
    if(mes>1)
    {mes--;}
    else
    {mes = 12;}
    break;
  case 2 :
    anio--;
    break;
  }
  EEPROM.put(DAY_ADDRESS, dia);
  EEPROM.put(MONTH_ADDRESS, mes);
  EEPROM.put(YEAR_ADDRESS, anio);
  displayFecha();
}
 

////////////////////  CONTROL DE LA EEPROM   //////////////////////////
void Menus::inicializarEEPROM() {
    int initCheck;
    EEPROM.get(INIT_CHECK_ADDRESS, initCheck);
    if (initCheck != 12345) {
        EEPROM.put(DAY_ADDRESS, 1); // Día inicial
        EEPROM.put(MONTH_ADDRESS, 1); // Mes inicial
        EEPROM.put(YEAR_ADDRESS, 2024); // Año inicial
        EEPROM.put(INIT_CHECK_ADDRESS, 12345); // Marcar como inicializado
    }
}


//////////////////  CONTROL DE LA BASCULA  ////////////////////////////
void Menus::iniciarCaliBascula()
{
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print("PULSE SEL :");
  lcd.setCursor(2,1);
  lcd.print("PARA INICIAR");
  iniciarCalibracion = false;
  calibrarPeso = true;
  calibracion(); 
}

void Menus::talarBascula()
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("PONGA EL PESO :");
  lcd.setCursor(4,1);
  lcd.print("PULSE SEL");
  calibrarPeso = false;
  calibrarPeso1 = true ;
}

void Menus::calibrarEscala()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ESPERE....");
  iniciarCalibracion = false;
  calibrarPeso = false;
  calibrarPeso1 = false ;
  finDeCalibracion();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RETIRE EL PESO");
  lcd.setCursor(2,1);
  lcd.print("YA ESTAAAA");
}
 
void Menus::mostrarElPeso()
{
  PesoActual();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("EL PESO ES :");
  lcd.setCursor(0,1);
  lcd.print(peso);
  lcd.setCursor(8,1);
  lcd.print("GRAMOS");
}


///////////////////  CONTROL DEL MOTOR  //////////////////////////////
void Menus::mostrarRpms()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LA FREQ:");
  //lcd.setCursor(10,0);
  //lcd.print(frecuencia);
  lcd.setCursor(2,1);
  lcd.print("LOS RPMS:");
  //lcd.setCursor(10,1);
  //lcd.print(Motor::calcularRpms());
}



///////////////////  CONTROL DE LA LIMPIEZA AUTOMATICA  //////////////////////////////

void Menus::iniciarLimpieza()
{

}



///////////////////  CONTROL DEL RESETEO TOTAL DE DATA//////////////////////////////


void Menus::ReseteoTotalVerif()
{
  resetearTodo = true;
  resetearTodoVerif = false;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("ESTAS SEGURO ??");
  lcd.setCursor(3,1);
  lcd.print("PULSE SEL");
}


void Menus::ReseteoTotal()
{
  litrosTotales = 0 ;
  EEPROM.put(LITROS_TOTALES_DIRECCION, litrosTotales);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("RESETEANDO...");
  delay(2000);  
  resetearLitrosMensuales();
  PantallaProgramador(0);
}