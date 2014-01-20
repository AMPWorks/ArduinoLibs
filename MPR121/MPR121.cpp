#include <Arduino.h>
#include <Wire.h>

#define DEBUG_LEVEL DEBUG_HIGH
#include <Debug.h>

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

MPR121::MPR121() {
  triggered = false;
  useInterrupt = false;
  irqpin = false;
}

/*
 * IMPORTANT NODE: Wire.begin() must be called before MPR121 initialization
 */
MPR121::MPR121(byte _irqpin, boolean _useInterrupt, byte _address) {
  init(_irqpin, _useInterrupt, _address);
}

MPR121::MPR121(byte _irqpin, boolean _useInterrupt) {
  init(_irqpin, _useInterrupt, START_ADDRESS);
}

void MPR121::init(byte _irqpin, boolean _useInterrupt, byte _address) {
  triggered = true;

  irqpin = _irqpin;
  useInterrupt = _useInterrupt;
  address = _address;

  for (int i = 0; i < MAX_SENSORS; i++) {
    touchStates[i] = false;
    prevStates[i] = false;
    touchTimes[i] = 0;
  }

  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH); //enable pullup resistor XXX: Is this needed with interrupts?

  if (useInterrupt) {
    /* Register the interrupt handler */
    switch (irqpin) {
      //XXX - Check if the nano shows up at all for the board type?
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
      DEBUG_ERR(F("Specified pin is not an interrupt"));
      DEBUG_ERR_STATE(13);
      useInterrupt = false;
      break;
    }
  }

  DEBUG_VALUE(DEBUG_MID, F("MPR121: Initialized.  IRQ="), irqpin);
  DEBUG_VALUELN(DEBUG_MID, F(" useInterrupt="), useInterrupt);

  initialize();
}

