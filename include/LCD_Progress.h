#ifndef MY_LCDBAR_H
#define MY_LCDBAR_H

#include <LiquidCrystal.h>
#include <Arduino.h>


//These 5 arrays paint the bars that go across the screen.  
extern byte zero[];
extern byte one[];
extern byte two[];
extern byte three[];
extern byte four[];
extern byte five[];

void lcdBarInit();

void updateProgressBar(unsigned long count, unsigned long totalCount, int lineToPrintOn);

#endif