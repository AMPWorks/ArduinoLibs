/*
 * Pin interface
 */
#include <Arduino.h>

#include "Pins.h"
#include "Debug.h"

/******************************************************************************
 * Generic pin interface
 *****************************************************************************/
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

/******************************************************************************
 * Sensor interface
 *****************************************************************************/
void
Sensor::init(boolean _pull_up, pin_action_t _action, void *_action_arg)
{
  pull_up = _pull_up;
  action = _action;
  action_arg = _action_arg;

  /* Init to defaults */
  curr_state = LOW;
  prev_state = LOW;
  debounce_time = 0;
  debounce_delay = DEFAULT_DEBOUNCE_DELAY;

  if (pull_up) {
     pinMode(pin, INPUT_PULLUP);
  } else {
    pinMode(pin, INPUT);
  }
}

Sensor::Sensor(byte _pin, boolean _pull_up, boolean _analog,
               pin_action_t _action) 
    : Pin(_pin, _analog, PIN_TYPE_SENSOR)
{
  init(_pull_up, _action, NULL);
}
 
Sensor::Sensor(byte _pin, boolean _pull_up, boolean _analog,
               pin_action_t _action, void *_action_arg)
    : Pin(_pin, _analog, PIN_TYPE_SENSOR)
{
  init(_pull_up, _action, _action_arg);
}

/*
 * Read the raw value of the sensor and perform an action if one is assigned
 */
int
Sensor::read(void) 
{
  prev_state = curr_state;
  if (analog) {
    curr_state = analogRead(pin);
  } else {
    curr_state = digitalRead(pin);
    if (pull_up) {
      /* Pins set to INPUT_PULLUP will register HIGH when off */
      if (curr_state == HIGH) curr_state = LOW;
      else curr_state = HIGH;
    }
  }

  DEBUG_PRINT(2, "read- pin ");
  DEBUG_PRINT(2, pin);
  DEBUG_PRINT(2, ": value=");
  DEBUG_PRINT(2, curr_state);
  DEBUG_PRINT(2, "\n");

  if (action) action(pin, curr_state, action_arg);

  return (curr_state);
}

/*
 * Read the value of the sensor with debouncing and perform an action if
 * one is assigned.
 */
int
Sensor::debouncedRead(void)
{
  byte currentValue;
  if (analog) {
    currentValue = analogRead(pin);
  } else {
    currentValue = digitalRead(pin);
    if (pull_up) {
      /* Pins set to INPUT_PULLUP will register HIGH when off */
      if (currentValue == HIGH) currentValue = LOW;
      else currentValue = HIGH;
    }
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

/* Check the state of every sensor in the array */
boolean
checkSensors(Pin **pins, byte num_pins, boolean debounce) {
  boolean retval = false;

  for (byte i = 0; i < num_pins; i++) {
    Pin *pin = pins[i];
    if (pin->type == PIN_TYPE_SENSOR) {
      if (debounce && !(pin->analog)) {
        if (((Sensor *)pin)->debouncedRead()) {
          retval = true;
        }
      } else {
        if (((Sensor *)pin)->read()) {
          ;
        }
      }
    }
  }
  return retval;
}
