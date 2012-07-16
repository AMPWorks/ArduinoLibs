/*
 * Code for on-off controls
 */
#include <Arduino.h>

#include "Pins.h"
#include "Debug.h"

Pin::Pin(byte _pin, boolean _analog, pin_type_t _type)
{
  pin = _pin;
  type = _type;
  analog = _analog;
  if (!analog) {
    digitalWrite(pin, LOW);
  }
}

Pin::Pin(byte _pin, boolean _analog)
{
  Pin(_pin, PIN_TYPE_NONE);
}

void
Switch::init(byte _pin, pin_action_t _action)
{
  pinMode(pin, INPUT);
  action = _action;

  /* Init to defaults */
  curr_state = LOW;
  prev_state = LOW;
  debounce_time = 0;
  debounce_delay = DEFAULT_DEBOUNCE_DELAY;
}


Switch::Switch(byte _pin, boolean _analog, pin_action_t _action) 
    : Pin(_pin, _analog, PIN_TYPE_SWITCH)
{
  init(_pin, _action);
  action_arg = NULL;

}
 
Switch::Switch(byte _pin, boolean _analog, pin_action_t _action, void *_action_arg)
    : Pin(_pin, _analog, PIN_TYPE_SWITCH)
{
  init(_pin, _action);
  action_arg = _action_arg;
}


int
Switch::read(void) 
{
  if (analog) {
    curr_state = analogRead(pin);
  } else {
    curr_state = digitalRead(pin);
  }

  DEBUG_PRINT(2, "read- pin ");
  DEBUG_PRINT(2, pin);
  DEBUG_PRINT(2, ": value=");
  DEBUG_PRINT(2, curr_state);
  DEBUG_PRINT(2, "\n");

  if (action) action(pin, curr_state, action_arg);

  return (curr_state);
}


int
Switch::debouncedRead(void)
{
  byte currentValue;
  if (analog) {
    currentValue = analogRead(pin);
  } else {
    currentValue = digitalRead(pin);
  }
  
  if (currentValue != prev_state) {
    debounce_time = millis();
    DEBUG_PRINT(2, "debounce- pin ");
    DEBUG_PRINT(2, pin);
    DEBUG_PRINT(2, ": buttons_debounce_time=");
    DEBUG_PRINT(2, debounce_time);
    DEBUG_PRINT(2, "\n");
  }
  prev_state = currentValue;
  
  if (currentValue != curr_state) {
    if ((millis() - debounce_time) > debounce_delay) {
      curr_state = currentValue;
      DEBUG_PRINT(2, "debounce- pin ");
      DEBUG_PRINT(2, pin);
      DEBUG_PRINT(2, ": value=");
      DEBUG_PRINT(2, curr_state);
      DEBUG_PRINT(2, "\n");
    }
  }

  if (action) action(pin, currentValue, action_arg);
      
  return (curr_state == HIGH);
}

/* Check the state of every sensor */
boolean
checkSwitches(Pin **pins, byte num_pins, boolean debounce) {
  boolean retval = false;
  for (byte i = 0; i < num_pins; i++) {
    Pin *pin = pins[i];
    if (pin->type == PIN_TYPE_SWITCH) {
      if (debounce && !(pin->analog)) {
        if (((Switch *)pin)->debouncedRead()) {
          retval = true;
        }
      } else {
        if (((Switch *)pin)->read()) {
          ;
        }
      }
    }
  }
  return retval;
}
