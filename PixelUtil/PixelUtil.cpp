/*******************************************************************************
 * Pixel strand utility, a wrapper for the FastLED library which provides for
 * some additional functionality.
 *
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2014
 */

#include <Arduino.h>
#include "FastLED.h"

#ifdef DEBUG_LEVEL_PIXELUTIL
  #define DEBUG_LEVEL DEBUG_LEVEL_PIXELUTIL
#endif
#include "Debug.h"

#include "PixelUtil.h"
#include "PixelUtil_config.h"

PixelUtil::PixelUtil() 
{
  initialized = false;
}

// WARNING: PixulUtil initialization *must* be after the Serial.init()
// XXX: This might have been corrected by changing pixels to a pointer
PixelUtil::PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
                     uint8_t order)
{
  initialized = false;
  init(numPixels, dataPin, clockPin, order);
}

void PixelUtil::init(const uint16_t _numPixels, const uint8_t dataPin,
                     const uint8_t clockPin, const uint8_t order)
{
  if (initialized) {
    DEBUG_ERR("PixelUtil::init already initialized");
    DEBUG_ERR_STATE(DEBUG_ERR_REINIT);
  } else {

#ifndef BIG_PIXELS
    if (_numPixels > (PIXEL_ADDR_TYPE)-1) {
      DEBUG1_VALUELN("ERROR: Too many pixels:", _numPixels)
      DEBUG_ERR_STATE(DEBUG_ERR_INVALID);
    }
#endif

    num_pixels = (PIXEL_ADDR_TYPE)_numPixels;

#ifdef PIXEL_NUM_OVERRIDE
    DEBUG1_VALUELN("Pix override:", PIXEL_NUM_OVERRIDE);
    num_pixels = (PIXEL_ADDR_TYPE)PIXEL_NUM_OVERRIDE;
#endif

    /*
     * Allocate the LED data array.  This has proven to be a likely cause
     * of memory corruption when the number of LEDs is too high, which
     * leads to an error check for a common corruption below.
     */
    leds = new CRGB[num_pixels];
#ifndef PIXEL_NUM_OVERRIDE
    if (num_pixels != _numPixels) {
      DEBUG_ERR("Numpix changed");
      DEBUG_ERR_STATE(DEBUG_ERR_MEMCORRUPT);
    }
#endif

    /*
     * Call the macro to add LEDS based on compiler variables.
     */
    PIXELS_ADD();

    if (!initialized) {
      DEBUG_ERR("Invalid Pixel pin configuration");
      DEBUG_ERR_STATE(DEBUG_ERR_BADPINS);
    }

    FastLED.setCorrection(PIXELS_CORRECTION);

    setAllRGB(0, 0, 0);
    FastLED.show();
  }
  DEBUG2_VALUELN("PixelUtil::init ", numPixels());
}

uint16_t PixelUtil::numPixels() 
{
  return num_pixels;
}

void PixelUtil::setPixelRGB(uint16_t led, byte r, byte g, byte b)
{
  leds[led] = CRGB(r, g, b);
}

void PixelUtil::setPixelRGB(uint16_t led, CRGB rgb) {
  leds[led] = rgb;
}

void PixelUtil::setPixelRGB(uint16_t led, uint32_t color)
{
  leds[led] = color;
}

void PixelUtil::setPixelRGB(PRGB *rgb) {
  if (rgb->pixel < numPixels()) {
    leds[rgb->pixel] = rgb->color();
  }
}

void PixelUtil::setAllRGB(byte r, byte g, byte b)
{
  fill_solid(leds, numPixels(), CRGB(r, g, b));
}

void PixelUtil::setAllRGB(uint32_t color)
{
  fill_solid(leds, numPixels(), color);
}

void PixelUtil::setRangeRGB(pixel_range_t range, CRGB crgb) {
  if (range.start + range.length > numPixels()) {
    range.length = numPixels() - range.start;
  }
  if (range.length == 0) setAllRGB(crgb);
  else fill_solid(leds + range.start, range.length, crgb);
}

