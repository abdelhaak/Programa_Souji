#include "Menus.h"


// ERRORES

// MENUS GENERALES

int idioma = 0 ;
int opcionLenguaje = 0; 
int opcionCalibre = 0;
const int PAUSE = 99;
bool enPausa = false;
bool pausado = false;
/*menuIndex = 0; 
numMezclas = 0;
estado = 0;
estado2 = 0;
i_mezclas = 0;*/
const uint8_t RS = A3, EN = A2, D4 = A0, D5 = 0, D6 = 1, D7 = 2;
//const uint8_t RS = 22, EN = 23, D4 = 31, D5 = 30, D6 = 29, D7 = 28;
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
}

///////////////// Pantallas del MENU Principal   /////////////////
void Menus::PantallaSeleccionada(uint8_t pantalla)
{
  // Limpieza de la pantalla 
  if(pantalla != misPantallas)
  {
    serial.println("Estamos en misPantallas");
    lcd.clear();
    delay(20);
    misPantallas = pantalla;
  }
  
  // Pantalla de Cantidad de Souji
  if (pantalla == 0)
  {  
    serial.println("Estamos en la pantalla 0");
    EEPROM.get(SCALE_ADDRESS, escala);
    EEPROM.get(STATUS_ADRESS, mezcla.estado);
    EEPROM.get(STATUS_2_ADRESS, mezcla.estado2);
    EEPROM.get(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    EEPROM.get(PESO_ACEITE_ACTUAL_ADRESS, mezcla.pesoLiquido);
    serial.println("Pasamos del EEPROM");
    menuPrincipal = true;
    menuProgramador = false;
    mostrarLitrosMensuales = false;
    inSubMenu = false;
    menuIndex = 0;
    serial.println("Pasamos dE LA DECLARACION");
    if (mezcla.estado == 0)
    {
      if(idioma == 0)
      {
        serial.println("Pasamos dentro");
        lcd.setCursor(0,0);
        lcd.print("CANTIDAD SOUJI");
        lcd.setCursor(0,1);
        lcd.print("LITROS:");
        lcd.setCursor(9,1);
        lcd.print(Cantidad_Souji[IndexCantidad]);
        lcd.setCursor(11,1);
        lcd.print("/");
        lcd.setCursor(12,1);
        lcd.print("25");
      }
      else
      {
        lcd.setCursor(2,0);
        lcd.print("SOUJI AMOUNT");
        lcd.setCursor(0,1);
        lcd.print("LITERS:");
        lcd.setCursor(9,1);
        lcd.print(Cantidad_Souji[IndexCantidad]);
        lcd.setCursor(11,1);
        lcd.print("/");
        lcd.setCursor(12,1);
        lcd.print("25");
      }
    }
    else
    {
      mezcla.mezclaGeneral(mezcla.numMezclas);
    }
  }
  
  // Pantalla de Litros Mensuales
  if (pantalla == 1)
  {
    serial.println("Estamos en la pantalla 1");
    mezcla.estado = 0;
    EEPROM.put(STATUS_ADRESS, mezcla.estado);
    if(mes == 0)mes =1;
    mostrarLitrosMensuales = false;
    if(idioma == 0)
    {     
      lcd.setCursor(4,0);
      lcd.print("MOSTRAR");
      lcd.setCursor(0,1);
      lcd.print("LITROS MENSUALES");
    }
    else
    {
      lcd.setCursor(6,0);
      lcd.print("SHOW");
      lcd.setCursor(1,1);
      lcd.print("MONTHLY LITERS");
    }
  }
  
  // Pantalla de Litros Totales
  if (pantalla == 2)
  { 
    serial.println("Estamos en la pantalla 2");
    EEPROM.get(LITROS_TOTALES_DIRECCION, litrosTotales);
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("LITROS TOTALES");
      lcd.setCursor(0,1);
      lcd.print(litrosTotales);
      lcd.setCursor(8,1);
      lcd.print("LITROS");
    }
    else
    {    
      lcd.setCursor(2,0);
      lcd.print("TOTAL LITERS");
      lcd.setCursor(0,1);
      lcd.print(litrosTotales);
      lcd.setCursor(8,1);
      lcd.print("LITERS");
    }
  }
  
  // El Menu de la FECHA
  if (pantalla == 3)
  { 
    EEPROM.get(DAY_ADDRESS, dia);
    EEPROM.get(MONTH_ADDRESS, mes);
    EEPROM.get(YEAR_ADDRESS, anio);

    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("LA FECHA ES :");
      lcd.setCursor(4,1);
      lcd.print(dia);
      lcd.print("/");
      lcd.print(mes);
      lcd.print("/");
      lcd.print(anio);
    }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("THE DATE IS :");
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
    vacioAutomatico = false;
    if(idioma==0)
    {
      lcd.setCursor(5,0);
      lcd.print("VACIAR");
      lcd.setCursor(4,1);
      lcd.print("DEPOSITO");
    }
    else
    {
      lcd.setCursor(6,0);
      lcd.print("EMPTY");
      lcd.setCursor(4,1);
      lcd.print("CONTAINER");
    }
  }
  
  // El Menu de Calibracion de Bascula
  if (pantalla == 5)
  {
    //mostrarPeso = false;
    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("CALIBRACION");
      lcd.setCursor(4,1);
      lcd.print("BASCULA");
    }
    else
    {
      lcd.setCursor(5,0);
      lcd.print("SCALE ");
      lcd.setCursor(2,1);
      lcd.print("CALIBRATION");
    }
  }
  
  // El Menu de Lenguaje
  if (pantalla == 6)
  {
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
      lcd.setCursor(0,0);
      lcd.print("ELIGE CANTIDAD:");
      lcd.setCursor(3,1);
      lcd.print(Cantidad_Souji[IndexCantidad]);  
      lcd.setCursor(7,1);
      lcd.print("LITROS");
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
    for (int i = 0; i < 13; ++i) 
    {
      int direccion = LITROS_MENSUALES_DIRECCION + i * TAMANIO_DATOS_MENSUALES;
      EEPROM.get(direccion, litrosMensuales[i]);
    }
    mostrarLitrosMensuales = true;
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
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("AJUSTAR FECHA ");
      lcd.setCursor(3,1);
      lcd.print("PULSE SEL ");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(4,0);
      lcd.print("SET DATE ");
      lcd.setCursor(3,1);
      lcd.print("PULSE SEL ");
    }
  }
  
  // El SubMenu de Vaciar Deposito
  if (pantalla == 11)
  {
    vacioAutomatico = true;
    //vaciando = false;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("INICIAR VACIO");
      lcd.setCursor(3,1);
      lcd.print("PULSE SEL");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("START VACUUM");
      lcd.setCursor(3,1);
      lcd.print("PRESS SEL");
    }
  }
  
  // El SubMenu de Calibracion
  if (pantalla == 12)
  {
    elegirCalibracion = true;
    /*ultima_tara = balanza.get_offset();
    EEPROM.put(sizeof(escala), ultima_tara);*/
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
    lcd.setCursor(4, 0);
    lcd.print("CALIBRAR");
    lcd.setCursor(4, 1);
    lcd.print("PESAR");
  }

    /*iniciarCalibracion = true;
    if(idioma==0)
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(1,0);
      lcd.print("INICIAR CALIB.");
      lcd.setCursor(3,1);
      lcd.print("PULSE SEL");
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(2,0);
      lcd.print("START CALIB.");
      lcd.setCursor(3,1);
      lcd.print("PULSE SEL");
    } */
  
  
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
    lcd.setCursor(4, 0);
    lcd.print("ESPANOL");
    lcd.setCursor(4, 1);
    lcd.print("INGLES");
  }
}

