/*******************************************************************************
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

// WARNING: PixulUtil initialization *must* be after the Serial.init()
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
  pixels.setPixelColor(led, pixelColor(r, g, b));
}

void PixelUtil::setPixelRGB(uint16_t led, uint32_t color)
{
  pixels.setPixelColor(led, color);
}

void PixelUtil::setPixelRGB(RGB *rgb) {
  pixels.setPixelColor(rgb->pixel, rgb->color());
}

uint32_t PixelUtil::getColor(uint16_t led) {
  return pixels.getPixelColor(led);
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
  c.argb[UNUSED] = 0;
}

void RGB::setColor(uint32_t color) {
  c.color = color;
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

uint32_t RGB::color() {
  return c.color;
}

//void RGB::update() {
  //  c.color = next.color;
//}


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
