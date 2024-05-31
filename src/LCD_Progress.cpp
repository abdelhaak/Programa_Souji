#include <Wire.h> 
#include "LCD_Progress.h"
#include "Menus.h"
#include <Arduino.h>

// ERRORES

byte zero[] = 
{
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};
byte one[] = 
{
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000,
  B10000
};
byte two[] = 
{
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000,
  B11000
};
byte three[] = 
{
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100,
  B11100
};
byte four[] = 
{
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110,
  B11110
};
byte five[] = 
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte vacio[] = 
{
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111,
  B11111
};
byte Character1[] =
{
  0b00000,
  0b01110,
  0b01110,
  0b00100,
  0b11111,
  0b00100,
  0b01010,
  0b10001
};
byte Character2[] =
{
  0b11111,
  0b10101,
  0b11111,
  0b10001,
  0b11111,
  0b01010,
  0b01010,
  0b01010
};

void lcdBarInit() 
{
  // initialize the LCD and allocate the 5 arrays to a number.
  lcd.createChar(0, zero);
  lcd.createChar(1, one);
  lcd.createChar(2, two);
  lcd.createChar(3, three);
  lcd.createChar(4, four);
  lcd.createChar(5, five);
  lcd.createChar(6, vacio); 
  lcd.createChar(7, Character1); 
  lcd.createChar(8, Character2); 
}

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn)
{
    double factor = totalCount / 16.0;  // Ancho del LCD
    int progress = count / factor;

    lcd.setCursor(0, lineToPrintOn);
    
    // Imprime el símbolo Unicode para la barra de progreso
    for (int i = 0; i < progress; i++) 
    {
      // Utiliza el símbolo Unicode para una barra gruesa (0xDB)
      lcd.write((uint8_t)0xFF); 
    }

    // Imprime espacios en blanco para el resto del ancho del LCD
    for (int i = progress; i < 16; i++) 
    {
      lcd.write(' ');
    }
 }