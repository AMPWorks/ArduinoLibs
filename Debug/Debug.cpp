/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 */

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

/* Print the memory pointers and free space */
void debug_print_memory() {
  extern int __heap_start, *__brkval;
  int v;

  v = (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);

  DEBUG_PRINT(0, "BRKVAL: ");
  DEBUG_PRINT(0, (int)__brkval);
  DEBUG_PRINT(0, " HEAP_START: ");
  DEBUG_PRINT(0, __heap_start);
  DEBUG_PRINT(0, " &HEAP_START: ");
  DEBUG_PRINT(0, (int)&__heap_start);
  DEBUG_PRINT(0, " &V: ");
  DEBUG_PRINT(0, (int)&v);
  DEBUG_PRINT(0, " FREE: ");
  DEBUG_PRINTLN(0, v);
}