void Menus::updateMenuDisplay()
{
  serial.println("Estamos en decrementandoIndex");
  serial.print("menuIndex antes: ");
  serial.println(menuIndex);
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
      Menus::PantallaSeleccionada(7);
      break;
      case 1:
      Menus::PantallaSeleccionada(8);
      break;
      case 2:
      Menus::PantallaSeleccionada(9);
      break;
      case 3:
      Menus::PantallaSeleccionada(10);
      break;
      case 4:
      Menus::PantallaSeleccionada(11);
      break;
      case 5:
      Menus::PantallaSeleccionada(12);
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
      lcd.setCursor(1,0);
      lcd.print("ESTAMOS EN EL");
      lcd.setCursor(0,1);
      lcd.print("MODO PROGRAMADOR");  
    }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("THIS IS THE");
      lcd.setCursor(0,1);
      lcd.print("PROGRAMMING MODE");
    }
  }
  
  // Pantalla de Ajustar la cantidad del ACEITE
  if (pantallaProg == 1)
  {
    serial.print("porcentajeAceite : ");
    serial.println(mezcla.porcentajeAceite);
    ajustarAceite = false;
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("AJUSTAR CANTIDAD");
      lcd.setCursor(0,1);
      lcd.print("DE : ACEITE");
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("SETTING THE");
      lcd.setCursor(0,1);
      lcd.print("OIL QUANTITY");
    }
  }

  // Pantalla de Ajustar la cantidad del SOUJI
  if (pantallaProg == 2)
  {
    serial.print("porcentajeSouji : ");
    serial.println(mezcla.porcentajeSouji);
    ajustarSouji = false ;
    if(idioma==0)
    {
      lcd.setCursor(0,0);
      lcd.print("AJUSTAR CANTIDAD");
      lcd.setCursor(2,1);
      lcd.print("DE : SOUJI");
    }
    else
    {
      lcd.setCursor(0,0);
      lcd.print("SETTING THE");
      lcd.setCursor(2,1);
      lcd.print("SOUJI QUANTITY");
    }
  }

  // Pantalla de RESETEAR LA CANTIDAD DE LITROS ACUMULADA
  if (pantallaProg == 3)
  {
    resetearTodo = false;
    if(idioma==0)
    {
      lcd.setCursor(2,0);
      lcd.print("RESETEAR LOS");
      lcd.setCursor(1,1);
      lcd.print("LITROS TOTALES");
    }
    else
    {
      lcd.setCursor(2,0);
      lcd.print("RESET THE");
      lcd.setCursor(1,1);
      lcd.print("TOTAL LITERS");
    }
  }

  // Pantalla de Ajustar LA VELOCIDAD DEL MOTOR
  if (pantallaProg == 4)
  {
    if(idioma==0)
    {
      lcd.setCursor(3,0);
      lcd.print("AJUSTAR RPMS");
      lcd.setCursor(5,1);
      lcd.print("DEL MOTOR");
    }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("SETTING THE :");
      lcd.setCursor(3,1);
      lcd.print("MOTOR RPMS");
    }
  }

  // SUBMENU de Ajustar la cantidad del ACEITE
  if (pantallaProg == 5)
  {
    ajustarAceite = true;
    if(idioma==0)
    {
      lcd.setCursor(3,0);
      lcd.print("AJUSTANDO");
      lcd.setCursor(5,1);
      lcd.print("ACEITE ...");
    }
    else
    {
      lcd.setCursor(3,0);
      lcd.print("SETTING ");
      lcd.setCursor(5,1);
      lcd.print("OIL ...");
    }
  }

  // SUBMENU de Ajustar la cantidad del SOUJI
  if (pantallaProg == 6)
  {
    ajustarSouji = true;
    if(idioma==0)
    {
      lcd.setCursor(3,0);
      lcd.print("AJUSTANDO");
      lcd.setCursor(5,1);
      lcd.print("SOUJI ...");
    }
    else
    {
      lcd.setCursor(3,0);
      lcd.print("SETTING ");
      lcd.setCursor(5,1);
      lcd.print("SOUJI ...");
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
      lcd.setCursor(3,1);
      lcd.print("PRESS SEL");
    }
    else
    {
      lcd.setCursor(1,0);
      lcd.print("TO DELETE ALL");
      lcd.setCursor(3,1);
      lcd.print("PRESS SEL");
    }
  }

  // Pantalla de Ajustar LA VELOCIDAD DEL MOTOR
  if (pantallaProg == 8)
  {
    validarRpms = false;
    accederRpms = true;
    if(idioma==0)
    {
      lcd.setCursor(3,0);
      lcd.print("PULSE SEL");
      lcd.setCursor(2,1);
      lcd.print("MODIFICAR");
    }
    else
    {
      lcd.setCursor(3,0);
      lcd.print("PRESS SEL");
      lcd.setCursor(4,1);
      lcd.print("TO MODIFY");
    }
  }
}

