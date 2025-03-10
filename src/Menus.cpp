#include "Menus.h"


// ERRORES

// MENUS GENERALES

int idioma = 0 ;
int tipoMezcla = 0;
int opcionLenguaje = 0; 
int opcionCalibre = 0;
int opcionTipo = 0;
const int PAUSE = 99;
bool enPausa = false;
bool pausado = false;

//const uint8_t RS = A3, EN = A2, D4 = A0, D5 = 0, D6 = 1, D7 = 2;
const uint8_t RS = 8, EN = 9, D4 = 0, D5 = 1, D6 = 2, D7 = 3;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7); 

Mezclas mezcla(mySerial);
Motor motor(PIN_MOTOR,pin_encoder);


//Menus::Menus()    
Menus::Menus(LiquidCrystal &display, Stream &serial) : lcd(display), serial(serial)
{
  misPantallas = 100; 
  lcd_init();
}
 
void Menus::lcd_init()
{  
  lcd.begin(16,2);
  motor.pararMotor();
}

///////////////// Pantallas del MENU Principal   /////////////////
void Menus::PantallaSeleccionada(uint8_t pantalla)
{
  // Limpieza de la pantalla 
  if(pantalla != misPantallas)
  {
    lcd.clear();
    delay(20);
    misPantallas = pantalla;
  }
  
  // Pantalla de Cantidad de Souji
  if (pantalla == 0)
  {  
    EEPROM.get(PESO_RELATIVO_ADDRESS, mezcla.pesoRelative);
    EEPROM.get(SCALE_ADDRESS, escala);
    EEPROM.get(STATUS_ADRESS, mezcla.estado);
    EEPROM.get(STATUS_2_ADRESS, mezcla.estado2);
    EEPROM.get(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, mezcla.pesoLiquido);
    menuPrincipal = true;
    menuProgramador = false;
    mostrarLitrosMensuales = false;
    mostrarLitros = false;
    inSubMenu = false;
    mezcla.mezclando = false;
    cambiarTipo = false;
    menuIndex = 0;
    tipoMezcla = 0;
    opcionTipo = 0;
    if (mezcla.estado == 0)
    {   
      if(idioma == 0)
      {
        lcd.setCursor(2,0);
        lcd.print("SELECCIONAR");  
        lcd.setCursor(1,1);
        lcd.print("TIPO DE MEZCLA");
      }
      else
      {
        lcd.setCursor(3,0);
        lcd.print("CHOOSE THE");  
        lcd.setCursor(4,1);
        lcd.print("MIX TYPE");
      }
    }
    /*else
    {
      mezcla.mezclaGeneral(mezcla.numMezclas);
    }*/
  }
  
  // Pantalla de Litros Mensuales
  if (pantalla == 1)
  {
    mezcla.estado = 0;
    EEPROM.put(STATUS_ADRESS, mezcla.estado);
    mostrarLitros = true;
    mostrarLitrosMensuales = false;
    if(mes == 0)mes =1;
    if(idioma == 0)
    {     
      lcd.setCursor(4,0);
      lcd.print("ENSENAR");
      lcd.setCursor(2,1);
      lcd.print("LITROS / MES");
    }
    else
    {
      lcd.setCursor(6,0);
      lcd.print("SHOW");
      lcd.setCursor(1,1);
      lcd.print("LITERS / MONTH");
    }
  }
  
  // Pantalla de Litros Totales
  if (pantalla == 2)
  { 
    EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
    mostrarLitros = false;
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("LITROS TOTALES");
      lcd.setCursor(0,1);
      lcd.print(litrosTotales);
      lcd.setCursor(9,1);
      lcd.print("LITROS");
    }
    else
    {    
      lcd.setCursor(2,0);
      lcd.print("TOTAL LITERS");
      lcd.setCursor(0,1);
      lcd.print(litrosTotales);
      lcd.setCursor(9,1);
      lcd.print("LITERS");
    }
  }
  
  // El Menu de la FECHA
  if (pantalla == 3)
  { 
    vacioAutomatico = false;
    DateTime Tiempo = rtc.now();
    dia = Tiempo.day();
    mes = Tiempo.month();
    anio = Tiempo.year();

    // Guardar la fecha en EEPROM
    EEPROM.put(DAY_ADDRESS, dia);
    EEPROM.put(MONTH_ADDRESS, mes);
    EEPROM.put(YEAR_ADDRESS, anio);

    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("FECHA ACTUAL");
      lcd.setCursor(4,1);
      lcd.print(dia);
      lcd.print("/");
      lcd.print(mes);
      lcd.print("/");
      lcd.print(anio);
    }
    else
    {
      lcd.setCursor(2,0);
      lcd.print("CURRENT DATE");
      lcd.setCursor(4,1);
      lcd.print(dia);
      lcd.print("/");
      lcd.print(mes);
      lcd.print("/");
      lcd.print(anio);
    }   
  }

  // El Menu de Vaciar Deposito
  if (pantalla == 4)
  {
    vacioAutomatico = true;
    //elegirCalibracion = false;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("DISPENSAR");
      lcd.setCursor(4,1);
      lcd.print("PRODUCTO");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("DISPENSE");
      lcd.setCursor(4,1);
      lcd.print("PRODUCT");
    }
  }
  
  // El Menu de Calibracion de Bascula
  if (pantalla == 5)
  {
    vacioAutomatico = false;
    //elegirCalibracion = false;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("CALIBRACION");
      lcd.setCursor(4,1);
      lcd.print("BASCULA");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("WEIGHT  SCALE");
      lcd.setCursor(2,1);
      lcd.print("CALIBRATION");
    }
  }
  
  // El Menu de Lenguaje
  if (pantalla == 6)
  {
    cambiarIdioma = false;
    //elegirCalibracion = false;
    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("SELECCIONAR");
      lcd.setCursor(4,1);
      lcd.print("LENGUAJE");
    }
    else
    {
      lcd.setCursor(5,0);
      lcd.print("SELECT");
      lcd.setCursor(4,1);
      lcd.print("LANGUAGE");
    }
  }

  // El SubMenu de Cantidad Souji
  if (pantalla == 7)
  {
    inSubMenu = true;
    variarCantidad = true;
    if(idioma==0)
    {      
      lcd.clear();
      delay(20);
      lcd.setCursor(3,0);
      lcd.print("SELECCIONE");
      lcd.setCursor(0,1);
      lcd.print("CANTIDAD:");
      lcd.setCursor(11,1);
      lcd.print(Cantidad_Souji[IndexCantidad]);  
      lcd.setCursor(15,1);
      lcd.print("L"); 
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("CHOOSE QUANTITY:");
      lcd.setCursor(3,1);
      lcd.print(Cantidad_Souji[IndexCantidad]);  
      lcd.setCursor(7,1);
      lcd.print("LITERS");
    }
  }

  // El SubMenu de Litros Mensuales
  if (pantalla == 8)
  {
    mostrarLitrosMensuales = true;
    for (int i = 0; i < 13; ++i) 
    {
      int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
      EEPROM.get(direccion, litrosMensuales[i]);
    }
    displayLitrosMensuales();
  }

  // El SubMenu de Litros Totales
  if (pantalla == 9)
  {
    EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
    displayLitrosTotales();
  }

  // El SubMenu de Fecha
  if (pantalla == 10)
  {    
    definirFecha = true;
    editIndex = 0 ;
    ajustarFecha();
  }
  
  // El SubMenu de Vaciar Deposito
  if (pantalla == 11)
  {
    //vacioAutomatico = true;
    vaciandoDeposito();
    vacioAutomatico = false;
    PantallaSeleccionada(0);
  }
  /*
  // El SubMenu de Calibracion
  if (pantalla == 12)
  {
    elegirCalibracion = true;
    iniciarCalibracion = false;
    calibrarPeso = false;
    calibrarPeso1 = false;
    finDeCalibre = false;
    lcd.clear();
    delay(20);
    if (opcionCalibre == 0) 
    {
      lcd.setCursor(2, 0);
      lcd.print(">");
    } 
    else 
    {
      lcd.setCursor(2, 1);
      lcd.print(">");
    }
    if(idioma==0)
    {
      lcd.setCursor(4, 0);
      lcd.print("CALIBRAR");
      lcd.setCursor(4, 1);
      lcd.print("PESAR");
    }
    else
    {
      lcd.setCursor(4, 0);
      lcd.print("CALIBRATE");
      lcd.setCursor(4, 1);
      lcd.print("WEIGH"); 
    }
    
  }
  */
  // El SubMenu de lenguaje
  if (pantalla == 13)
  {
    cambiarIdioma = true;
    lcd.clear();
    delay(20);
    if (opcionLenguaje == 0) 
    {
      lcd.setCursor(2, 0);
      lcd.print(">");
    } 
    else 
    {
      lcd.setCursor(2, 1);
      lcd.print(">");
    }
    if(idioma == 0)
    {
      lcd.setCursor(4, 0);
      lcd.print("ESPANOL");
      lcd.setCursor(4, 1);
      lcd.print("INGLES");
    }
    else
    {
      lcd.setCursor(4, 0);
      lcd.print("SPANISH");
      lcd.setCursor(4, 1);
      lcd.print("ENGLISH");
    }
  }

  // El SubMenu de seleccionar el tipo de producto
  if (pantalla == 14)
  {
    cambiarTipo = true;
    lcd.clear();
    delay(20);
    if (opcionTipo == 0) 
    {
      lcd.setCursor(0, 0);
      lcd.print(">");
    } 
    else 
    {
      lcd.setCursor(0, 1);
      lcd.print(">");
    }
    if(idioma == 0)
    {
      lcd.setCursor(1, 0);
      lcd.print("LIMP. MULTISUP.");
      lcd.setCursor(1, 1);
      lcd.print("LAVAVAJ. MANUAL");
    }
    else
    {
      lcd.setCursor(1, 0);
      lcd.print("MULTI PURPOSE");
      lcd.setCursor(1, 1);
      lcd.print("FLOOR WASHING");
    }
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
    delay(20);
    switch (menuIndex) 
    {
      case 0:
      Menus::PantallaSeleccionada(14);
      break;
      case 1:
      inSubMenu = false;
      Menus::PantallaSeleccionada(1);
      break;
      case 2:
      inSubMenu = false;
      Menus::PantallaSeleccionada(2);
      break;
      case 3:
      Menus::PantallaSeleccionada(10);
      break;
      case 4:
      inSubMenu = false;
      Menus::PantallaSeleccionada(4);
      break;
      case 5:
      Menus::iniciarCaliBascula();
      break;
      case 6:
      Menus::PantallaSeleccionada(13);
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

//////////////// PANTALLAS DEL MODO PROGRAMADOR  /////////////////
void Menus::PantallaProgramador(uint8_t pantallaProg)
{
  // Limpieza de la pantalla 
  if(pantallaProg != misPantallasProg)
  {
    lcd.clear();
    delay(20);
    misPantallasProg = pantallaProg;
  }

  // Pantalla de inicio del modo PROGRAMADOR
  if (pantallaProg == 0)
  {
    modoProg=true;
    menuProgramador = true;
    menuPrincipal = false;
    ajustarAceite = false;
    ajustarSouji = false;
    accederRpms = false;
    resetearTodo = false;
    lcd.clear();
    delay(20);
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("MODO PROGRAMADOR");  
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("PROGRAMMING MODE");
    }
  }
  
  /*
  // Pantalla de Ajustar la cantidad del ACEITE
  if (pantallaProg == 1)
  {
    validarAjusteAceite = false;
    modoProg=false;
    ajustandoAceite = false;
    ajustarSouji = false;
    ajustarAceite = true;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("AJUSTAR CANTIDAD");
      lcd.setCursor(2,1);
      lcd.print("DE : ACEITE");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("SETTING  THE");
      lcd.setCursor(2,1);
      lcd.print("OIL QUANTITY");
    }
  }

  // Pantalla de Ajustar la cantidad del SOUJI
  if (pantallaProg == 2)
  {
    validarAjusteSouji = false;
    modoProg=false;
    ajustandoSouji=false;
    ajustarAceite = false;
    ajustarSouji = true;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print("AJUSTAR CANTIDAD");
      lcd.setCursor(3,1);
      lcd.print("DE : SOUJI");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("SETTING  THE");
      lcd.setCursor(1,1);
      lcd.print("SOUJI QUANTITY");
    }
  }

  // Pantalla de Ajustar LA VELOCIDAD DEL MOTOR
  if (pantallaProg == 4)
  {
    modoProg=false;
    validarRpms = false;
    accederRpms = true;
    ajustarSouji = false;
    if(idioma==0)
    {
      lcd.setCursor(4,0);
      lcd.print("AJUSTAR");
      lcd.setCursor(0,1);
      lcd.print("VELOCIDAD (RPMS)");
    }
    else
    {
      lcd.setCursor(2,0);
      lcd.print("ADJUST SPEED");
      lcd.setCursor(3,1);
      lcd.print("PARAMETERS");
    }
  }  

  */

  // Pantalla de RESETEAR LA CANTIDAD DE LITROS ACUMULADA
  if (pantallaProg == 1)
  {
    modoProg=false;
    ajustarSouji = false;
    resetearTodo = false;
    accederRpms = false;
    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("RESETEAR LOS");
      lcd.setCursor(1,1);
      lcd.print("LITROS TOTALES");
    }
    else
    {
      lcd.setCursor(4,0);
      lcd.print("RESET THE");
      lcd.setCursor(2,1);
      lcd.print("TOTAL  LITERS");
    }
  }


  
  // SUBMENU de RESETEAR LA CANTIDAD DE LITROS ACUMULADA
  if (pantallaProg == 7)
  {
    resetearTodo = true;
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("PARA BORRAR TODO");
      lcd.setCursor(2,1);
      lcd.print("PULSE SELECT");
    }
    else
    {
      lcd.setCursor(3,0);
      lcd.print("TO  DELETE");
      lcd.setCursor(2,1);
      lcd.print("PRESS SELECT");
    }
  }
}

