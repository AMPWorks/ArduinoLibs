/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2013
 * 
 * This is a minimal example using the MPR121 sensor library.
 *
 * This sketch will flash an LED whenever a sensor changes state and print
 * the new state of the sensor.
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
#include <Wire.h>

#include "Debug.h"
#include "MPR121.h"

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
  touch.setThreshold(0, 5, 10);
  touch.setThreshold(1, 5, 10);
  touch.setThreshold(2, 5, 10);
  touch.setThreshold(3, 5, 10);
  touch.setThreshold(4, 5, 10);
  touch.setThreshold(5, 5, 10);
  touch.setThreshold(6, 5, 10);
  touch.setThreshold(7, 5, 10);
  touch.setThreshold(8, 5, 10);
  touch.setThreshold(9, 5, 10);
  touch.setThreshold(10, 5, 10);
  touch.setThreshold(11, 5, 10);

  Serial.println("MPR121 sensons initialized");
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
	  Serial.println(" Released");
	}
      }
    }

    // Flash the on-board LED when there is a change of state
    debug_on = !debug_on;
    if (debug_on) digitalWrite(DEBUG_LED, HIGH);
    else digitalWrite(DEBUG_LED, HIGH);
  }
}