void MPR121::initialize(void) {

  set_register(ELE_CFG, 0x00);
  
  // Section A - Controls filtering when data is > baseline.
  set_register(MHD_R, 0x01);
  set_register(NHD_R, 0x01);
  set_register(NCL_R, 0x00);
  set_register(FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(MHD_F, 0x01);
  set_register(NHD_F, 0x01);
  set_register(NCL_F, 0xFF);
  set_register(FDL_F, 0x02);
  
  // Section C - Sets default touch and release thresholds for each electrode
  set_register(ELE0_T, TOU_THRESH);
  set_register(ELE0_R, REL_THRESH);
 
  set_register(ELE1_T, TOU_THRESH);
  set_register(ELE1_R, REL_THRESH);
  
  set_register(ELE2_T, TOU_THRESH);
  set_register(ELE2_R, REL_THRESH);
  
  set_register(ELE3_T, TOU_THRESH);
  set_register(ELE3_R, REL_THRESH);
  
  set_register(ELE4_T, TOU_THRESH);
  set_register(ELE4_R, REL_THRESH);
  
  set_register(ELE5_T, TOU_THRESH);
  set_register(ELE5_R, REL_THRESH);
  
  set_register(ELE6_T, TOU_THRESH);
  set_register(ELE6_R, REL_THRESH);
  
  set_register(ELE7_T, TOU_THRESH);
  set_register(ELE7_R, REL_THRESH);
  
  set_register(ELE8_T, TOU_THRESH);
  set_register(ELE8_R, REL_THRESH);
  
  set_register(ELE9_T, TOU_THRESH);
  set_register(ELE9_R, REL_THRESH);
  
  set_register(ELE10_T, TOU_THRESH);
  set_register(ELE10_R, REL_THRESH);
  
  set_register(ELE11_T, TOU_THRESH);
  set_register(ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*
    set_register(ATO_CFG0, 0x0B);
    set_register(ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
    set_register(ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  */
  
  set_register(ELE_CFG, 0x0C);
}

void MPR121::setThreshold(byte sensor, byte trigger, byte release) {
  byte trig;
  byte rel;
  switch (sensor) {
  case 0: trig = ELE0_T; rel = ELE0_R; break;
  case 1: trig = ELE1_T; rel = ELE1_R; break;
  case 2: trig = ELE2_T; rel = ELE2_R; break;
  case 3: trig = ELE3_T; rel = ELE3_R; break;
  case 4: trig = ELE4_T; rel = ELE4_R; break;
  case 5: trig = ELE5_T; rel = ELE5_R; break;
  case 6: trig = ELE6_T; rel = ELE6_R; break;
  case 7: trig = ELE7_T; rel = ELE7_R; break;
  case 8: trig = ELE8_T; rel = ELE8_R; break;
  case 9: trig = ELE9_T; rel = ELE9_R; break;
  case 10: trig = ELE10_T; rel = ELE10_R; break;
  case 11: trig = ELE11_T; rel = ELE11_R; break;
  default: {
    DEBUG_ERR(F("Specified sensor does not exist"));
    DEBUG_ERR_STATE(14);
    return;
  }
  }

  set_register(ELE_CFG, 0x00);
  set_register(trig, trigger);
  set_register(rel, release);
  set_register(ELE_CFG, 0x0C);

  DEBUG_VALUE(DEBUG_MID, "Set threshold- Sensor:", sensor);
  DEBUG_VALUE(DEBUG_MID, " trigger:", trigger);
  DEBUG_VALUELN(DEBUG_MID, " release:", release);
}

/* Set the debounce values, range for each is 0-7 */
void MPR121::setDebounce(byte trigger, byte release) {
  byte value = (((release & 0x7) << 4) | (trigger & 0x7));

  set_register(ELE_CFG, 0x00);
  set_register(DEBOUNCE, value);
  set_register(ELE_CFG, 0x0C);

  DEBUG_VALUE(DEBUG_MID, "Set debouce- trigger:", trigger);
  DEBUG_VALUELN(DEBUG_MID, " release:", release);
}

/* Return the value of the sensor from the most recent check */
boolean MPR121::touched(byte sensor) {
  return touchStates[sensor];
}

/* Return the value of the sensor from the previous check */
boolean MPR121::previous(byte sensor) {
  return prevStates[sensor];
}

/* Check if the sensor value changed from the previous check */
boolean MPR121::changed(byte sensor) {
  return (touchStates[sensor] != prevStates[sensor]);
}

/*
 *  If currently touched, return the time touched so far, otherwise return
 * the total time from the last touch period.
 */
unsigned long MPR121::touchTime(byte sensor) {
  if (touchStates[sensor]) {
    return millis() - touchTimes[sensor];
  } else {
    return touchTimes[sensor];
  }
}

/*
 * Check if the IRQ pin has been triggered and if so query the chip for the
 * current state of the sensors.
 */
boolean MPR121::readTouchInputs() {
  if (!useInterrupt) {
    checkInterrupt();
  }

  /*
   *  The prevStates are used to determine if values have changed since the
   * previous call and so should be updated regardless of whether IRQ was
   * triggered.
   */
  for (int i = 0; i < MAX_SENSORS; i++) {
    prevStates[i] = touchStates[i];
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
	DEBUG_COMMAND(DEBUG_TRACE,
		      if (touchStates[i] == 0) {
			//pin i was just touched
			DEBUG_VALUELN(DEBUG_TRACE, F("Touched pin "), i);
		      }
		      );
	touchStates[i] = true;

	if (!prevStates[i]) {
	  /* Changed from not-touched to touch, record the time */
	  touchTimes[i] = millis();
	}

      } else {
	DEBUG_COMMAND(DEBUG_TRACE,
		      if (touchStates[i] == 1) {
			//pin i is no longer being touched
			DEBUG_VALUELN(DEBUG_TRACE, F("Released pin "), i);
		      }
		      );
        touchStates[i] = false;

	if (prevStates[i]) {
	  /* Changed from touched to not-touch, record the total touched time */
	  touchTimes[i] = millis() - touchTimes[i];
	}
      }
    }

    // Change of state
    return true;
  } else {
    // No change of state
    return false;
  }
}

void MPR121::checkInterrupt(void) {
  if (!digitalRead(irqpin)) { // IRQ is triggered on a down pulse
    triggered = true;
  }
}

void MPR121::set_register(uint8_t r, uint8_t v) {
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}

boolean MPR121::read_register(uint8_t r, byte* result) {
  Wire.beginTransmission(address);
  Wire.write(r);
  Wire.endTransmission(false);

  Wire.requestFrom(address,  (uint8_t)1, (uint8_t)true);

  if (Wire.available() >= 1) {
    *result = Wire.read() ;
    return true;
  } else {
    return false;
  }
}
