/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This trivially demonstrates how to use the Debugging print macros
 */

// The debugging level must be specified in every file using debug statements,
// without it no debug statements will be compiled
#define DEBUG_LEVEL DEBUG_MID
#include <Debug.h>

void setup() {
  Serial.begin(9600);
}

void loop() {

  DEBUG_PRINTLN(DEBUG_HIGH, "This is a high level print statement");

  int value1 = 10;
  int value2 = 20;
  DEBUG_VALUELN(DEBUG_MID, "This is a value:", value1);

  DEBUG_VALUE(DEBUG_LOW, "This is two values on one line: ", value1);
  DEBUG_VALUELN(DEBUG_LOW, ",", value2);

  DEBUG_ERR("This is an error, the program will halt on the next statement");
  DEBUG_ERR_STATE(1);
}
