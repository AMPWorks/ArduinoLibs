/*
 * Code to drive ShiftBar, ShiftBrite, etc
 */
#ifndef SHIFTBAR_H
#define SHIFTBAR_H

#include <Arduino.h>

/* SPI Pins */
#define SHIFTBAR_CLOCK_PIN  13 // CI
#define SHIFTBAR_ENABLE_PIN 10 // EI
#define SHIFTBAR_LATCH_PIN   9 // LI
#define SHIFTBAR_DATA_PIN   11 // DI

#define SHIFTBAR_MAX   1023
#define SHIFTBAR_RED   0
#define SHIFTBAR_GREEN 1
#define SHIFTBAR_BLUE  2

class ShiftBar 
{
  public:
  ShiftBar(uint8_t modules, uint16_t *values);
  void set(uint8_t module, uint16_t red, uint16_t green, uint16_t blue);
  void set(uint16_t red, uint16_t green, uint16_t blue);
  void update(void);

  private:
  uint8_t _modules;
  uint16_t *_values;

  void spiSend(int commandmode, int red, int blue, int green);
};


#endif
