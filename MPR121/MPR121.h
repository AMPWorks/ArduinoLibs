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



#ifndef MPR121_H
#define MPR121_H

#define TS1 0x00
#define TS2 0x01
#define OORS1 0x02
#define OORS2 0x03

// filtered data
#define E0FDL 0x04
#define E0FDH 0x05
#define E1FDL 0x06
#define E1FDH 0x07
#define E2FDL 0x08
#define E2FDH 0x09
#define E3FDL 0x0A
#define E3FDH 0x0B
#define E4FDL 0x0C
#define E4FDH 0x0D
#define E5FDL 0x0E
#define E5FDH 0x0F
#define E6FDL 0x10
#define E6FDH 0x11
#define E7FDL 0x12
#define E7FDH 0x13
#define E8FDL 0x14
#define E8FDH 0x15
#define E9FDL 0x16
#define E9FDH 0x17
#define E10FDL 0x18
#define E10FDH 0x19
#define E11FDL 0x1A
#define E11FDH 0x1B
#define E12FDL 0x1C
#define E12FDH 0x1D

// baseline values
#define E0BV 0x1E
#define E1BV 0x1F
#define E2BV 0x20
#define E3BV 0x21
#define E4BV 0x22
#define E5BV 0x23
#define E6BV 0x24
#define E7BV 0x25
#define E8BV 0x26
#define E9BV 0x27
#define E10BV 0x28
#define E11BV 0x29
#define E12BV 0x2A

#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32

// proximity electrode touch sense baseline filters
// rising filter
#define MHDPROXR 0x36
#define NHDPROXR 0x37
#define NCLPROXR 0x38
#define FDLPROXR 0x39

// falling filter
#define MHDPROXF 0x3A
#define NHDPROXF 0x3B
#define NCLPROXF 0x3C
#define FDLPROXF 0x3D

// touched filter
#define NHDPROXT 0x3E
#define NCLPROXT 0x3F
#define FDLPROXT 0x40


// Electrode thresholds
#define	ELE0_T	0x41
#define	ELE0_R	0x42
#define	ELE1_T	0x43
#define	ELE1_R	0x44
#define	ELE2_T	0x45
#define	ELE2_R	0x46
#define	ELE3_T	0x47
#define	ELE3_R	0x48
#define	ELE4_T	0x49
#define	ELE4_R	0x4A
#define	ELE5_T	0x4B
#define	ELE5_R	0x4C
#define	ELE6_T	0x4D
#define	ELE6_R	0x4E
#define	ELE7_T	0x4F
#define	ELE7_R	0x50
#define	ELE8_T	0x51
#define	ELE8_R	0x52
#define	ELE9_T	0x53
#define	ELE9_R	0x54
#define	ELE10_T	0x55
#define	ELE10_R	0x56
#define	ELE11_T	0x57
#define	ELE11_R	0x58
#define ELEPROX_T 0x59
#define ELEPROX_R 0x5A

#define DEBOUNCE 0x5B
#define	FIL_CFG	0x5D
#define	ELE_CFG	0x5E
#define GPIO_CTRL0	0x73
#define	GPIO_CTRL1	0x74
#define GPIO_DATA	0x75
#define	GPIO_DIR	0x76
#define	GPIO_EN		0x77
#define	GPIO_SET	0x78
#define	GPIO_CLEAR	0x79
#define	GPIO_TOGGLE	0x7A
#define	ATO_CFG0	0x7B
#define	ATO_CFGU	0x7D
#define	ATO_CFGL	0x7E
#define	ATO_CFGT	0x7F

// Initial I2C address for MPR121
#define START_ADDRESS 0x5A

// Default touch and release thresholds
#define TOU_THRESH 0x0A
#define	REL_THRESH 0x0F

class MPR121
{
 public:
  static const uint8_t MAX_SENSORS = 13;

  MPR121();

  /*
   * IMPORTANT NODE: Wire.begin() must be called before MPR121 initialization
   */
  MPR121(byte irqpin, boolean interrupt, boolean times);
  MPR121(byte irqpin, boolean interrupt, byte address, boolean times);
  MPR121(byte irqpin, boolean interrupt, byte address, boolean times,
         boolean auto_enabled);
  void init(byte irqpin, boolean interrupt, byte address, boolean times, 
            boolean auto_enabled);

