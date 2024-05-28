#include "Mezclas.h"

Bomba bombaAceite(PIN_BOMBA_ACEITE);
Bomba bombaSouji(PIN_BOMBA_SOUJI);
Bomba bombaAgua(PIN_BOMBA_AGUA);
Bomba bombaVacio(PIN_BOMBA_VACIO);

Motor motorMezclador(PIN_MOTOR,PIN_SENSOR);
Menus menus(lcd);

// 2 minutos => 120000 ms    ::   PARA LA PRIMERA MEZCLA
uint64_t tiempoMezcla1 = 5000;
// 3 minutos => 180000 ms    ::   PARA LA SEGUNDA MEZCLA
uint64_t tiempoMezcla2 = 5000;
// El peso minimo del vacio autorizado
uint64_t pesoMinimo = 30.0;
// El tiempo de error autorizado de la bascula 
// iniciado en 3 minutos => 180000
uint64_t tiempoErrorBascula = 180000;

uint64_t tiempoPasado = 0;
float porcentajeAceite = 30.0;
float porcentajeSouji = 50.0;

Mezclas::Mezclas()
{
  pesoAceiteDeseado = 0;
  pesoSoujiDeseado = 0;
  volumenAceite = 0.0;
  volumenSouji = 0.0;
  volumenAgua = 0.0 ;
  pesoAgregado = 0.0;
  init();
}
void Mezclas::init()
{
    parado();

}

void Mezclas::parado()
{
    bombaAceite.off();
    bombaSouji.off();
    bombaAgua.off();
    bombaVacio.off();
    motorMezclador.parar();
}

void Mezclas::mezclaGeneral(int cantidadSouji)
{

  Pantallamezcla(0);
  delay(1000);
  calcularVolumen();

  // Hechamos la cantidad adecuada para el Aceite
  Serial.println("Bomba Aceite activada : ");
  Pantallamezcla(1);
  bombaAceite.on();
  hecharLiquido(volumenAceite);
  Serial.println("Apagando Bomba de Aceite");
  bombaAceite.off();

  // Hechamos la cantidad adecuada para el SOUJI
  Serial.println("Bomba Souji activada : ");
  Pantallamezcla(2);
  bombaSouji.on();
  hecharLiquido(volumenSouji);
  Serial.println("Apagando Bomba de Souji");
  bombaSouji.off();

  // Pasamos a la primera etapa de mezcla con el motor 
  Serial.println("Ahora activamos el motor mezclador");
  delay(1000);
  Serial.print("Motor mezclador activo en :  ");
  Serial.print(RPMS_INICIO);
  Serial.println(" RPMs");
  Pantallamezcla(4);
  // Activamos el motor con los RPMs guardados y el tiempo adecuado
  motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla1);
  // Apagar el motor y pasar a la siguiente etapa
  motorMezclador.parar();

  // Hechamos la cantidad adecuada de AGUA
  Serial.println("Bomba Agua activada : ");
  Pantallamezcla(3);
  bombaAgua.on();
  hecharLiquido(volumenAgua);
  Serial.println("Apagando Bomba de Agua");
  bombaAgua.off();

  // Pasamos a la segunda etapa de mezcla con el motor 
  Serial.println("Ahora activamos el motor mezclador por la segunda vez");
  delay(1000);
  Serial.print("Motor mezclador activo en :  ");
  Serial.print(RPMS_INICIO);
  Serial.println(" RPMs");
  Pantallamezcla(5);
  // Activamos el motor con los RPMs guardados y el tiempo adecuado
  motorMezclador.ajustarRpms(RPMS_INICIO,tiempoMezcla2);
  // Apagar el motor y pasar a la siguiente etapa
  motorMezclador.parar();

  // Pasamos a la etapa del vacio
  Serial.println("Empezamos el vacio....");
  Pantallamezcla(6);
  mezclaVacio();
  Serial.println("Vacio finalizado");
  Serial.println("Mezcla terminada.");
  Pantallamezcla(7);
  delay(2000);
  menus.PantallaSeleccionada(0);
}

/*
void Mezclas::mezclaGeneralizada(int cantidadTotal) {
  int cantidadDeCincoLitros = cantidadTotal / 5;
  
  for (int i = 0; i < cantidadDeCincoLitros; i++) {
    laMezcla(5);
  }
  
  int cantidadRestante = cantidadTotal % 5;
  if (cantidadRestante) {
    laMezcla(cantidadRestante);
  }
  Serial.println("Mezcla terminada.");
  menus.PantallaSeleccionada(0);
}
*/

