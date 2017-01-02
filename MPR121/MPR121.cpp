/*
 * Library for intacting with MPR121 12-channel capacitive touch sensors
 *
 * This code was initially based on the MPR121 example code provided by 
 * Sparkfun: https://github.com/sparkfun/MPR121_Capacitive_Touch_Breakout/tree/master/Firmware/MPR121Q/Arduino%20Sketch
 * Bits have also been integrated from two other MPR121 implementations:
 *   Adafruit: https://github.com/adafruit/Adafruit_MPR121
 *   Bare Conductive: https://github.com/BareConductive/mpr121
 *
 * Datasheet: https://www.sparkfun.com/datasheets/Components/MPR121.pdf
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

#ifdef DEBUG_LEVEL_MPR121
  #define DEBUG_LEVEL DEBUG_LEVEL_MPR121
#endif
#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL DEBUG_MID
#endif
#include <Debug.h>

#include "MPR121.h"

/*
 * Due to the definition of attachInterrupt a class method cannot be specified
 * as the interrupt function.  To enable multiple MPR121's to be used multiple
 * handlers are defined.
 */
MPR121 *cap[2] = {NULL, NULL};
void irqTriggered0() {
  // TODO: properly tag function with 'interrupt'
  noInterrupts();
  if (cap[0] != NULL) cap[0]->triggered = true;
  interrupts();
}
void irqTriggered1() {
  noInterrupts();
  if (cap[1] != NULL) cap[1]->triggered = true;
  interrupts();
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
    digitalWrite(irqpin, HIGH); //enable pullup resistor XXX: Is this needed with interrupts? TODO: Don't need if physical one present?

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

  // Setup the configuration mask
  config_mask = 0x0C | // Enable all 12 electrodes
                0x30 | // enable proximity on all electrodes
                0x80;  // Baseline tracking is enabled with fast-start;

  initialized = true;

  DEBUG3_VALUE("MPR121: Initialized.  IRQ=", irqpin);
  DEBUG3_VALUELN(" useInterrupt=", useInterrupt);

  initialize(auto_enabled);
}

/*
 * Disable sensing
 */
void MPR121::disable() {
  set_register(ELE_CFG, 0x00);
}

/*
 * Enable sensing
 */
void MPR121::enable() {
  set_register(ELE_CFG, config_mask);
}

