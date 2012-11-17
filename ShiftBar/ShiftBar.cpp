#include "ShiftBar.h"

ShiftBar::ShiftBar(uint8_t modules, uint16_t *values) 
{
  num_modules = modules;
  _values = values;

  /* Initialize to all-off */
  for (int i = 0; i < (num_modules * 3); i++) {
    _values[i] = 0;
  }

  /* Setup the ShiftBrite pins */
  pinMode(SHIFTBAR_DATA_PIN, OUTPUT);
  pinMode(SHIFTBAR_LATCH_PIN, OUTPUT);
  pinMode(SHIFTBAR_ENABLE_PIN, OUTPUT);
  pinMode(SHIFTBAR_CLOCK_PIN, OUTPUT);

  SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);

  digitalWrite(SHIFTBAR_LATCH_PIN, LOW);
  digitalWrite(SHIFTBAR_ENABLE_PIN, LOW);
}

/* Set the values for one module */
void ShiftBar::set(uint8_t module,
                   uint16_t red, uint16_t green, uint16_t blue) 
{
  _values[module * 3 + 0] = red;
  _values[module * 3 + 1] = green;
  _values[module * 3 + 2] = blue;
}

/* Default for single module */
void ShiftBar::set(uint16_t red, uint16_t green, uint16_t blue) 
{
  set(0, red, green, blue);
}


void ShiftBar::spiSend(int commandmode, int red, int blue, int green) {
  if (commandmode == B01) {
    red = 120;
    green = 100;
    blue = 100;
  }
 
  SPDR = commandmode << 6 | blue >> 4;
  while(!(SPSR & (1 << SPIF)));
  SPDR = blue << 4 | red >> 6;
  while(!(SPSR & (1 << SPIF)));
  SPDR = red << 2  | green >> 8;
  while(!(SPSR & (1 << SPIF)));
  SPDR = green;
  while(!(SPSR & (1 << SPIF)));
}

void ShiftBar::update() {
 
  // Write to PWM control registers
  for (int h = 0; h < num_modules; h++) {
    spiSend(B00,
            _values[h + 0],
            _values[h + 1],
            _values[h + 2]);
  }
 
  delayMicroseconds(15);
  digitalWrite(SHIFTBAR_LATCH_PIN, HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(SHIFTBAR_LATCH_PIN, LOW);
 
  for (int z = 0; z < num_modules; z++) spiSend(B01, 0, 0, 0);

  delayMicroseconds(15);
  digitalWrite(SHIFTBAR_LATCH_PIN, HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(SHIFTBAR_LATCH_PIN, LOW);
}