void Menus::entrarMenuProg()
{
  menuProgramador = true;
  PantallaProgramador(0);
}

void Menus::salirMenuProg()
{
  menuPrincipal = true;
  menuProgramador = false;
  accederRpms = false;
  ajustarSouji = false;
  ajustarAceite = false;
  lcd.clear();
  delay(20);
  PantallaSeleccionada(0);
}

void Menus::entrarSubMenuProg()
{
  lcd.clear();
  delay(20);
  if(!modoProg)
  {
    if(!SubMenuProgamador)
    {
      SubMenuProgamador = true;
      lcd.clear();
      delay(20);
      switch (menuProgIndex) 
      {
        case 0:
          PantallaProgramador(0);
          break;
        case 1:
          PantallaProgramador(7);
          break;
        /*case 2:
          PantallaProgramador(2);
          break;
        case 3:
          PantallaProgramador(7);
          break;
        case 4 :
          PantallaProgramador(4);   
          break;
      */
      }
    }
    else
    {
      menuPrincipal = false;
      ajustarAceite = false;
      ajustarSouji = false;
      accederRpms = false;
      resetearTodo = false;
      SubMenuProgamador = false;
      updateMenuProgDisplay();
    }
  }
}

void Menus::updateMenuProgDisplay()
{
  lcd.clear();
  delay(20);
  PantallaProgramador(menuProgIndex);
}