void Menus::entrarMenuProg()
{
  PantallaProgramador(0);
}

void Menus::salirMenuProg()
{
  menuPrincipal = true;
  menuProgramador = false;
  PantallaSeleccionada(0);
}

void Menus::entrarSubMenuProg()
{
  if(!SubMenuProgamador)
  {
    SubMenuProgamador = true;
    lcd.clear();
    delay(20);
    switch (menuProgIndex) 
    {
      case 0:
        PantallaProgramador(4);
        break;
      case 1:
        PantallaProgramador(5);
        break;
      case 2:
        PantallaProgramador(6);
        break;
      case 3:
        PantallaProgramador(7);
        break;
      case 4 :
        PantallaProgramador(8);   
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


//////////////// PANTALLAS DEL MODO PROGRAMADOR  /////////////////



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
    if(definirFecha && !bascularFecha)
    {
      //bascularFecha = true;
      ajustarFecha();
    } 
    else if(definirFecha && bascularFecha)
    {
      pasarFecha();
    }
    /*else if(iniciarCalibracion)
    {
      iniciarCaliBascula();
    }*/
    else if(calibrarPeso)
    {
      talarBascula();
    }
    else if(calibrarPeso1)
    {
      calibrarEscala();
    }
    /*else if(mostrarPeso)
    {
      mostrarElPeso();
    }*/
    else if(vacioAutomatico)
    {
      vaciandoDeposito();
    }
    else if(variarCantidad)
    {
      serial.println("variar cantidad true");
      ejecutarMezcla(Cantidad_Souji[IndexCantidad]);
    } 
    else if(cambiarIdioma)
    {
      idioma = opcionLenguaje;
      cambiarIdioma = false;
      EEPROM.put(IDIOMA_ADRESS,idioma);
      PantallaSeleccionada(0);
    }
    else if(elegirCalibracion)
    {
      elegirCalibracion = false;
      if(opcionCalibre == 0)
      {
        iniciarCaliBascula();
      }
      else
      {
        mostrarElPeso();
      }
    }
    else if(mezcla.enPausa)
    {
      serial.println("enPausa es true");
      mezcla.enPausa = !mezcla.enPausa;
      if(mezcla.enPausa)
      {
        mezcla.Pantallamezcla(8);
      }
      else
      {
        EEPROM.get(STATUS_ADRESS,mezcla.estado);
        EEPROM.get(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
        serial.println("reanudando...");
        mezcla.mezclaGeneral(mezcla.numMezclas);
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
      motor.mostrarRpms(pin_encoder);
    }
    else if (ajustarAceite)
    {
      serial.println("AjutarAceite");
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(8);
    } 
    else if (ajustarSouji)
    {
      SubMenuProgamador = false;
      mezcla.Pantallamezcla(9);
    } 
    else {}  
  } 
}

void Menus::decrementandoIndex() 
{
    if (!inSubMenu && menuPrincipal && menuIndex < 6) 
    {
      serial.println("Estamos en decrementandoIndex");
      serial.print("menuIndex antes: ");
      serial.println(menuIndex);
      menuIndex++;
      serial.print("menuIndex despues: ");
      serial.println(menuIndex);
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
    else if(menuProgramador && !inSubMenuProg && menuProgIndex < 4 && !ajustarAceite && !ajustarSouji && !validarRpms)
    {
      menuProgIndex++;
      updateMenuProgDisplay(); 
    }
    else if(ajustarAceite)
    {
      serial.println("decremantoAceite");
      mezcla.bajarPorcentajeAceite();   
    }
    else if(ajustarSouji)
    {
      mezcla.bajarPorcentajeSouji();    
    }
    else if(validarRpms)
    {
      motor.bajarRpms();
    }
    else if(cambiarIdioma)
    {
      opcionLenguaje = 1;
      PantallaSeleccionada(13);
    }
    else if(elegirCalibracion)
    {
      opcionCalibre = 1;
      PantallaSeleccionada(12);
    }
    else
    {
    }
}

void Menus::decrementandoIndexRapido()
{
  if(definirFecha)
  {
    bajaFechaRapido();
    displayFecha();
  }
}

void Menus::incrementandoIndex() 
{
    if (!inSubMenu && menuPrincipal && menuIndex > 0)
    {
      serial.println("Estamos en incrementandoIndex");
      serial.print("menuIndex antes: ");
      serial.println(menuIndex);
      menuIndex--;
      serial.print("menuIndex despues: ");
      serial.println(menuIndex);
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
    else if(menuProgramador && !inSubMenuProg && menuProgIndex > 0 && !ajustarAceite && !ajustarSouji && !validarRpms)
    {
      menuProgIndex--;
      updateMenuProgDisplay(); 
    }
    else if(ajustarAceite)
    {
      serial.println("incremantoAceite");
      mezcla.subirPorcentajeAceite();   
    }
    else if(ajustarSouji)
    {
      mezcla.subirPorcentajeSouji();    
    }
    else if(validarRpms)
    {
      motor.subirRpms();
    }
    else if(cambiarIdioma)
    {
      opcionLenguaje = 0 ;
      PantallaSeleccionada(13);
    }
    else if(elegirCalibracion)
    {
      opcionCalibre = 0;
      PantallaSeleccionada(12);
    }
    else
    {
    }
}

void Menus::incrementandoIndexRapido()
{
  if(definirFecha)
  {
    subeFechaRapido();
    displayFecha();
  }
}

////////////////   Programas de las mezclas     /////////////////
void Menus::updateCantidadSouji()
{
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("ELIGE CANTIDAD:");
    lcd.setCursor(3,1);
    lcd.print(Cantidad_Souji[IndexCantidad]);
    lcd.setCursor(7,1);
    lcd.print("LITROS");
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
      lcd.print(" HAY:");
      lcd.setCursor(3,1);
      lcd.print(litrosMensuales[mes]); // Muestra los litros mensuales del mes actual
      lcd.setCursor(8,1);
      lcd.print("LITROS");  
    }
    else
    {
      lcd.clear();
      delay(20);
      lcd.setCursor(0,0);
      lcd.print(elegirMes(mes)); // Muestra el nombre del mes actual
      lcd.print(" THERE");
      lcd.setCursor(0,1);
      lcd.print("ARE:");
      lcd.setCursor(5,1);
      lcd.print(litrosMensuales[mes]); // Muestra los litros mensuales del mes actual
      lcd.setCursor(10,1);
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

/*
void Menus::validarMezcla()
{
  if(variarCantidad)
  {
    
    ejecutarMezcla(Cantidad_Souji[IndexCantidad]);
  }
}
*/
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
  serial.println("Estamos en ejecutarMezcla");
  motor.cargarRpms();
  switch (Cantidad_Souji)
  {
  case 5:
    incrementarCantidad(5);
    serial.println("caso de 5");
    mezcla.numMezclas = 1;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    serial.println("vAMOS A L AMEZCLA");
    serial.print("nUM DE MEZCLA: ");
    serial.println(mezcla.numMezclas);
    mezcla.mezclaGeneral(mezcla.numMezclas);
    incrementarCantidad(5);
  break;
  case 10:
    mezcla.numMezclas = 2;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    mezcla.mezclaGeneral(mezcla.numMezclas);;
    incrementarCantidad(10);
  break;
  case 15:
    mezcla.numMezclas = 3;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    mezcla.mezclaGeneral(mezcla.numMezclas);
    incrementarCantidad(15);
  break;
  case 20:
    mezcla.numMezclas = 4;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    mezcla.mezclaGeneral(mezcla.numMezclas);
    incrementarCantidad(20);
  break;
  case 25:
    mezcla.numMezclas = 5;
    EEPROM.put(NUM_MEZCLAS_ADRESS, mezcla.numMezclas);
    mezcla.mezclaGeneral(mezcla.numMezclas);
    incrementarCantidad(25);
  break;
  }
}

void Menus::resetearLitrosMensuales()
{
  for (int i = 0; i < 13; ++i) 
  {
    litrosMensuales[i] = 0; // Reseteamos los litros mensuales a cero
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
  EEPROM.put(DAY_ADDRESS, dia);
  EEPROM.put(MONTH_ADDRESS, mes);
  EEPROM.put(YEAR_ADDRESS, anio);
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0, 0);
    lcd.print("FECHA AJUSTADA A");
    lcd.setCursor(0, 1);
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
    lcd.setCursor(0, 0);
    lcd.print("DATE SET TO :");
    lcd.setCursor(0, 1);
    lcd.print(dia); 
    lcd.print("/"); 
    lcd.print(mes); 
    lcd.print("/"); 
    lcd.print(anio);
  }

  delay(1000);
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

void Menus::subeFechaRapido()
{
  switch (editIndex)
  {
  case 0 :
    if(dia<31)
    {dia+=2;}
    else
    {dia = 1;}
    EEPROM.put(DAY_ADDRESS, dia);
    break;
  case 1 :
    if(mes<12)
    {mes+=2;}
    else
    {mes = 1;}
    EEPROM.put(MONTH_ADDRESS, mes);
    break;
  case 2 :
    if(anio<3000)
    {anio++;}
    else
    {anio = 2024;}
    EEPROM.put(YEAR_ADDRESS, anio);
    break;
    
  }
  delay(300);
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

void Menus::bajaFechaRapido()
{
  switch (editIndex)
  {
  case 0 :
    if(dia>1)
    {dia-=2;}
    else
    {dia = 31;}
    EEPROM.put(MONTH_ADDRESS, dia);
    break;
  case 1 :
    if(mes>1)
    {mes-=2;}
    else
    {mes = 12;}
    EEPROM.put(MONTH_ADDRESS, mes);
    break;
  case 2 :
    if(anio>2024)
    {anio-=2;}
    else
    {anio=3000;}
    EEPROM.put(YEAR_ADDRESS, anio);
    break;
  }
  delay(300);
}


/////////////////  CONTROL DE LA EEPROM   /////////////////

void Menus::inicializarEEPROM() 
{
    int initCheck;
    EEPROM.get(INIT_CHECK_ADDRESS, initCheck);
    if (initCheck != 12345) 
    {
        EEPROM.put(DAY_ADDRESS, 24); // Día inicial
        EEPROM.put(MONTH_ADDRESS, 6); // Mes inicial
        EEPROM.put(YEAR_ADDRESS, 2024); // Año inicial
        EEPROM.put(RPMS_ADRESS, 1500); // RPMs del motor inicial
        EEPROM.put(LITROS_TOTALES_DIRECCION, 0); // Litros totales iniciales
        EEPROM.put(IDIOMA_ADRESS, 0); // Idioma por defecto es español
        EEPROM.put(PORCENTAJE_ACEITE_ADRESS, 30); // Porcentaje de aceite inicial
        EEPROM.put(PORCENTAJE_SOUJI_ADRESS, 50); // Porcentaje de Souji inicial
        EEPROM.put(STATUS_ADRESS, 0); // Estado inicial
        EEPROM.put(STATUS_2_ADRESS, 0); // Estado 2 inicial
        EEPROM.put(NUM_MEZCLAS_ADRESS, 1); // Numero de mezclas inicial
        EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, 0);
        resetearLitrosMensuales();
    }
}


/////////////////  CONTROL DE LA BASCULA  /////////////////
void Menus::iniciarCaliBascula()
{
  serial.println("Antes de BalanzaSetup ");
  serial.println("Despues de BalanzaSetup ");
  iniciarCalibracion = false;
  calibrarPeso = true;
  if(idioma==0)
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(3,0);
    lcd.print("PULSE SEL :");
    lcd.setCursor(2,1);
    lcd.print("PARA INICIAR");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(3,0);
    lcd.print("PRESS SEL :");
    lcd.setCursor(4,1);
    lcd.print("TO START");
  }
  calibracion(); 
  serial.println("Despues de calibracion() ");
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
    lcd.print("PONGA EL PESO :");
    lcd.setCursor(4,1);
    lcd.print("PULSE SEL");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(1,0);
    lcd.print("PUT THE WEIGHT");
    lcd.setCursor(4,1);
    lcd.print("PRESS SEL");
  }

}

void Menus::calibrarEscala()
{
  serial.print("Despues de calibrarEscala ");
  finDeCalibre = true;
  calibrarPeso1 = false;
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
    lcd.print("PULSE SET");
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
  serial.print("Antes ");
  /*Serial.print("ElPeso es : ");
  Serial.println(PesoActual());*/
  balanza.set_scale(escala); // Establecemos la escala
  balanza.tare(20);  //El peso actual de la base es considerado zero.
  ultima_tara = balanza.get_offset();
  EEPROM.put(TARE_ADRESS, ultima_tara);
  EEPROM.put(PESO_ACEITE_ACTUAL_ADRESS, 0);
  serial.print("Despues ");

  /*Serial.print("ElPeso es : ");
  Serial.println(PesoActual());*/
}

void Menus::mostrarElPeso()
{
  /*ultima_tara = balanza.get_offset();
  EEPROM.put(TARE_ADRESS, ultima_tara);*/
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
  //vaciando = true;
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
  delay(2000);
  PantallaSeleccionada(11);
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
    lcd.setCursor(3,1);
    lcd.print("PULSE SEL");
  }
  else
  {
    lcd.clear();
    delay(20);
    lcd.setCursor(0,0);
    lcd.print("ARE YOU SURE ??");
    lcd.setCursor(3,1);
    lcd.print("PRESS SEL");
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
  PantallaProgramador(0);
}

void Menus::iniciando()
{
  lcd.clear();
  delay(20);
  lcd.setCursor(0,0);
  lcd.print("INICIANDO...");
}