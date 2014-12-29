/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2013-2014
 * 
 * This is a trivial example using multiple MPR121 sensors
 */

//#define DEBUG_LEVEL DEBUG_HIGH
#include <Debug.h>

#include <Arduino.h>
#include "MPR121.h"
#include <Wire.h>

#define DEBUG_LED 13


#define NUM_SENSORS 2

byte irq_pins[NUM_SENSORS] = {
  3,
  5
};

byte addresses[NUM_SENSORS] = {
  0x5A, // ADD=GND (default)
  0x5B, // ADD=VCC
  //  0x5C, // ADD=SDA
  //  0x5D, // ADD=SCL
};

MPR121 sensors[NUM_SENSORS];

void setup() {
  Serial.begin(9600);
  Serial.println("MPR121 Multiple initializing");

  Wire.begin();

  for (byte sensor = 0; sensor < NUM_SENSORS; sensor++) {
    sensors[sensor] = MPR121(
                             irq_pins[sensor],  // triggered/interupt pin
                             false,             // interrupt mode?
                             addresses[sensor], // START_ADDRESS = 0x5A
                             true,              // use touch times
                             false              // use auto config and reconfig
                             );
    /*
     * Set default touch and release thresholds
     */
    for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
      sensors[sensor].setThreshold(i, 15, 2);
    }  
  }

  pinMode(DEBUG_LED, OUTPUT);

  Serial.println("MPR121 sensors initialized");
}

boolean debug_on = false;

void loop() {
  boolean changed = false;

  for (byte sensor = 0; sensor < NUM_SENSORS; sensor++) {

    if (sensors[sensor].readTouchInputs()) {
      changed = true;

      for (int i = 0; i < MPR121::MAX_SENSORS; i++) {
        if (sensors[sensor].changed(i)) {

          Serial.print(sensor);
          Serial.print(" - pin ");
          Serial.print(i);
          Serial.print(":");
	
          if (sensors[sensor].touched(i)) {
            Serial.println(" Sensed");
          } else {
            Serial.print(" Released after ");
            Serial.print(sensors[sensor].touchTime(i));
            Serial.println(" ms ");
          }
        }
      }
    }
  }

  if (changed) {
    // Flash the on-board LED when there is a change of state
    debug_on = !debug_on;
    if (debug_on) digitalWrite(DEBUG_LED, HIGH);
    else digitalWrite(DEBUG_LED, HIGH);
  }
}