////////////////  Manejar los botones de entrada  /////////////////

void Menus::modificarBotonSet()
{
  if(menuPrincipal)
  {
    if(bascularFecha)
    {
      validarFecha();
    }
    else if(finDeCalibre)
    {
      finalizarCalibracion();
      PantallaSeleccionada(0);
    }
    else
    {
      entrarSubMenu();
    }
  }
  else if(menuProgramador)
  {
    if(modoProg)
    {
      PantallaProgramador(0);
    }
    else if(ajustarAceite && !validarAjusteAceite)
    {
      validarAjusteAceite = true;
      ajustandoAceite = true;
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(10);
    }
    else if(ajustarSouji && !validarAjusteSouji)
    {
      validarAjusteSouji = true;
      ajustandoSouji=true;
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(11);
    } 
    else if(accederRpms)
    {
      accederRpms = false;
      validarRpms= true ;
      motor.modificarRpms();
    }
    else
    {
      entrarSubMenuProg();
    }
  }
  else
  {
    PantallaProgramador(0);
  }
}

void Menus::modificarBotonSel()
{
  if (menuPrincipal)
  {
    if(definirFecha && bascularFecha)
    {
      pasarFecha();
    }
    else if(cambiarTipo)
    {
      tipoMezcla = opcionTipo;
      cambiarTipo = false;
      PantallaSeleccionada(7);
    }
    else if(mostrarLitros)
    {
      mostrarLitros = false;
      PantallaSeleccionada(8);
    }
    else if(calibrarPeso)
    {
      talarBascula();
    }
    else if(calibrarPeso1)
    {
      calibrarEscala();
    }
    else if(vacioAutomatico)
    {
      vaciandoDeposito();
      vacioAutomatico = false;
      PantallaSeleccionada(0);
    }
    else if(variarCantidad)
    {
      ejecutarMezcla(Cantidad_Souji[IndexCantidad]);
    } 
    else if(cambiarIdioma)
    {
      idioma = opcionLenguaje;
      cambiarIdioma = false;
      EEPROM.put(IDIOMA_ADRESS,idioma);
      PantallaSeleccionada(0);
    }
    /*else if(elegirCalibracion)
    {
      elegirCalibracion = false;
      iniciarCaliBascula();
    }*/
    else if(mezcla.enPausa)
    {
      mezcla.enPausa = !mezcla.enPausa;
      if(mezcla.enPausa)
      {
        mezcla.Pantallamezcla(8);
      }
      else
      {
        EEPROM.get(STATUS_ADRESS,mezcla.estado);
        EEPROM.get(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
        if(opcionTipo == 0)
        {
          mezcla.mezclaMultiusos(mezcla.numMezclas);
        }
        else
        {
          mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
        }      
      }
    }
    else{}
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
      accederRpms = false;
      validarRpms= true ;
      motor.modificarRpms();
    }
    else if(validarRpms)
    {
      lcd.clear();
      delay(20);
      PantallaProgramador(4);
    }
    else if (ajustarAceite && !validarAjusteAceite)
    {
      validarAjusteAceite = true;
      ajustandoAceite = true;
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(10);
    } 
    else if (ajustarSouji && !validarAjusteSouji)
    {
      validarAjusteSouji = true;
      ajustandoSouji = true;
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(11);
    } 
    else if(validarAjusteAceite)
    {
      validarAjusteAceite = false;
      ajustandoAceite = false;
      PantallaProgramador(1);
    }
    else if(validarAjusteSouji)
    {
      validarAjusteSouji = false;
      ajustandoSouji = false;
      PantallaProgramador(2);
    }
    else {}  
  } 
}

