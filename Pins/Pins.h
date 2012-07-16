/*
 * Switch class
 *
 * Written by Adam Phelps (amp@cs.stanford.edu)
 */

#ifndef SWITCHES_H
#define SWITCHES_H

#include <Arduino.h>

#define DEFAULT_DEBOUNCE_DELAY 50

typedef enum 
{
  PIN_TYPE_NONE,
  PIN_TYPE_SWITCH
} pin_type_t;
  

class Pin 
{
  public:
  Pin(byte _pin, boolean _analog, pin_type_t _type);
  Pin(byte _pin, boolean _analog);

  pin_type_t type;
  boolean analog;

  protected:
  byte pin;
};

typedef void (*pin_action_t)(int pin, int value, void *arg);

class Switch : public Pin
{
  public:
  Switch(byte _pin, boolean _analog, pin_action_t _action);
  Switch(byte _pin, boolean _analog,
         pin_action_t _action, void *_action_arg);
  int read(void);
  int debouncedRead(void);

  pin_action_t action;
  void *action_arg;

  private:
  void init(byte _pin, pin_action_t _action);

  unsigned int curr_state;
  unsigned int prev_state;

  unsigned long debounce_time;
  unsigned long debounce_delay;
};

boolean checkSwitches(Pin **pins, byte num_pins, boolean debounce);

#endif
