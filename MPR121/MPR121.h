#ifndef MPR121_H
#define MPR121_H

/*
    MPR121.h
	April 8, 2010
	by: Jim Lindblom

    Library conversion
        October 31, 2013
        by: Adam Phelps
*/



// MPR121 Register Defines
#define MHD_R	0x2B
#define NHD_R	0x2C
#define	NCL_R 	0x2D
#define	FDL_R	0x2E
#define	MHD_F	0x2F
#define	NHD_F	0x30
#define	NCL_F	0x31
#define	FDL_F	0x32
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
#define	REL_THRESH 0x06

class MPR121
{
 public:
  static const uint8_t MAX_SENSORS = 12;

  MPR121();

  /*
   * IMPORTANT NODE: Wire.begin() must be called before MPR121 initialization
   */
  MPR121(byte irqpin, boolean interrupt, byte address);
  MPR121(byte irqpin, boolean interrupt);


  boolean readTouchInputs();
  boolean touched(byte sensor);
  boolean previous(byte sensor);
  boolean changed(byte sensor);
  unsigned long touchTime(byte sensor);

  /* Set configuration values */
  void setThreshold(byte sensor, byte trigger, byte release);
  void setDebounce(byte trigger, byte release);

  void set_register(uint8_t r, uint8_t v);
  boolean read_register(uint8_t r, byte* result);

  boolean triggered;
  boolean useInterrupt;
 private:
  byte address;
  byte irqpin;

  void init(byte irqpin, boolean interrupt, byte address);

  boolean touchStates[MAX_SENSORS];
  boolean prevStates[MAX_SENSORS];
  unsigned long touchTimes[MAX_SENSORS];

  void initialize();
  void checkInterrupt();
};

/*
 * Check the board type in order to determine which pins match which interrupt
 */
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define INTERUPT_0_PIN 2
#define INTERUPT_1_PIN 3
#define INTERUPT_2_PIN -1
#define INTERUPT_3_PIN -1
#define INTERUPT_4_PIN -1
#define INTERUPT_5_PIN -1
#elif defined(__AVR_ATmega328P__)
#define INTERUPT_0_PIN 2
#define INTERUPT_1_PIN 3
#define INTERUPT_2_PIN 21
#define INTERUPT_3_PIN 20
#define INTERUPT_4_PIN 19
#define INTERUPT_5_PIN 18
#elif defined(__AVR_ATmega328P__)
#define INTERUPT_0_PIN 3
#define INTERUPT_1_PIN 2
#define INTERUPT_2_PIN 0
#define INTERUPT_3_PIN 1
#define INTERUPT_4_PIN 7
#define INTERUPT_5_PIN -1
#else
#define INTERUPT_0_PIN -1
#define INTERUPT_1_PIN -1
#define INTERUPT_2_PIN -1
#define INTERUPT_3_PIN -1
#define INTERUPT_4_PIN -1
#define INTERUPT_5_PIN -1
#endif

#endif