void Menus::modificarBotonPro()
{
  if(!menuProgramador)
  {
    entrarMenuProg();
  }
  else
  {
    salirMenuProg();
  }
}

void Menus::decrementandoIndex() 
{
  if(menuPrincipal)
  {
    if(!inSubMenu && menuIndex < 6 && !mostrarLitrosMensuales) 
    {
      menuIndex++;
      updateMenuDisplay();
    }
    else if(cambiarTipo)
    {
      opcionTipo = 1;
      PantallaSeleccionada(14);
    }
    else if(inSubMenu && variarCantidad && IndexCantidad > 0 && !mostrarLitrosMensuales)
    {                      
      IndexCantidad--; 
      updateCantidadSouji();
    }
    else if(definirFecha)
    {
      bajaFecha();
      displayFecha(); 
    }
    else if(mostrarLitrosMensuales)
    {
      mes++;
      if (mes > 12) 
      {
        mes = 1;
      }
      displayLitrosMensuales();
    }
    else if(cambiarIdioma)
    {
      opcionLenguaje = 1;
      PantallaSeleccionada(13);
    }
    /*else if(elegirCalibracion)
    {
      opcionCalibre = 1;
      PantallaSeleccionada(12);
    }*/
    else
    {}
  }
  else if(menuProgramador)
  {
    if(!inSubMenuProg && menuProgIndex < 1 && !ajustandoAceite && !ajustandoSouji && !validarRpms)
    {
      menuProgIndex++;
      updateMenuProgDisplay(); 
    }
    /*else if(ajustandoAceite && !ajustandoSouji)
    {
      mezcla.bajarPorcentajeAceite();   
    }
    else if(ajustandoSouji && !ajustandoAceite)
    {
      mezcla.bajarPorcentajeSouji();    
    }
    else if(validarRpms )
    {
      motor.bajarRpms();
    }*/
  } 
}


