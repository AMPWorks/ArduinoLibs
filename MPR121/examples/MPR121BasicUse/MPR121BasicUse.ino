/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2013
 * 
 * This is a minimal example using the MPR121 sensor library.
 *
 * This sketch will flash an LED whenever a sensor changes state and print
 * the new state of the sensor.  It also provides a minimal CLI for tweaking
 * and reading configuration values for the MPR121.
 *
 * By default, the following pins on the MPR121 should be attached to these
 * Arduino pins
 *   SCL  -> Analog 5 (I2C Clock)
 *   SDA  -> Analog 4 (I2C Data)
 *   IRQ  -> Digital 2
 *   GND  -> GND
 *   3.3V -> 3.3V
 */

#include <Arduino.h>
#include "MPR121.h"
#include <Wire.h>

#define DEBUG_LEVEL DEBUG_HIGH
#include <Debug.h>

#define IRQ_PIN 2

#define DEBUG_LED 13

MPR121 touch;

void setup() {
  Serial.begin(9600);
  Wire.begin();

  touch = MPR121(
		 IRQ_PIN,       // triggered/interupt pin
		 false          // interrupt mode?
		 );

  pinMode(DEBUG_LED, OUTPUT);

  /*
   * Set touch and release thresholds
   */
  //  touch.setThreshold(0, 12, 1);
  //  touch.setThreshold(1, 12, 15);
  //  touch.setThreshold(2, 5, 10);
  //  touch.setThreshold(3, 5, 10);
  //  touch.setThreshold(4, 5, 10);
  //  touch.setThreshold(5, 5, 10);
  //  touch.setThreshold(6, 5, 10);
  //  touch.setThreshold(7, 5, 10);
  //  touch.setThreshold(8, 5, 10);
  //  touch.setThreshold(9, 6, 2);
  //  touch.setThreshold(10, 5, 10);
  //  touch.setThreshold(11, 5, 10);

  Serial.println("MPR121 sensors initialized");
}

boolean debug_on = false;

void loop() {
  if (touch.readTouchInputs()) {
    for (int i = 0; i < MPR121::MAX_SENSORS; i++) {
      if (touch.changed(i)) {

	Serial.print("pin ");
	Serial.print(i);
	Serial.print(":");
	
	if (touch.touched(i)) {
	  Serial.println(" Sensed");
	} else {
	  Serial.print(" Released after ");
	  Serial.print(touch.touchTime(i));
	  Serial.println(" ms ");
	}
      }
    }

    // Flash the on-board LED when there is a change of state
    debug_on = !debug_on;
    if (debug_on) digitalWrite(DEBUG_LED, HIGH);
    else digitalWrite(DEBUG_LED, HIGH);
  }

  checkSerial();
}


/* Read data from the serial port */
#define MAX_CLI_LEN 128
void checkSerial() {
  static char input[MAX_CLI_LEN];
  static int i = 0;

  while (Serial.available()) {
    char c = (char)Serial.read();

    if (c == '\n') {
      input[i] = 0;
      handleSerial(input);
      i = 0;
    } else {
      input[i] = c;
      i++;
    }

    if (i >= MAX_CLI_LEN - 2) {
      // OVERFLOW
      i = 0;
    }
  }
}

/*
 * Available serial commands:
 * 
 * - Set touch thresholds:
 *   't <sensor> <trigger> <release>' - Set the touch thresholds for a sensor
 *   'd <trigger> <release>' - Set the debounce values for a sensor
 *   'r <register>' - Read the indicated register's value
 *   's <register> <value>' - Set the value of a register
 *
 *   example: 't 0 10 2'
 */
#define MAX_TOKENS 8
void handleSerial(char *command) {
  char *tokens[MAX_TOKENS];
  for (int i = 0; i < MAX_TOKENS; i++) {
    tokens[i] = NULL;
  }

  char *value_ptr = command;
  int token = 0;
  tokens[token] = value_ptr;
  while (*value_ptr != 0) {
    if (*value_ptr == ' ') {
      token++;
      if (token > MAX_TOKENS) {
	break;
      }

      value_ptr++;
      tokens[token] = value_ptr;
      continue;
    }
    value_ptr++;
  }

  token = 4;
  switch (tokens[0][0]) {
  case 't': {
    int sensor = atoi(tokens[1]);
    int trigger = atoi(tokens[2]);
    int release = atoi(tokens[3]);
    touch.setThreshold(sensor, trigger, release);
    break;
  }
  case 'd': {
    int trigger = atoi(tokens[1]);
    int release = atoi(tokens[2]);
    touch.setDebounce(trigger, release);
    break;
  }

  case 'r': {
    uint8_t reg = strtol(tokens[1], NULL, 16);
    byte value;
    if (touch.read_register(reg, &value)) {
      Serial.print("Read register:0x");
      Serial.print(reg, HEX);
      Serial.print(" value:0x");
      Serial.println(value, HEX);
    } else {
      Serial.print("Unable to read register:0x");
      Serial.println(reg, HEX);
    }
    break;
  }
  case 's': {
    uint8_t reg = strtol(tokens[1], NULL, 0);
    uint8_t value = strtol(tokens[2], NULL, 0);
    touch.set_register(ELE_CFG, 0x00);
    touch.set_register(reg, value);
    touch.set_register(ELE_CFG, 0x0C);
    Serial.print("Set register:0x");
    Serial.print(reg, HEX);
    Serial.print(" value:0x");
    Serial.println(value, HEX);
    break;
  }
  }
}
