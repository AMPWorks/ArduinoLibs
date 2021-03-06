/*
 * Pin interface
 */
//#define DEBUG
//#define DEBUG_VERBOSE 2

#include <Arduino.h>

#include "Debug.h"
#include "Pins.h"
#include "Shift.h"

/******************************************************************************
 * Generic pin interface
 *****************************************************************************/
Pin::Pin(byte _pin, boolean _analog, pin_type_t _type)
{
  pin = _pin;
  type = _type;
  analog = _analog;
}

Pin::Pin(byte _pin, boolean _analog)
{
  Pin(_pin, PIN_TYPE_NONE);
}

/******************************************************************************
 * Sensor interface
 *****************************************************************************/
void
Sensor::init(boolean _pull_up, boolean _reversed,
             pin_action_t _action, void *_action_arg)
{
  pull_up = _pull_up;
  action = _action;
  action_arg = _action_arg;
  reversed = _reversed;

  /* Init to defaults */
  curr_state = LOW;
  prev_state = LOW;
  debounce_time = 0;
  debounce_delay = DEFAULT_DEBOUNCE_DELAY;

  if (pull_up) {
    if (analog) {
      digitalWrite(pin, HIGH);
    } else {
      pinMode(pin, INPUT_PULLUP);
    }
  } else {
    pinMode(pin, INPUT);
  }
}

Sensor::Sensor(byte _pin, boolean _pull_up, boolean _analog,
               pin_action_t _action)
    : Pin(_pin, _analog, PIN_TYPE_SENSOR)
{
  init(_pull_up, _pull_up, _action, NULL);
}
 
Sensor::Sensor(byte _pin, boolean _pull_up, boolean _analog,
               pin_action_t _action, void *_action_arg)
    : Pin(_pin, _analog, PIN_TYPE_SENSOR)
{
  init(_pull_up, _pull_up, _action, _action_arg);
}

Sensor::Sensor(byte _pin, boolean _pull_up, boolean _analog,
               boolean _reversed, pin_action_t _action, void *_action_arg)
      : Pin(_pin, _analog, PIN_TYPE_SENSOR)
{
  init(_pull_up, _reversed, _action, _action_arg);
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
    if (reversed) {
      /* Pins set to INPUT_PULLUP will register HIGH when off */
      if (curr_state == HIGH) curr_state = LOW;
      else curr_state = HIGH;
    }
  }

  DEBUG3_PRINT("read- pin ");
  DEBUG3_PRINT(pin);
  DEBUG3_PRINT(": value=");
  DEBUG3_PRINT(curr_state);
  DEBUG3_PRINT("\n");

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
    if (reversed) {
      /* Pins set to INPUT_PULLUP will register HIGH when off */
      if (currentValue == HIGH) currentValue = LOW;
      else currentValue = HIGH;
    }
  }
  
  if (currentValue != prev_state) {
    debounce_time = millis();
    DEBUG3_PRINT("debounce- pin ");
    DEBUG3_PRINT(pin);
    DEBUG3_PRINT(": buttons_debounce_time=");
    DEBUG3_PRINT(debounce_time);
    DEBUG3_PRINT("\n");
  }
  prev_state = currentValue;
  
  if (currentValue != curr_state) {
    if ((millis() - debounce_time) > debounce_delay) {
      curr_state = currentValue;
      DEBUG3_PRINT("debounce- pin ");
      DEBUG3_PRINT(pin);
      DEBUG3_PRINT(": value=");
      DEBUG3_PRINT(curr_state);
      DEBUG3_PRINT("\n");
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
          retval = true;
        }
      }
    }
  }
  return retval;
}


/******************************************************************************
 * Output interface
 *****************************************************************************/

void Output::init(byte pin, byte value, Shift *shift, Sensor *sensor)
{
  _sensor = sensor;
  _value = value;
  _next_value = value;
  _shift = shift;
  if (_shift == NULL) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, _value);
  } else {
    _shift->SetBit(pin, (_value == HIGH ? true : false));
  }
}

Output::Output(byte pin, byte value, Shift *shift, Sensor *sensor)
    : Pin(pin, false, PIN_TYPE_OUTPUT)
{
  init(pin, value, shift, sensor);
}

Output::Output(byte pin, byte value, Shift *shift)
    : Pin(pin, false, PIN_TYPE_OUTPUT)
{
  init(pin, value, shift, NULL);
}

Output::Output(byte pin, byte value)
    : Pin(pin, false, PIN_TYPE_OUTPUT)
{
  init(pin, value, NULL, NULL);
}

/* Set the new value */
void
Output::setValue(byte value) 
{
  _next_value = value;
}

/* Apply the next value */
void
Output::trigger(void) 
{
  if (_sensor) _next_value = _sensor->curr_state;
  
  _value = _next_value;
  if (_shift == NULL) {
    digitalWrite(pin, _value);
  } else {
    _shift->SetBit(pin, (_value == HIGH ? true : false));
  }
}

void triggerOutputs(Pin **pins, byte num_pins) 
{
  for (byte i = 0; i < num_pins; i++) {
    Pin *pin = pins[i];
    if (pin->type == PIN_TYPE_OUTPUT) {
      ((Output *)pin)->trigger();
    }
  }  
}


