/*
 *  Library by Adam Phelps (amp@cs.stanford.edu)
 */

#include "Arduino.h"

#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(5, 6, 7, 8, 9, 10);

unsigned long lcd_update_time = 0;
char lcd_enabled = 0;
unsigned long lcd_disable_timeout = 10000;

void LCD_setup() 
{
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
  lcd.noCursor();
  lcd_enabled = 0;
  lcd.noDisplay();
}

void LCD_set(int row, int col, String text, boolean pad) 
{
  if (!lcd_enabled) {
    lcd_enabled = 1;
    lcd.display();
  }

  /* Pad the string to the full 16 characters */
  if (pad) {
    for (int i = col + text.length(); i < 16; i++) {
      text += " ";
    }
  }

  lcd.setCursor(col, row);
  lcd.print(text);
  lcd_update_time = millis();
}

void LCD_loop() 
{
  if (lcd_disable_timeout &&
      (millis() - lcd_update_time > lcd_disable_timeout)) {
    /* Disable the display when inactive */
    lcd_update_time = millis();
    lcd_enabled = 0;
    lcd.noDisplay();
  }
}
