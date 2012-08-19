/*
 * Shift registers - 74HC595
 */
//#define DEBUG
//#define DEBUG_VERBOSE 2

#include <Arduino.h>

#include "Shift.h"
#include "Debug.h"

Shift::Shift(byte clock, byte latch, byte data, byte registers) {
  _clock_pin = clock;
  _latch_pin = latch;
  _data_pin = data;
  _registers = registers;

  pinMode(_clock_pin, OUTPUT);
  pinMode(_latch_pin, OUTPUT);
  pinMode(_data_pin, OUTPUT);
  
  digitalWrite(_clock_pin, LOW);
  digitalWrite(_latch_pin, LOW);
  digitalWrite(_data_pin, LOW);

  _latch_value = LOW;

  for (byte i = 0; i < sizeof (_data); i++) {
    _data[i] = 0;
  }
}

/* Trigger the latch pin pin on the shift registers */
void Shift::sendLatch(void) {
  _latch_value = !_latch_value;
  digitalWrite(_latch_pin, _latch_value);
}

void Shift::SetBit(byte bit, boolean on) 
{
  uint32_t *data = (uint32_t *)&_data;

  if (on) {
    *data |= 1 << bit;
  } else {
    *data &= ~(1 << bit);
  }
}

void Shift::Write(void) 
{
  int i;
  for (i = 0; i < _registers; i++) {
    int value = _data[_registers - i - 1];
    DEBUG_PRINT_HEX(2, _data[i]);
    shiftOut(_data_pin, _clock_pin, MSBFIRST, value);
  }
  DEBUG_PRINT(2, "\n");
  for (i = 0; i < _registers; i++) {
    sendLatch();
  }
}