void Mezclas::Pantallamezcla(uint8_t pantallamezcla)
{
  // Limpieza de la pantalla 
  if(pantallamezcla != misPantallasMezc)
  {
    lcd.clear();
    misPantallasMezc = pantallamezcla;
  }

  if (pantallamezcla == 0)
  {

    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("AHORA EMPEZAMOS");
    lcd.setCursor(0,4);
    lcd.print("LA MEZCLA");
  }
  // Pantalla hechando Aceite
  if (pantallamezcla == 1)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HECHANDO ACEITE");
  }
  // Pantalla hechando Souji
  if (pantallamezcla == 2)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HECHANDO SOUJI");
  }
  // Pantalla hechando Agua
  if (pantallamezcla == 3)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("HECHANDO AGUA");
  }
  // Pantalla Mezclando Primera Mezcla
  if (pantallamezcla == 4)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("PRIMERA MEZCLA");
  }
  // Pantalla Mezclando Primera Mezcla
  if (pantallamezcla == 5)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("SEGUNDA MEZCLA");
  }
  // Pantalla del VACIO
  if (pantallamezcla == 6)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("VACIANDO ....");
  }
  // Pantalla de fin de mezcla
  if (pantallamezcla == 7)
  {
    lcd.clear();
    lcd.setCursor(5,0);
    lcd.print("MEZCLA");
    lcd.setCursor(3,1);
    lcd.print("FINALIZADA");
  }
  // Pantalla Ajustar porcentaje Aceite
  if (pantallamezcla == 8)
  {
    Serial.println("pantalla 8");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AJUSTANDO ACEITE");
    lcd.setCursor(5,1);
    lcd.print(porcentajeAceite);
    lcd.setCursor(10,1);
    lcd.print("%");

  }
  // Pantalla Ajustar porcentaje Souji
  if (pantallamezcla == 9)
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("AJUSTANDO SOUJI");
    lcd.setCursor(5,1);
    lcd.print(porcentajeSouji);
    lcd.setCursor(10,1);
    lcd.print("%");
  }
}



void Mezclas::mezclaVacio()
{
  Serial.println("Iniciamos el vacio");
  Serial.println("Activamos la bomba");
  bombaVacio.on();
  unsigned long tiempoInicio = millis(); // Guardar el tiempo de inicio
  float pesoInicial = PesoActual();
  float pesoVaciado;
  float pesoActual; 
  float pesoTotalAVaciar = pesoInicial - pesoMinimo;

  while(PesoActual() > pesoMinimo)
  {
    pesoActual = PesoActual();
    pesoVaciado = pesoInicial - pesoActual; 
    updateProgressBar(pesoVaciado, pesoTotalAVaciar, 1); // Actualizar la barra de progreso
    
    delay(500);
    if (millis() - tiempoInicio > tiempoErrorBascula)
    {
      Serial.println("Error de la bascula : Tiempo máximo de espera alcanzado");
      Serial.println("Apagamos la Bomba ");
      bombaVacio.off();
      return;
    }
  }  
  Serial.println("Bomba apagada");
  bombaVacio.off();
  Serial.println("Proceso finalizado");
}

void Mezclas::calcularVolumen()
{
  // Calculos de la cantidad de Aceite
  pesoAceiteDeseado = porcentajeAceite * CAPACIDAD_TOTAL / 100.0;
  volumenAceite = pesoAceiteDeseado / DENSIDAD_ACEITE;
  // Calculos de la cantidad de Souji
  pesoSoujiDeseado = porcentajeSouji * CAPACIDAD_TOTAL /100.0;
  volumenSouji = pesoSoujiDeseado / DENSIDAD_SOUJI;
  // Calculos de la cantidad de Agua que es lo que queda
  volumenAgua = CAPACIDAD_TOTAL - (pesoAceiteDeseado + pesoSoujiDeseado);
}

void Mezclas::hecharLiquido(float volumen)
{
  pesoInicioEtapa = PesoActual();
  float pesoRelativo = 0;

  while(pesoRelativo < volumen)
  {
    pesoRelativo = PesoActual() - pesoInicioEtapa;
    updateProgressBar(pesoRelativo, volumen, 1);  
    Serial.print("El peso es : ");
    Serial.println(pesoRelativo);
    delay(100);
  }
}


void Mezclas::subirPorcentajeAceite()
{
  porcentajeAceite++;
  Pantallamezcla(8);
}
void Mezclas::subirPorcentajeSouji()
{
  porcentajeSouji++;
  Pantallamezcla(9);
}

void Mezclas::bajarPorcentajeAceite()
{
  porcentajeAceite--;
  Pantallamezcla(8);
}

void Mezclas::bajarPorcentajeSouji()
{
  porcentajeSouji--;
  Pantallamezcla(9);
}
