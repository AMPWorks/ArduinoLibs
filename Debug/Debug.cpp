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

  DEBUG1_PRINT("BRKVAL: ");
  DEBUG1_PRINT((int)__brkval);
  DEBUG1_PRINT(" HEAP_START: ");
  DEBUG1_PRINT(__heap_start);
  DEBUG1_PRINT(" &HEAP_START: ");
  DEBUG1_PRINT((int)&__heap_start);
  DEBUG1_PRINT(" &V: ");
  DEBUG1_PRINT((int)&v);
  DEBUG1_PRINT(" FREE: ");
  DEBUG1_PRINTLN(v);
}