void MPR121::initialize(boolean auto_enabled) {

  // Clear registers for the case where the chip is being configured again
  // without the power being reset
  set_register(0x80, 0x63);

  disable();

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

  // Set proximity sensing defaults
  set_register(MHDPROXR, 0xFF);
  set_register(NHDPROXR, 0xFF);
  set_register(NCLPROXR, 0x00);
  set_register(FDLPROXR, 0x00);
  set_register(MHDPROXF, 0x01);
  set_register(NHDPROXF, 0x01);
  set_register(NCLPROXF, 0xFF);
  set_register(FDLPROXF, 0xFF);
  set_register(NHDPROXT, 0x00);
  set_register(NCLPROXT, 0x00);
  set_register(FDLPROXT, 0x00);

  // Section C - Sets default touch and release thresholds for each electrode
  setThresholds(TOU_THRESH, REL_THRESH);

  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(FIL_CFG, 0x04);
  

  // Section F
  // Enable Auto Config and auto Reconfig
  if (auto_enabled) {
    DEBUG2_PRINTLN("Auto-configure enabled");
    set_register(ATO_CFG0, 0x0B);
    set_register(ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V
    set_register(ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
    set_register(ATO_CFGT, 0xB5);  // Target = 0.9*USL = 0xB5 @3.3V
  }

  // Enable sensing
  enable();
}

void MPR121::setThreshold(byte sensor, byte trigger, byte release) {
  byte trig = ELE0_T + sensor * 2;
  byte rel = ELE0_R + sensor * 2;

  disable(); // Must be in stop mode to write to registers
  set_register(trig, trigger);
  set_register(rel, release);
  enable(); // Return to previous config state

  DEBUG3_VALUE("Set threshold- Sensor:", sensor);
  DEBUG3_VALUE(" trigger:", trigger);
  DEBUG3_VALUELN(" release:", release);
}

/* Set threshold on all sensors */
void MPR121::setThresholds(byte trigger, byte release) {
  DEBUG3_VALUE("setThresholds trigger:", trigger);
  DEBUG3_VALUELN(" release:", release);

  disable(); // Must be in stop mode to write to registers
  for (byte i = 0; i < MPR121::MAX_SENSORS; i++) {
    set_register(ELE0_T + 2 * i, trigger);
    set_register(ELE1_T + 2 * i, release);
  }
  enable(); // Return to previous config state
}


/* Set the debounce values, range for each is 0-7 */
void MPR121::setDebounce(byte trigger, byte release) {
  byte value = (((release & 0x7) << 4) | (trigger & 0x7));

  disable(); // Must be in stop mode to write to registers
  set_register(DEBOUNCE, value);
  enable(); // Return to previous config state

  DEBUG3_VALUE("Set debouce- trigger:", trigger);
  DEBUG3_VALUELN(" release:", release);
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
   * The prevStates are used to determine if values have changed since the
   * previous call and so should be updated regardless of whether IRQ was
   * triggered.
   */
  prevStates = touchStates;

  if (triggered) {
    triggered = false;

    // read the touch state from the MPR121
    Wire.requestFrom(address, (uint8_t)2);

    if (Wire.available() < 2) {
      DEBUG4_PRINTLN("No response from MPR121");
      return false;
    }

    byte LSB = Wire.read();
    byte MSB = Wire.read();

    // 16bits that make up the touch states
    touchStates = ((MSB << 8) | LSB);

    DEBUG5_COMMAND(
                  // Check what electrodes were pressed
                  for  (int i = 0; i < MAX_SENSORS; i++) {
                    if (touched(i)){
                      if (!previous(i)) {
                        //pin i was just touched
                        DEBUG5_VALUELN("Touched pin ", i);
                      }
                    } else {
                      if (touched(i)) {
                        //pin i is no longer being touched
                        DEBUG5_VALUELN("Released pin ", i);
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

uint8_t MPR121::getBaseline(byte sensor) {
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

/*
 * Read a single byte value from a register
 */
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

/*
 * Read a 2-byte value from a register
 */
boolean MPR121::read16(uint8_t r, uint16_t *result) {
  Wire.beginTransmission(address);
  Wire.write(r);
  Wire.endTransmission(false);

  Wire.requestFrom(address,  (uint8_t)2, (uint8_t)true);

  if (Wire.available() >= 2) {
    result[0] = Wire.read();
    result[1] = Wire.read();
    return true;
  } else {
    return false;
  }

}

/*
 * Get the raw sensor values
 */
boolean MPR121::getBaselineAll() {
  Wire.beginTransmission(address);
  // set address register to read from the start of the baeline data
  Wire.write(E0BV);
  Wire.endTransmission(false); // repeated start

  if (Wire.requestFrom(address, MAX_SENSORS) == MAX_SENSORS) {
    for (int i=0; i < MAX_SENSORS; i++){ // 13 raw values
      byte value = Wire.read();
      DEBUG3_VALUE(" ", value);
    }

    return true;
  } else {
    return false;
  }

}

/*
 * Get the filtered sensor values
 */
boolean MPR121::getFilteredAll() {
  unsigned char LSB, MSB;

  Wire.beginTransmission(address);
  // set address register to read from the start of the filtered data
  Wire.write(E0FDL);
  Wire.endTransmission(false); // repeated start

  if (Wire.requestFrom((uint8_t)address,
                       (uint8_t)(MAX_SENSORS * 2)) == MAX_SENSORS * 2) {
    for(int i=0; i < MAX_SENSORS; i++){ // 13 filtered values
      LSB = Wire.read();
      MSB = Wire.read();
      uint16_t result = ((MSB << 8) | LSB);
      DEBUG3_VALUE(" ", result);
    }

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
          DEBUG4_VALUE("Double tap:", i);
          DEBUG4_VALUELN(" ms:", now - tap_times[i]);
        }
        tap_times[i] = now;
      }

      if (checkHeld(i)) {
        // Detect long touch
        if ((tap_times[i] != 0) && (now - tap_times[i] > LONG_TOUCH_MS)) {
          new_state |= SENSE_LONG;
          DEBUG4_VALUE("Long touch:", i);
          DEBUG4_VALUELN(" ms:", now - tap_times[i]);
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

