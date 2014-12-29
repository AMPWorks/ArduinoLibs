/*
 * Library for intacting with MPR121 12-channel capacitive touch sensors
 *
 * This code was initially based on the MPR121 example code provided by 
 * Sparkfun: https://github.com/sparkfun/MPR121_Capacitive_Touch_Breakout/tree/master/Firmware/MPR121Q/Arduino%20Sketch
 *
 *
 * Original MPR121.h
 *   April 8, 2010
 *   by: Jim Lindblom
 *
 * Library conversion and continuing improvements
 *   October 31, 2013
 *   by: Adam Phelps
 */

#include <Arduino.h>
#include <Wire.h>

//#define DEBUG_LEVEL DEBUG_HIGH
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
  initialized = false;
  triggered = false;
  useInterrupt = false;
  irqpin = false;
  touchTimes = NULL;
}

/*
 * IMPORTANT NODE: Wire.begin() must be called before MPR121 initialization
 */
MPR121::MPR121(byte _irqpin, boolean _useInterrupt, byte _address,
	       boolean times) {
  initialized = false;
  init(_irqpin, _useInterrupt, _address, times, false);
}

MPR121::MPR121(byte _irqpin, boolean _useInterrupt, byte _address,
               boolean times, boolean auto_enabled) {
  initialized = false;
  init(_irqpin, _useInterrupt, _address, times, auto_enabled);
}

MPR121::MPR121(byte _irqpin, boolean _useInterrupt, boolean times) {
  initialized = false;
  init(_irqpin, _useInterrupt, START_ADDRESS, times, false);
}

void MPR121::init(byte _irqpin, boolean _useInterrupt, byte _address,
                  boolean times, boolean auto_enabled) {
  if (initialized) {
    DEBUG_ERR("MPR121::init already initialized");
    DEBUG_ERR_STATE(DEBUG_ERR_REINIT);
  } else {

    triggered = true;

    irqpin = _irqpin;
    useInterrupt = _useInterrupt;
    address = _address;

    touchStates = 0;
    prevStates = 0;

    if (times) {
      touchTimes = (unsigned long *)malloc(sizeof(unsigned long) * MAX_SENSORS);
      for (int i = 0; i < MAX_SENSORS; i++) {
        touchTimes[i] = 0;
      }
    } else {
      touchTimes = NULL;
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
        DEBUG_ERR("Specified pin is not an interrupt");
        DEBUG_ERR_STATE(13);
        useInterrupt = false;
        break;
      }
    }
  }
  initialized = true;

  DEBUG_VALUE(DEBUG_MID, "MPR121: Initialized.  IRQ=", irqpin);
  DEBUG_VALUELN(DEBUG_MID, " useInterrupt=", useInterrupt);

  initialize(auto_enabled);
}

