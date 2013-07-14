#include <Arduino.h>

#include "GeneralUtils.h"

/*
 * Flash the indicated value on the pin
 *   - Non blocking
 *
 * XXX: Can only execute against a single pin
 */
void blink_value(int pin, int value, int period_ms, int idle_periods) 
{
  static long period_start_ms = 0;
  static long last_step_ms = 0;
  static boolean led_value = true;

  long now = millis();
  if (period_start_ms == 0) period_start_ms = now;

  if ((now - last_step_ms) > period_ms) {
    int current_step = (now - period_start_ms) / period_ms;

    if (current_step >= (value * 2 + idle_periods)) {
      // The end of the current display cycle
      period_start_ms = now;
    } else if (current_step >= value * 2) {
      return;
    }

    led_value = !led_value;
    digitalWrite(pin, led_value);

    last_step_ms = now;
  }
}

/*
 * Check if a pin is PWM
 *
 * XXX: This is hardcoded for the ArduinoNano (and similar boards)
 */
boolean pin_is_PWM(int pin)
{
  switch (pin) {
      case 3:
      case 5:
      case 6:
      case 9:
      case 10:
      case 11:
        return true;
      default:
        return false;
  }
}
