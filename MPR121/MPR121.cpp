#define DEBUG_LEVEL DEBUG_HIGH
#include <Debug.h>

#include <Arduino.h>
#include <Wire.h>

#include "MPR121.h"

/*
 * Due to the definition of attachInterrupt a class method cannot be specified
 * as the interrupt function.  To enable multiple MPR121's to be used multiple
 * handlers are defined.
 */
MPR121 *cap[2] = {NULL, NULL};
void irqTriggered0() {
  if (cap[0] != NULL) cap[0]->triggered = true;
}
void irqTriggered1() {
  if (cap[1] != NULL) cap[1]->triggered = true;
}

/* Initializer */
MPR121::MPR121(byte _irqpin, boolean *_touchStates, boolean _useInterrupt) {
  triggered = true;

  irqpin = _irqpin;
  useInterrupt = _useInterrupt;
  touchStates = _touchStates;

  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor XXX: Is this needed with interrupts?

  if (useInterrupt) {
    /* Register the interrupt handler */
    switch (irqpin) {
    case INTERUPT_0_PIN: {
      cap[0] = this;
      attachInterrupt(0, irqTriggered0, RISING);
      break;
    }
    case INTERUPT_1_PIN: {
      cap[1] = this;
      attachInterrupt(1, irqTriggered1, RISING);
      break;
    }
    default:
      /*
       * If the pin specified is not a known interupt pin then allow this to
       * work anyway.
       */
      DEBUG_ERR("Specified pin is not an interrupt");
      DEBUG_ERR_STATE(13);
      useInterrupt = false;
      break;
    }
  }

  setup();
}

void MPR121::setup(void) {

  set_register(0x5A, ELE_CFG, 0x00);
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, 10); //TOU_THRESH);
  set_register(0x5A, ELE0_R, 10); //REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, 10); //TOU_THRESH);
  set_register(0x5A, ELE2_R, 20); //REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*
    set_register(0x5A, ATO_CFG0, 0x0B);
    set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
    set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(0x5A, ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  */
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}

void MPR121::readTouchInputs() {
  if (!useInterrupt) {
    checkInterrupt();
  }

  if (triggered) {
    triggered = false;

    // read the touch state from the MPR121
    Wire.requestFrom(0x5A, 2);

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    // 16bits that make up the touch states
    uint16_t touched = ((MSB << 8) | LSB);

    for  (int i = 0; i < 12; i++) {  // Check what electrodes were pressed
      if (touched & (1 << i)){
	DEBUG_COMMAND(DEBUG_HIGH,
		      if (touchStates[i] == 0) {
			//pin i was just touched
			DEBUG_VALUE(DEBUG_MID, "Touched pin ", i);
		      }
		      );
	touchStates[i] = 1;
      } else {
	DEBUG_COMMAND(DEBUG_HIGH,
		      if (touchStates[i] == 1) {
			//pin i is no longer being touched
			DEBUG_VALUE(DEBUG_MID, "Released pin ", i);
		      }
		      );
        touchStates[i] = 0;
      }
    }
  }
}

void MPR121::checkInterrupt(void) {
  if (digitalRead(irqpin)) triggered = true;
}

void MPR121::set_register(int address, unsigned char r, unsigned char v) {
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
