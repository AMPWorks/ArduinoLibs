#include <Arduino.h>

#include "Debug.h"

char close_line = 0;

/*
 * Enter a persistent error state, flashing the indicated error code on the
 * built-in LED (pin 13);
 */
void debug_err_state(int code) {
  pinMode(13, OUTPUT);

  while (true) {
    /* Flash the error code */
    digitalWrite(13, LOW);
    for (int i = 0; i < code; i++) {
      delay(250);
      digitalWrite(13, HIGH);
      delay (250);
      digitalWrite(13, LOW);
    }

    /* Wait in between flashing the error code */
    delay(2000);
  }
}
