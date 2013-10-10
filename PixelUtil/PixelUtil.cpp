/*
 * Pixel strand utility, derived from Adafruit_WS2801 library example
 */

#include <Arduino.h>
#include "SPI.h"
#include "Adafruit_WS2801.h"

//#define DEBUG_LEVEL DEBUG_HIGH
#include "Debug.h"

#include "PixelUtil.h"

PixelUtil::PixelUtil() 
{

}

PixelUtil::PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
                     uint8_t order) 
{
  pixels = Adafruit_WS2801(numPixels, dataPin, clockPin, order);
  pixels.begin();
  pixels.show();
}

uint16_t PixelUtil::numPixels() 
{
  return pixels.numPixels();
}

/* Construct a 32bit value from 8bit RGB values */
uint32_t PixelUtil::pixelColor(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
}

void PixelUtil::setPixelRGB(uint16_t led, byte r, byte g, byte b)
{
  pixels.setPixelColor(led, pixel_color(r, g, b));
}

void PixelUtil::setPixelRGB(uint16_t led, uint32_t color)
{
  pixels.setPixelColor(led, color);
}

void PixelUtil::update() 
{
  pixels.show();
}

/* Loop through the given pattern */
void PixelUtil::patternLoop(byte pattern[][3], int pattern_size, int periodms) {
  static unsigned long next_time = millis();
  static byte current = 0;

  if (millis() > next_time) {
    setPixelRGB(current % numPixels(), 0, 0, 0);
    current++;
    next_time += periodms;
  }

  for (byte i = 0; i <  pattern_size; i++) {
    setPixelRGB((current + i) % numPixels(),
		pattern[i][0], pattern[i][1], pattern[i][2]);
  }
}

void PixelUtil::patternOne(int periodms) {
  static byte pattern[9][3] = {
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
  static byte pattern[7][3] = {
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

/*******************************************************************************
 * RGB class
 ******************************************************************************/
RGB::RGB() {
  setColor(0, 0, 0);
}

RGB::RGB(byte r, byte g, byte b) {
  setColor(r, g, b);
}

void RGB::setColor(byte r, byte g, byte b) {
  c.argb[RED] = r;
  c.argb[GREEN] = g;
  c.argb[BLUE] = b;
}

void RGB::incrColor(int r, int g, int b) {
  if ((r > 0) && (255 - r < c.argb[RED] )) c.argb[RED] = 255;
  else if ((r < 0) && (0 - r > c.argb[RED])) c.argb[RED] = 0;
  else c.argb[RED] += r;

  if ((g > 0) && (255 - g < c.argb[GREEN] )) c.argb[GREEN] = 255;
  else if ((g < 0) && (0 - g > c.argb[GREEN])) c.argb[GREEN] = 0;
  else c.argb[GREEN] += g;

  if ((b > 0) && (255 - b < c.argb[BLUE] )) c.argb[BLUE] = 255;
  else if ((b < 0) && (0 - b > c.argb[BLUE])) c.argb[BLUE] = 0;
  else c.argb[BLUE] += b;
}

byte RGB::red() {
  return c.argb[RED];
}

byte RGB::green() {
  return c.argb[GREEN];
}

byte RGB::blue() {
  return c.argb[BLUE];
}


/* Construct a 32bit value from 8bit RGB values */
uint32_t pixel_color(byte r, byte g, byte b)
{
  uint32_t c;
  c = r;
  c <<= 8;
  c |= g;
  c <<= 8;
  c |= b;
  return c;
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

void pixel_rainbow(Adafruit_WS2801 strip, uint8_t wait) {
  uint16_t i, j;
   
  for (j=0; j < 256; j++) {     // 3 cycles of all 256 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, pixel_wheel( (i + j) % 255));
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// Slightly different, this one makes the rainbow wheel equally distributed 
// along the chain
void rainbowCycle(Adafruit_WS2801 strip, uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 256 * 5; j++) {     // 5 cycles of all 25 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 96-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 96 is to make the wheel cycle around
      strip.setPixelColor(i, pixel_wheel( ((i * 256 / strip.numPixels()) + j) % 256) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(Adafruit_WS2801 strip, uint32_t c, uint8_t wait) {
  uint16_t i;
  
  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