void Menus::incrementandoIndex() 
{
  if(menuPrincipal)
  {
    if(!inSubMenu && menuIndex > 0 && !mostrarLitrosMensuales)
    {
      menuIndex--;
      updateMenuDisplay(); 
    }
    else if(cambiarTipo)
    {
      opcionTipo = 0;
      PantallaSeleccionada(14);
    }
    else if(inSubMenu && variarCantidad && IndexCantidad < 4 && !mostrarLitrosMensuales)
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
    else if(cambiarIdioma)
    {
      opcionLenguaje = 0 ;
      PantallaSeleccionada(13);
    }
    /*else if(elegirCalibracion)
    {
      opcionCalibre = 0;
      PantallaSeleccionada(12);
    }*/
    else
    {
    }
  }
  else if(menuProgramador)
  {
    if(!inSubMenuProg && menuProgIndex > 0 && !ajustandoAceite && !ajustandoSouji && !validarRpms)
    {
      menuProgIndex--;
      updateMenuProgDisplay(); 
    }
    /*else if(ajustandoAceite && !ajustandoSouji)
    {
      mezcla.subirPorcentajeAceite();   
    }
    else if(ajustandoSouji && !ajustandoAceite)
    {
      mezcla.subirPorcentajeSouji();    
    }
    else if(validarRpms)
    {
      motor.subirRpms();
    }*/
    else
    {}
  }
}

