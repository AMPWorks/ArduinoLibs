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

  DEBUG4_PRINTLN("This is a high level print statement");

  int value1 = 10;
  int value2 = 20;
  DEBUG3_VALUELN("This is a value:", value1);

  DEBUG2_VALUE("This is two values on one line: ", value1);
  DEBUG2_VALUELN(",", value2);

  DEBUG_ERR("This is an error, the program will halt on the next statement");
  DEBUG_ERR_STATE(1);
}