  void enable();
  void disable();

  boolean readTouchInputs();
  boolean touched(byte sensor);
  boolean previous(byte sensor);
  boolean changed(byte sensor);
  unsigned long touchTime(byte sensor);

  uint8_t getBaseline(byte sensor);
  uint16_t getFiltered(byte sensor);
  boolean getBaselineAll();
  boolean getFilteredAll();

  /* Set configuration values */
  void setThreshold(byte sensor, byte trigger, byte release);
  void setThresholds(byte trigger, byte release);
  void setDebounce(byte trigger, byte release);

  void set_register(uint8_t r, uint8_t v);
  boolean read_register(uint8_t r, byte* result);
  boolean read16(uint8_t r, uint16_t *result);

  boolean triggered; // XXX - Combine booleans into bitflags
  boolean useInterrupt; // XXX
 private:
  boolean initialized; // XXX
  byte address;
  byte irqpin;

  byte config_mask;

  uint16_t touchStates;
  uint16_t prevStates;
  unsigned long *touchTimes; // XXX - Uses 4bytes even when not used

  void initialize(boolean auto_enabled);
  void checkInterrupt();
};

/*
 * Check the board type in order to determine which pins match which interrupt
 */
#if defined(__AVR_ATmega328P__)
// Arduino Uno, Nano
#define INTERUPT_0_PIN 2
#define INTERUPT_1_PIN 3
#define INTERUPT_2_PIN -1
#define INTERUPT_3_PIN -1
#define INTERUPT_4_PIN -1
#define INTERUPT_5_PIN -1
#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
// Arduino Mega
#define INTERUPT_0_PIN 2
#define INTERUPT_1_PIN 3
#define INTERUPT_2_PIN 21
#define INTERUPT_3_PIN 20
#define INTERUPT_4_PIN 19
#define INTERUPT_5_PIN 18
#elif defined(__AVR_ATmega32u4__)
// Arduino Leonardo
#define INTERUPT_0_PIN 3
#define INTERUPT_1_PIN 2
#define INTERUPT_2_PIN 0
#define INTERUPT_3_PIN 1
#define INTERUPT_4_PIN 7
#define INTERUPT_5_PIN -1
#elif defined(__AVR_ATmega1284P__)
#define INTERUPT_0_PIN 10
#define INTERUPT_1_PIN 11
#define INTERUPT_2_PIN  2
#define INTERUPT_3_PIN -1
#define INTERUPT_4_PIN -1
#define INTERUPT_5_PIN -1
#elif defined(__AVR_ATmega32U4__)
#define INTERUPT_0_PIN  2
#define INTERUPT_1_PIN  3
#define INTERUPT_2_PIN  7
#define INTERUPT_3_PIN  0
#define INTERUPT_4_PIN  1
#define INTERUPT_5_PIN -1
#else
XXX - This board needs to be defined
#define INTERUPT_0_PIN -1
#define INTERUPT_1_PIN -1
#define INTERUPT_2_PIN -1
#define INTERUPT_3_PIN -1
#define INTERUPT_4_PIN -1
#define INTERUPT_5_PIN -1
#endif

/******************************************************************************
 * Class to track the state history of a sensor array
 */

class MPR121_State {
 public:
  MPR121_State(MPR121 *_sensor, uint16_t _sensor_map);

  void updateState(void);

  boolean checkTouched(byte s);
  boolean checkChanged(byte s);
  boolean checkTapped(byte s);
  boolean checkReleased(byte s);
  boolean checkHeld(byte s);

  static const uint8_t SENSE_TOUCH  = 0x01;
  static const uint8_t SENSE_CHANGE = 0x02;
  static const uint8_t SENSE_DOUBLE = 0x04;
  static const uint8_t SENSE_LONG   = 0x08;

  static const uint16_t DOUBLE_TAP_MS = 750;
  static const uint16_t LONG_TOUCH_MS = 750;

 private:
  MPR121 *sensor;
  uint16_t sensor_map;
  uint8_t sensor_states[MPR121::MAX_SENSORS];  // XXX - Should be configurable
  uint32_t tap_times[MPR121::MAX_SENSORS];
};


#endif