////////////////   Programas de las mezclas     /////////////////
void Menus::updateCantidadSouji()
{
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(3,0);
    lcd.print("SELECCIONE");
    lcd.setCursor(0,1);
    lcd.print("CANTIDAD:");
    lcd.setCursor(11,1);
    lcd.print(Cantidad_Souji[IndexCantidad]);  
    lcd.setCursor(15,1);
    lcd.print("L"); 
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("CHOOSE QUANTITY:");
    lcd.setCursor(3,1);
    lcd.print(Cantidad_Souji[IndexCantidad]);
    lcd.setCursor(7,1);
    lcd.print("LITERS");  
  }
}

void Menus::incrementarCantidad(int cantidad) 
{
  EEPROM.get(DAY_ADDRESS, dia);
  EEPROM.get(MONTH_ADDRESS, mes);
  EEPROM.get(YEAR_ADDRESS, anio);

  int mesActual = mes; 
  litrosMensuales[mesActual] += cantidad; 
  litrosTotales += cantidad;
  EEPROM.put(LITROS_TOTALES_DIRECCION, litrosTotales);
  for (int i = 0; i < 13; ++i) 
  {
    int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
    EEPROM.put(direccion, litrosMensuales[i]);
  }
  IndexCantidad = 0;
  lcd.clear();
  delay(20);
  Menus::PantallaSeleccionada(0);
}

void Menus::displayLitrosMensuales()
{
    for (int i = 0; i < 13; ++i) 
    {
      int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
      EEPROM.get(direccion, litrosMensuales[i]);
    }
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print(elegirMes(mes)); // Muestra el nombre del mes actual
      lcd.setCursor(0,1);
      lcd.print(litrosMensuales[mes]); // Muestra los litros mensuales del mes actual
      lcd.setCursor(6,1);
      lcd.print("LITROS");  
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print(elegirMes(mes)); // Muestra el nombre del mes actual
      lcd.setCursor(0,1);
      lcd.print(litrosMensuales[mes]); // Muestra los litros mensuales del mes actual
      lcd.setCursor(6,1);
      lcd.print("LITERS");  
    }    
}

void Menus::displayLitrosTotales()
{
  EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("HASTA AHORA HAY");
    lcd.setCursor(2,1);
    lcd.print(litrosTotales);
    lcd.setCursor(8,1);
    lcd.print("LITROS");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("UNTIL NOW :");
    lcd.setCursor(2,1);
    lcd.print(litrosTotales);
    lcd.setCursor(8,1);
    lcd.print("LITERS");
  }
}

String Menus::elegirMes(uint8_t mes)
{
  if(idioma==0)
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
  else
  {
    switch (mes) 
    {
    case 1:
      elMes = "JANUARY";
      break;
    case 2:
      elMes = "FEBRUARY";
      break;
    case 3:
      elMes = "MARCH";
      break;
    case 4:
      elMes = "APRIL";
      break;
    case 5:
      elMes = "MAY";
      break;
    case 6:
      elMes = "JUNE";
      break;
    case 7:
      elMes = "JULY";
      break;
    case 8:
      elMes = "AUGUST";
      break;
    case 9:
      elMes = "SEPTEMBER";
      break;
    case 10:
      elMes = "OCTOBER";
      break;
    case 11:
      elMes = "NOVEMBER";
      break;
    case 12:
      elMes = "DECEMBER";
      break;
  }
  return elMes;
  }
}