/**
 * Sets the brightness level of all LEDs
 * @param brightness
 */
void PixelUtil::setBrightness(uint8_t brightness) {
  FastLED.setBrightness(brightness);
}

/*
 * setDistinct() is used to set a single LED value in setups where each
 * value is a distinct led rather than RGB pixels.
 */
void PixelUtil::setDistinct(PIXEL_ADDR_TYPE led, byte value){
  leds[led / (PIXEL_ADDR_TYPE)3][led % 3] = value;
}


uint32_t PixelUtil::getColor(uint16_t led) {
  return pixel_color(leds[led].r, leds[led].g, leds[led].b);
}

void PixelUtil::update() 
{
  FastLED.show();
}

/* Loop through the given pattern */
void PixelUtil::patternLoop(byte pattern[][3], int pattern_size, int periodms) {
  static unsigned long next_time = millis();
  static byte current = 0;

  if (millis() > next_time) {
    setPixelRGB(current, 0, 0, 0);
    current = (current + 1) % numPixels();
    next_time += periodms;

    for (byte i = 0; i <  pattern_size; i++) {
      setPixelRGB((current + i) % numPixels(),
		  pattern[i][0], pattern[i][1], pattern[i][2]);
    }
  }
}

void PixelUtil::patternOne(int periodms) {
  byte pattern[9][3] = {
          {64,  0,   128},
	  {128, 0,   64 },
	  {255, 0,   0  },
	  {128, 64,  0  },
	  {64,  128, 0  },
	  {0,   255, 0  },
	  {0,   128, 64 },
          {0,   64,  128},
          {0,   0,   255},
        };
  patternLoop(pattern, 9, periodms);
}

void PixelUtil::patternRed(int periodms) {
  byte pattern[7][3] = {
    {32,  0, 0},
    {64,  0, 0},
    {128, 0 ,0},
    {255, 0, 0},
    {128, 0 ,0},
    {64,  0, 0},
    {32,  0, 0},
  };
  patternLoop(pattern, 7, periodms);
}

void PixelUtil::patternGreen(int periodms) {
  byte pattern[7][3] = {
    {0, 32,  0},
    {0, 64,  0},
    {0, 128, 0},
    {0, 255, 0},
    {0, 128, 0},
    {0, 64,  0},
    {0, 32,  0},
  };
  patternLoop(pattern, 7, periodms);
}

void PixelUtil::patternBlue(int periodms) {
  byte pattern[7][3] = {
    {0, 0, 32},
    {0, 0, 64},
    {0, 0, 128},
    {0, 0, 255},
    {0, 0, 128},
    {0, 0, 64},
    {0, 0, 32},
  };
  patternLoop(pattern, 7, periodms);
}

/*******************************************************************************
 * RGB class
 ******************************************************************************/
PRGB::PRGB() {
  setColor(0, 0, 0);
}

PRGB::PRGB(byte r, byte g, byte b) {
  setColor(r, g, b);
}

void PRGB::setColor(byte r, byte g, byte b) {
  red = r;
  green = g;
  blue = b;
}

void PRGB::setColor(uint32_t color) {
  setColor(pixel_red(color), pixel_green(color), pixel_blue(color));
}

void PRGB::incrColor(int r, int g, int b) {
  if ((r > 0) && (255 - r < red )) red = 255;
  else if ((r < 0) && (0 - r > red)) red = 0;
  else red += r;

  if ((g > 0) && (255 - g < green )) green = 255;
  else if ((g < 0) && (0 - g > green)) green = 0;
  else green += g;

  if ((b > 0) && (255 - b < blue )) blue = 255;
  else if ((b < 0) && (0 - b > blue)) blue = 0;
  else blue += b;
}

uint32_t PRGB::color() {
  return pixel_color(red, green, blue);
}

CRGB PRGB::getCRGB() {
  return CRGB(red, green, blue);
}
/*******************************************************************************
 * Generic color manipulation functions
 ******************************************************************************/

