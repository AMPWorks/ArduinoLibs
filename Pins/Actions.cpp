#include "Debug.h"
#include "Pins.h"

/* Actions */

/* Set an output's next value to the value of the sensor */
void action_set_output(int pin, int value, void *arg) 
{
  Output *out = (Output *)arg;

  out->setValue(value);
  DEBUG_PRINT(2, "set_output: pin-");
  DEBUG_PRINT(2, pin);
  DEBUG_PRINT(2, " value-");
  DEBUG_PRINT(2, value);
  DEBUG_PRINT(2, " outpin-");
  DEBUG_PRINT(2, out->_value);
  DEBUG_PRINT(2, "\n");
}


void action_print_value(int pin, int value, void *arg) 
{
  DEBUG_PRINT(3, "Pin ");
  DEBUG_PRINT(3, pin);
  DEBUG_PRINT(3, " value: ");
  DEBUG_PRINT(3, value);
  DEBUG_PRINT(3, "\n");
}
