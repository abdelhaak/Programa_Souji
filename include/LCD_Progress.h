#ifndef MY_LCDBAR_H
#define MY_LCDBAR_H

#include <LiquidCrystal.h>
#include <Arduino.h>

// Caracteres de la LCD por añadir
extern byte zero[];
extern byte one[];
extern byte two[];
extern byte three[];
extern byte four[];
extern byte five[];
extern byte Character1[];
extern byte Character2[];

void lcdBarInit();
void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn);

#endif