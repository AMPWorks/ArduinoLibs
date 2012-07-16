/*
 *  Library by Adam Phelps (amp@cs.stanford.edu)
 */

#include "Arduino.h"
#include "LiquidCrystal.h"

#include "Debug.h"
#include "LCD.h"
#include "Menu.h"

/*
 ******************************************************************************
 * MenuItem
 *
 ******************************************************************************
 */
 
MenuItem::MenuItem(String text) 
{
  menuText = text;
  selectedText = NULL;
}

MenuItem::MenuItem(String _menuText, String _selectedText,
           menu_action_t _action_ptr, void *_action_args)
{
  menuText = _menuText;
  selectedText = _selectedText;
  action_ptr = _action_ptr;
  action_args = _action_args;
  actionText = "";
}

int
MenuItem::action(void) 
{
  if (action_ptr != NULL)
    return action_ptr(this, action_args);
  else
    return 0;
}


void
MenuItem::select(boolean _selected) 
{
  selected = _selected;
}


/*
 ******************************************************************************
 * Menu
 *
 ******************************************************************************
 */

/* Construct a menu with an array of menu items */
Menu::Menu(int _numItems, MenuItem **_items, LiquidCrystal *_lcd)
{
  numItems = _numItems;
  items = _items;
  current = 0;
  lcd = _lcd;
}


void
Menu::next(void)
{
  current = (current + 1) % numItems;
  display();
  DEBUG_PRINT(1, "next now:0x");
  DEBUG_PRINT_HEX(1, current);
  DEBUG_PRINT(1, " max:");
  DEBUG_PRINT(1, numItems);
  DEBUG_PRINT(1, "\n");
}

void
Menu::prev(void)
{
  current--;
  if (current == (byte)(-1)) current = numItems - 1;
  display();
  DEBUG_PRINT(1, "prev now:0x");
  DEBUG_PRINT_HEX(1, current);
  DEBUG_PRINT(1, " max:");
  DEBUG_PRINT(1, numItems);
  DEBUG_PRINT(1, "\n");
}

void
Menu::enter(void) 
{
  lcd->clear();
  if (selected) {
    selected = false;
    currentItem()->select(false);
    display();
  } else {
    selected = true;
    currentItem()->select(true);
    display();
  }
}

int
Menu::action(void) 
{
  if (selected) {
    return currentItem()->action();
  } else {
    return 0;
  }
}


inline MenuItem *
Menu::currentItem(void) 
{
  return items[current];
}

inline MenuItem *
Menu::nextItem(void)
{
  return items[(current + 1) % numItems];
}

/* Print the current and next menu item */
void
Menu::display(void) 
{
  if (selected) {
    LCD_set(0, 0, currentItem()->selectedText, true);
    LCD_set(1, 0, currentItem()->actionText, true);
  } else {
    LCD_set(0, 0, currentItem()->menuText, true);
    LCD_set(1, 0, nextItem()->menuText, true);
    lcd->setCursor(0,0);
    lcd->blink();
  }
}
