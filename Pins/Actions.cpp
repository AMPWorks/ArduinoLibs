#include "Debug.h"
#include "Pins.h"

/* Actions */

/* Set an output's next value to the value of the sensor */
void action_set_output(int pin, int value, void *arg) 
{
  Output *out = (Output *)arg;

  out->setValue(value);
  DEBUG3_PRINT("set_output: pin-");
  DEBUG3_PRINT(pin);
  DEBUG3_PRINT(" value-");
  DEBUG3_PRINT(value);
  DEBUG3_PRINT(" outpin-");
  DEBUG3_PRINT(out->_value);
  DEBUG3_PRINT("\n");
}


void action_print_value(int pin, int value, void *arg) 
{
  DEBUG4_PRINT("Pin ");
  DEBUG4_PRINT(pin);
  DEBUG4_PRINT(" value: ");
  DEBUG4_PRINT(value);
  DEBUG4_PRINT("\n");
}
