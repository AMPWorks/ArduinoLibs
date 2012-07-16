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

#define IGNITION_TIMEOUT_MS 5000
#define SOLENOID_TIMEOUT_MS 500

class Poofer
{
  public:
  int sol_switch;
  int sol_led;
  int sol_relay;
  int ign_switch;
  int ign_led;
  int ign_relay;

  int sol_value;
  unsigned long sol_turned_on_ms;
  
  int ign_value;
  unsigned long ign_turned_on_ms;

  public:
  Poofer(int _sol_switch,
         int _sol_led,
         int _sol_relay,
         int _ign_switch,
         int _ign_led,
         int _ign_relay) {
    sol_switch = _sol_switch;
    sol_led = _sol_led;
    sol_relay = _sol_relay;
    ign_switch = _ign_switch;
    ign_led = _ign_led;
    ign_relay = _ign_relay;

    ign_value = 0;
    ign_turned_on_ms = 0;

    sol_value = 0;
    sol_turned_on_ms = 0;

    /* Set the output pins */
    pinMode(sol_led, OUTPUT);
    pinMode(sol_relay, OUTPUT);
    pinMode(ign_led, OUTPUT);
    pinMode(ign_relay, OUTPUT);

    /* Initialize the outputs to LOW */
    digitalWrite(sol_led, LOW);
    digitalWrite(sol_relay, LOW);
    digitalWrite(ign_led, LOW);
    digitalWrite(ign_relay, LOW);
  };

};


class Igniter : public Switch 
{
  Igniter(byte _pin, boolean _analog, pin_action_t _action);

  void igniter_action(int pin, int value, void *arg);
  
  private:
  pin_action_t action;
  unsigned int disable_timout_ms;
};

#endif