/* Construct a 32bit value from 8bit RGB values */
#ifndef pixel_color
uint32_t pixel_color(byte r, byte g, byte b)
{
  return PIXEL_COLOR(r,g,b);
}
#endif

byte pixel_red(uint32_t color) {
  return ((color & 0x00FF0000) >> 16);
}

byte pixel_green(uint32_t color) {
  return ((color & 0x0000FF00) >> 8);
}

byte pixel_blue(uint32_t color) {
  return (color & 0x000000FF);
}

//Input a value 0 to 255 to get a color value.
//The colours are a transition r - g -b - back to r
uint32_t pixel_wheel(byte WheelPos, byte max)
{
  // XXX - max should be scaling factor

  if (WheelPos < 85) {
   return pixel_color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
   WheelPos -= 85;
   return pixel_color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170; 
   return pixel_color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

uint32_t pixel_wheel(byte WheelPos) {
  return pixel_wheel(WheelPos, 255);
}

/* Choose a primary color mapped from 0-255 */
uint32_t pixel_primary(byte position) {
  switch (map(position, 0, 255, 0, 3)) {
  case 0: return pixel_color(255, 0,   0); break;
  case 1: return pixel_color(0,   255, 0); break;
  case 2: return pixel_color(0,   0,   255); break;
  default: {
    /* This should never be reached */
    return 0;
  }
  }
}

/* Choose a primary or secondary color mapped from 0-255 */
uint32_t pixel_secondary(byte position) {
  switch (map(position, 0, 255, 0, 5)) {
  case 0: return pixel_color(255, 0,   0); break;
  case 1: return pixel_color(255, 255, 0); break;
  case 2: return pixel_color(0,   255, 0); break;
  case 3: return pixel_color(0,   255, 255); break;
  case 4: return pixel_color(0,   0,   255); break;
  case 5: return pixel_color(255, 0,   255); break;
  default: {
    /* This should never be reached */
    return 0;
  }
  }
}

/* 
 * A pixel heat range, "evenly" distributed
 *   Blue -> Green -> Red -> Yellow -> White
 */
uint32_t pixel_heat(byte position) {
  if (position == 0) return 0;
 
  uint16_t range = map(position, 1, 255, 0, 256 * 5);
  byte offset = range % 256;
  if (range < 256) {
    return pixel_color(0,   0,   offset);
  } else if (range < 256*2) {
    return pixel_color(0,   offset, 255 - offset);
  } else if (range < 256*3) {
    return pixel_color(offset, 255 - offset,   0  );
  } else if (range < 256*4) {
    return pixel_color(255, offset, 0  );
  } else {
    return pixel_color(255, 255, offset);
  }
}

/* 
 * The same pixel heat range, but with full discrete colors
 *   Blue -> Green -> Red -> Yellow -> White
 */
uint32_t pixel_heat_discreet(byte position) {
  if (position == 0) return 0;
  switch (map(position, 0, 255, 0, 4)) {
  case 0: return pixel_color(0,   0,   255); break;
  case 1: return pixel_color(0,   255, 0  ); break;
  case 2: return pixel_color(255, 0,   0  ); break;
  case 3: return pixel_color(255, 255, 0  ); break;
  case 4: return pixel_color(255, 255, 255); break;
  default : {
    /* This should never be reached */
    return 0;
  }
  }
}


/* Return a color 'percent' of the way between the from and to colors */
uint32_t fadeTowards(uint32_t from, uint32_t to, byte percent) {
  int fromR = pixel_red(from);
  int fromG = pixel_green(from);
  int fromB = pixel_blue(from);
  int toR = pixel_red(to);
  int toG = pixel_green(to);
  int toB = pixel_blue(to);

  byte retR = fromR + ((toR - fromR) * percent) / 100;
  byte retG = fromG + ((toG - fromG) * percent) / 100;
  byte retB = fromB + ((toB - fromB) * percent) / 100;

  return pixel_color(retR, retG, retB);
}