void MPR121::initialize(boolean auto_enabled) {

  // Clear registers for the case where the chip is being configured again
  // without the power being reset
  set_register(0x80, 0x63);

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
  if (auto_enabled) {
    set_register(ATO_CFG0, 0x0B);
    set_register(ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
    set_register(ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  }
  
  set_register(ELE_CFG, 0x0C);
}

void MPR121::setThreshold(byte sensor, byte trigger, byte release) {
  byte trig = ELE0_T + sensor * 2;
  byte rel = ELE0_R + sensor * 2;

  set_register(ELE_CFG, 0x00); // ??? Disable all electrodes
  set_register(trig, trigger);
  set_register(rel, release);
  set_register(ELE_CFG, 0x0C); // ??? Enable all electrodes

  DEBUG_VALUE(DEBUG_MID, "Set threshold- Sensor:", sensor);
  DEBUG_VALUE(DEBUG_MID, " trigger:", trigger);
  DEBUG_VALUELN(DEBUG_MID, " release:", release);
}

void MPR121::setThresholds(byte trigger, byte release) {
  set_register(ELE_CFG, 0x00); // ??? Disable all electrodes
  for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
    set_register(ELE0_T + 2 * i, trigger);
    set_register(ELE1_T + 2 * i, release);
  }
  set_register(ELE_CFG, 0x0C); // ??? Enable all electrodes
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
  return ((touchStates & (1 << sensor)) != 0);
}

/* Return the value of the sensor from the previous check */
boolean MPR121::previous(byte sensor) {
  return ((prevStates & (1 << sensor)) != 0);
}

/* Check if the sensor value changed from the previous check */
boolean MPR121::changed(byte sensor) {
  return (touched(sensor) != previous(sensor));
}

/*
 *  If currently touched, return the time touched so far, otherwise return
 * the total time from the last touch period.
 */
unsigned long MPR121::touchTime(byte sensor) {
  if (touchTimes == NULL) return 0;
  if (touched(sensor)) {
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
  prevStates = touchStates;

  if (triggered) {
    triggered = false;

    // read the touch state from the MPR121
    Wire.requestFrom(address, (byte)2);

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    // 16bits that make up the touch states
    touchStates = ((MSB << 8) | LSB);

    DEBUG_COMMAND(DEBUG_TRACE,
                  // Check what electrodes were pressed
                  for  (int i = 0; i < MAX_SENSORS; i++) {
                    if (touched(i)){
                      if (!previous(i)) {
                        //pin i was just touched
                        DEBUG_VALUELN(DEBUG_TRACE, "Touched pin ", i);
                      }
                    } else {
                      if (touched(i)) {
                        //pin i is no longer being touched
                        DEBUG_VALUELN(DEBUG_TRACE, "Released pin ", i);
                      }
                    }
                  }
                  );

    if (touchTimes) {
      /* Update the touch times */
      unsigned long now = millis();

      for  (int i = 0; i < MAX_SENSORS; i++) {
        if (touched(i)) {
          if (!previous(i)) {
            /* Changed from not-touched to touch, record the time */
            touchTimes[i] = now;
          }

        } else {
          if (previous(i)) {
            /* Changed from touched to not-touch, record the total touched time */
            touchTimes[i] = now - touchTimes[i];
          }
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

uint16_t MPR121::getBaseline(byte sensor) {
  // TODO: Add calls to fetch baseline sensor values
  return 0;
}

uint16_t MPR121::getFiltered(byte sensor) {
  // TODO: Add calls to fetch filtered sensor values
  return 0;
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

/******************************************************************************
 * Class to track the state history of a sensor array
 */

MPR121_State::MPR121_State(MPR121 *_sensor, uint16_t _sensor_map) {
  sensor = _sensor;
  sensor_map = _sensor_map;
  for (int i = 0; i < MPR121::MAX_SENSORS; i++) {
    sensor_states[i] = 0;
    tap_times[i] = 0;
  }
}

void MPR121_State::updateState(void) {
  unsigned long now = millis();

  for (int i = 0; i < MPR121::MAX_SENSORS; i++) {
    if (sensor_map & (1 << i)) {
      uint8_t new_state = 0;

      if (sensor->touched(i)) new_state |= SENSE_TOUCH;
      if (sensor->changed(i)) new_state |= SENSE_CHANGE;
 
      if (checkTapped(i)) {
        // Detect double tap
        if (now - tap_times[i] < DOUBLE_TAP_MS) {
          new_state |= SENSE_DOUBLE;
          DEBUG_VALUE(DEBUG_HIGH, "Double tap:", i);
          DEBUG_VALUELN(DEBUG_HIGH, " ms:", now - tap_times[i]);
        }
        tap_times[i] = now;
      }

      if (checkHeld(i)) {
        // Detect long touch
        if ((tap_times[i] != 0) && (now - tap_times[i] > LONG_TOUCH_MS)) {
          new_state |= SENSE_LONG;
          DEBUG_VALUE(DEBUG_HIGH, "Long touch:", i);
          DEBUG_VALUELN(DEBUG_HIGH, " ms:", now - tap_times[i]);
          tap_times[i] = 0;
        }
      }

      sensor_states[i] = new_state;
    }
  }
}

boolean MPR121_State::checkTouched(byte s) {
  return (sensor_states[s] & SENSE_TOUCH);
}

boolean MPR121_State::checkChanged(byte s) {
  return (sensor_states[s] & SENSE_CHANGE);
}

boolean MPR121_State::checkTapped(byte s) {
  return ((sensor_states[s] & SENSE_TOUCH) && (sensor_states[s] & SENSE_CHANGE));
}

boolean MPR121_State::checkReleased(byte s) {
  return (!(sensor_states[s] & SENSE_TOUCH) && (sensor_states[s] & SENSE_CHANGE));
}

boolean MPR121_State::checkHeld(byte s) {
  return ((sensor_states[s] & SENSE_TOUCH) && !(sensor_states[s] & SENSE_CHANGE));
}

