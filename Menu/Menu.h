/*
 *  Library by Adam Phelps (amp@cs.stanford.edu)
 */
#ifndef MENU_H
#define MENU_H

#include "Arduino.h"
#include <LiquidCrystal.h>

class MenuItem;
typedef int (*menu_action_t)(MenuItem *item, void *arg);

class MenuItem 
{
  public:
  MenuItem(String _menuText);
  MenuItem(String _menuText, String _selectedText);
  MenuItem(String _menuText, String _selectedText,
           menu_action_t _action, void *_action_args);
  void select(boolean _selected);
  int action(void);

  String menuText;
  String selectedText;
  String actionText;
  
  private:
  boolean selected;
  menu_action_t action_ptr;
  void *action_args;
};

class Menu
{
  public:
  Menu(int numitems, MenuItem **_items, LiquidCrystal *_lcd);
  void next(void);
  void prev(void);
  void enter(void);
  int action(void);

  void display(void);

  boolean selected; // Is the current item selected?

  private:
  MenuItem *currentItem(void);
  MenuItem *nextItem(void);

  MenuItem **items;
  char numItems;
  byte current;
  LiquidCrystal *lcd;
};




#endif
