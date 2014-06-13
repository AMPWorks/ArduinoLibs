
#ifndef PIXELUTIL_H
#define PIXELUTIL_H

#include "SPI.h"
#include "Adafruit_WS2801.h"

class RGB {
 public:
  RGB();
  RGB(byte r, byte g, byte b);

  void setColor(byte r, byte g, byte b);
  void setColor(uint32_t c);
  void incrColor(int r, int g, int b);

  uint32_t color();

  //#define BIG_PIXELS // Uncomment to allow > 256 pixels
#ifdef BIG_PIXELS
  uint16_t pixel;
#else
  uint8_t pixel;
#endif
  byte red, green, blue;
};

class PixelUtil 
{
  public:

  // WARNING: PixulUtil initialization *must* be after the Serial.init()
  PixelUtil();
  PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=WS2801_RGB);
  void init(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
	    uint8_t order=WS2801_RGB);
  
  uint16_t numPixels();
  void setPixelRGB(uint16_t led, byte r, byte g, byte b);
  void setPixelRGB(uint16_t led, uint32_t color);
  void setPixelRGB(RGB *rgb);
  void setAllRGB(byte r, byte g, byte b);

  uint32_t getColor(uint16_t led);

  void update();

  /* Loop the pixels through a pattern */
  void patternLoop(byte pattern[][3], int patten_size, int periodms);
  void patternOne(int periodms);
  void patternRed(int periodms);
  void patternGreen(int periodms);
  void patternBlue(int periodms);

  private:
  Adafruit_WS2801 *pixels;
  boolean initialized;
};


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