void Menus::ejecutarMezcla(int Cantidad_Souji)
{
  //motor.cargarRpms();
  switch (Cantidad_Souji)
  {
  case 5:
    mezcla.numMezclas = 1;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    if(tipoMezcla == 0)
    {
      mezcla.mezclaMultiusos(mezcla.numMezclas);
      incrementarCantidad(5);
    }
    else
    {
      mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
      incrementarCantidad(5);
    }
  break;
  case 10:
    mezcla.numMezclas = 2;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    if(tipoMezcla == 0)
    {
      mezcla.mezclaMultiusos(mezcla.numMezclas);
      incrementarCantidad(10);
    }
    else
    {
      mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
      incrementarCantidad(10);
    }
  break;
  case 15:
    mezcla.numMezclas = 3;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    if(tipoMezcla == 0)
    {
      mezcla.mezclaMultiusos(mezcla.numMezclas);
      incrementarCantidad(15);
    }
    else
    {
      mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
      incrementarCantidad(15);
    }
  break;
  case 20:
    mezcla.numMezclas = 4;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    if(tipoMezcla == 0)
    {
      mezcla.mezclaMultiusos(mezcla.numMezclas);
      incrementarCantidad(20);
    }
    else
    {
      mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
      incrementarCantidad(20);
    }
  break;
  case 25:
    mezcla.numMezclas = 5;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    if(tipoMezcla == 0)
    {
      mezcla.mezclaMultiusos(mezcla.numMezclas);
      incrementarCantidad(25);
    }
    else
    {
      mezcla.mezclaFRIEGASUELOS(mezcla.numMezclas);
      incrementarCantidad(25);
    }
  break;
  }
}

void Menus::resetearLitrosMensuales()
{
  for (int i = 0; i < 13; ++i) 
  {
    litrosMensuales[i] = 0; 
    int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
    EEPROM.put(direccion, litrosMensuales[i]); // Guardamos los litros mensuales en la EEPROM
  }
}

/////////////////  CONTROL DE LA FECHA  /////////////////

void Menus::displayFecha()
{  
  EEPROM.get(DAY_ADDRESS, dia);
  EEPROM.get(MONTH_ADDRESS, mes);
  EEPROM.get(YEAR_ADDRESS, anio);
  
  // Limpiar la pantalla LCD y colocar el cursor al principio
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0, 0);
    lcd.print("AJUSTANDO ..."); 
    lcd.setCursor(0, 1);
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0, 0);
    lcd.print("SETTING ..."); 
    lcd.setCursor(0, 1);
  }

  
  // Mostrar el día con o sin resaltado según el editIndex
  if (editIndex == 0) 
  {
    // Indicar que está seleccionado
    lcd.print(">"); 
  } 
  else 
  {
    lcd.print(" ");
  }
  if (dia < 10) {
    lcd.print("0"); // Añadir un 0 si el día es menor que 10 para mantener el formato
  }
  lcd.print(dia);
  
  // Mostrar el mes con o sin resaltado según el editIndex
  if (editIndex == 1) 
  {
    lcd.print(">");  // Indicar que está seleccionado
  }
  else
  {
    lcd.print(" ");
  }
  if (mes < 10) 
  {
    lcd.print("0"); // Añadir un 0 si el mes es menor que 10 para mantener el formato
  }
  lcd.print(mes);
  
  // Mostrar el año con o sin resaltado según el editIndex
  if (editIndex == 2) 
  {
    lcd.print(">");  // Indicar que está seleccionado
  } 
  else 
  {
    lcd.print(" ");
  }
  if (anio < 2024) 
  {
    lcd.print(" "); 
  }
  lcd.print(anio);
}

void Menus::ajustarFecha()
{
  bascularFecha = true;
  displayFecha(); 
}

void Menus::validarFecha()
{
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0, 0);
    lcd.print("FECHA AJUSTADA A");
    lcd.setCursor(3, 1);
    lcd.print(dia); 
    lcd.print("/"); 
    lcd.print(mes); 
    lcd.print("/"); 
    lcd.print(anio);
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(2, 0);
    lcd.print("DATE SET TO:");
    lcd.setCursor(3, 1);
    lcd.print(dia); 
    lcd.print("/"); 
    lcd.print(mes); 
    lcd.print("/"); 
    lcd.print(anio);
  }
  delay(5000);

  EEPROM.put(DAY_ADDRESS, dia);
  EEPROM.put(MONTH_ADDRESS, mes);
  EEPROM.put(YEAR_ADDRESS, anio);
  DateTime nuevaFecha(anio, mes, dia, 12, 30, 0);
  rtc.adjust(nuevaFecha);
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

/////////////////  CONTROL DE LA EEPROM   /////////////////

