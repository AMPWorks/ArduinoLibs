#ifndef GENERALUTILS_H
#define GENERALUTILS_H

/* Non-blocking flash a value on the indicated pin */
void blink_value(int pin, int value, int period_ms, int idle_periods) ;

/* Check if a pin is PWM */
boolean pin_is_PWM(int pin);

#endif
