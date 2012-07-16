#ifndef LCD_H
#define LCD_H

#include "Arduino.h"

void LCD_setup();
void LCD_set(int row, int col, String text, boolean pad);
void LCD_loop();

extern LiquidCrystal lcd;

#endif