void Menus::inicializarEEPROM() 
{
  int initCheck;
  EEPROM.get(INIT_CHECK_ADDRESS, initCheck);
  if (initCheck != 12345) 
  {
    //EEPROM.put(DAY_ADDRESS, 23); // Día inicial
    //EEPROM.put(MONTH_ADDRESS, 10); // Mes inicial
    //EEPROM.put(YEAR_ADDRESS, 2024); // Año inicial
    //EEPROM.put(RPMS_ADRESS, 1500); // RPMs del motor inicial
    EEPROM.put(LITROS_TOTALES_DIRECCION, 0); // Litros totales iniciales
    EEPROM.put(IDIOMA_ADRESS, 0); // Idioma por defecto es español
    EEPROM.put(PESO_RELATIVO_ADDRESS, 0);
    //DateTime fechaPorDefecto(2024, 10, 23, 9, 45, 1);
    //rtc.adjust(fechaPorDefecto);
    resetearLitrosMensuales();
  }
}


/////////////////  CONTROL DE LA BASCULA  /////////////////
void Menus::iniciarCaliBascula()
{
  elegirCalibracion = true;
  iniciarCalibracion = false;
  calibrarPeso = true;
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(2,0);
    lcd.print("PULSE SELECT ");
    lcd.setCursor(2,1);
    lcd.print("PARA INICIAR");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(2,0);
    lcd.print("PRESS SELECT");
    lcd.setCursor(4,1);
    lcd.print("TO START");
  }
  calibracion(); 
}

void Menus::talarBascula()
{
  calibrarPeso = false;
  calibrarPeso1 = true ;
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(1,0);
    lcd.print("PONGA EL PESO");
    lcd.setCursor(2,1);
    lcd.print("PULSE SELECT");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(1,0);
    lcd.print("PUT THE WEIGHT");
    lcd.setCursor(2,1);
    lcd.print("PRESS SELECT");
  }
}

void Menus::calibrarEscala()
{
  calibrarPeso1 = false;
  finDeCalibre = true;
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("ESPERE....");
    finDeCalibracion();
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("RETIRE EL PESO");
    lcd.setCursor(2,1);
    lcd.print("PULSE  SET");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("WAIT....");
    finDeCalibracion();
    lcd.clear();
    delay(20);
    lcd.setCursor(1,0);
    lcd.print("REMOVE WEIGHT");
    lcd.setCursor(3,1);
    lcd.print("PRESS SET");
  }
  
}
 
void Menus::finalizarCalibracion()
{
  finDeCalibre = false;
  balanza.set_scale(escala); 
  balanza.tare(20);
  ultima_tara = balanza.get_offset();
  EEPROM.put(TARE_ADRESS, ultima_tara);
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, 0);
  delay(100);
}

void Menus::mostrarElPeso()
{
  elPeso = PesoActual();
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("EL PESO ES :");
    lcd.setCursor(0,1);
    lcd.print(elPeso);
    lcd.setCursor(8,1);
    lcd.print("GRAMOS");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("THE WEIGHT IS:");
    lcd.setCursor(0,1);
    lcd.print(elPeso);
    lcd.setCursor(8,1);
    lcd.print("GRAMS");
  }
}

/////////////////  CONTROL DEL VACIO AUTOMATICO  /////////////////

void Menus::vaciandoDeposito()
{
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("VACIANDO...");
    mezcla.vacioGeneral();
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("EMPTYING...");
    mezcla.vacioGeneral();
  }
}

/////////////////  CONTROL DEL RESETEO TOTAL DE DATA  /////////////////

void Menus::ReseteoTotalVerif()
{
  resetearTodo = true;
  resetearTodoVerif = false;
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("ESTAS SEGURO ??");
    lcd.setCursor(2,1);
    lcd.print("PULSE SELECT");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("ARE YOU SURE ??");
    lcd.setCursor(2,1);
    lcd.print("PRESS SELECT");
  }
}

void Menus::ReseteoTotal()
{  
  litrosTotales = 0 ;
  EEPROM.put(LITROS_TOTALES_DIRECCION, litrosTotales);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("RESETEANDO...");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("RESETTING...");
  }
  delay(2000);  
  resetearLitrosMensuales();
  mezcla.resetearTodo();
  inicializarEEPROM();
  PantallaProgramador(1);
}

void Menus::iniciando()
{
  lcd.clear();
  delay(20);
  lcd.setCursor(0,0);
  lcd.print("INICIANDO...");
}

void Menus::finalizarCiclo()
{
  inSubMenu = true;
  variarCantidad = true;
  menuIndex = 7;  
  IndexCantidad = 0;
  lcd.clear();
  delay(20);
  updateMenuDisplay();
}

 