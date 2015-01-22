/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2013-2014
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
 *   IRQ  -> Digital 3
 *   GND  -> GND
 *   3.3V -> 3.3V
 */

#include <Arduino.h>
#include "MPR121.h"
#include <Wire.h>

#define I2C_ADDRESS 0x5A // 0x5A - 0x5D
#define IRQ_PIN 3

#define DEBUG_LED 13

MPR121 touch;

void setup() {
  Serial.begin(9600);
  Serial.println(F("MPR121 Basic Use initializing"));

  Wire.begin();

  touch = MPR121(
                 IRQ_PIN,       // triggered/interupt pin
                 false,         // interrupt mode?
                 I2C_ADDRESS,   // START_ADDRESS = 0x5A
                 true,          // use touch times
                 false          // use auto config and reconfig
                 );

  pinMode(DEBUG_LED, OUTPUT);

  /*
   * Set default touch and release thresholds
   */
  for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
    touch.setThreshold(i, 15, 2);
  }

  Serial.println(F("MPR121 sensor initialized"));
}

boolean debug_on = false;

void loop() {
  if (touch.readTouchInputs()) {
    for (int i = 0; i < MPR121::MAX_SENSORS; i++) {
      if (touch.changed(i)) {

        Serial.print(F("pin "));
        Serial.print(i);
        Serial.print(F(":"));
	
        if (touch.touched(i)) {
          Serial.println(F(" Sensed"));
        } else {
          Serial.print(F(" Released after "));
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
      Serial.print(F("Setting threshold sensor="));
      Serial.print(sensor);
      Serial.print(F(" trig="));
      Serial.print(trigger);
      Serial.print(F(" rel="));
      Serial.println(release);
      touch.setThreshold(sensor, trigger, release);
      break;
    }
    case 'd': {
      int trigger = atoi(tokens[1]);
      int release = atoi(tokens[2]);
      Serial.print(F("Setting debounce trig="));
      Serial.print(trigger);
      Serial.print(F(" rel="));
      Serial.println(release);
      touch.setDebounce(trigger, release);
      break;
    }

    case 'r': {
      uint8_t reg = strtol(tokens[1], NULL, 16);
      byte value;
      if (touch.read_register(reg, &value)) {
        Serial.print(F("Read register:0x"));
        Serial.print(reg, HEX);
        Serial.print(F(" value:0x"));
        Serial.println(value, HEX);
      } else {
        Serial.print(F("Unable to read register:0x"));
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
      Serial.print(F("Set register:0x"));
      Serial.print(reg, HEX);
      Serial.print(F(" value:0x"));
      Serial.println(value, HEX);
      break;
    }
  }
}
