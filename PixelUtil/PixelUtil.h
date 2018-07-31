/*******************************************************************************
 * Pixel strand utility, a wrapper for the FastLED library which provides for
 * some additional functionality.
 *
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2014
 */

/*
 * There are a number of compile flags used to control the behavior of
 * PixelUtil:
 *
 *   BIG_PIXELS: This should be enabled to allow more than 255 LEDs, leaving it
 *               disabled slightly reduces memory usage.
 *   PIXEL_NUM_OVERRIDE: Use to override the number of LEDs passed in at
 *               initialization.  This is useful if you want to set the number of
 *               LEDs in a platformio config rather than in code or configuration.
 *   DEBUG_LEVEL_PIXELUTIL: Set a debug level specific to PixelUtil
 *
 * Since FastLED uses compiler tricks to determine which type of LED code to
 * include a compiler flag must be provided to specify the LED type and the MCU
 * pins being used.  The format for this is
 *   PIXELS_<type>_<datapin>[_clockpin]
 * such as:
 *
 *   PIXELS_WS2812B_3: WS2812B LEDs with data pin 3
 *   PIXELS_APA102_12_8: APA102 LEDs with data pin 12 and clock pin 8
 */

#ifndef PIXELUTIL_H
#define PIXELUTIL_H

#include "FastLED.h"

//#define BIG_PIXELS // Uncomment to allow > 256 pixels
#ifdef BIG_PIXELS
  #define PIXEL_ADDR_TYPE uint16_t
#else
  #define PIXEL_ADDR_TYPE uint8_t
#endif

/* Structure to represent a range of pixels */
typedef struct {
  PIXEL_ADDR_TYPE start;
  PIXEL_ADDR_TYPE length;
} pixel_range_t;

class PRGB {
 public:
  PRGB();
  PRGB(byte r, byte g, byte b);

  void setColor(byte r, byte g, byte b);
  void setColor(uint32_t c);
  void incrColor(int r, int g, int b);

  uint32_t color();
  CRGB getCRGB();


  PIXEL_ADDR_TYPE pixel;  // XXX: Can this be eliminated?!

  byte red, green, blue;
}; // XXX: Eliminate this class, replace with CRGB from FastLED!

class PixelUtil 
{
  public:

  // WARNING: PixulUtil initialization *must* be after the Serial.init()
  PixelUtil();
  PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=RGB);
  void init(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=RGB);
  
  uint16_t numPixels();
  void setPixelRGB(uint16_t led, byte r, byte g, byte b);
  void setPixelRGB(uint16_t led, uint32_t color);
  void setPixelRGB(uint16_t led, CRGB crgb);
  void setPixelRGB(PRGB *rgb);
  void setAllRGB(byte r, byte g, byte b);
  void setAllRGB(uint32_t color);
  void setRangeRGB(pixel_range_t range, CRGB crgb);
  void setBrightness(uint8_t brightness);

  /*
   * setDistinct() is used to set a single LED value in setups where each
   * value is a distinct led rather than RGB pixels.
   */
  void setDistinct(PIXEL_ADDR_TYPE led, byte value);

  uint32_t getColor(uint16_t led);

  void update();

  /* Loop the pixels through a pattern */
  void patternLoop(byte pattern[][3], int patten_size, int periodms);
  void patternOne(int periodms);
  void patternRed(int periodms);
  void patternGreen(int periodms);
  void patternBlue(int periodms);

  private:
  PIXEL_ADDR_TYPE num_pixels;

  CRGB *leds;
  boolean initialized;
};

#define PIXEL_COLOR(r,g,b) \
  (((uint32_t)r << 16) | ((uint32_t)g << 8) | (uint32_t)b)
uint32_t pixel_color(byte r, byte g, byte b);

byte pixel_red(uint32_t color);
byte pixel_green(uint32_t color);
byte pixel_blue(uint32_t color);
uint32_t pixel_wheel(byte WheelPos);
uint32_t pixel_wheel(byte WheelPos, byte max);
uint32_t pixel_primary(byte position);
uint32_t pixel_secondary(byte position);
uint32_t pixel_heat(byte position);
uint32_t pixel_heat_discrete(byte position);

uint32_t fadeTowards(uint32_t from, uint32_t to, byte percent);

#endif
