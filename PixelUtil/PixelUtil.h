#ifndef PIXELUTIL_H
#define PIXELUTIL_H

#include "SPI.h"
#include "Adafruit_WS2801.h"

typedef union {
#define RED 2
#define GREEN 1
#define BLUE 0
#define UNUSED 3
  byte argb[4];
  uint32_t color;
} color_t;

class RGB {
 public:
  RGB();
  RGB(byte r, byte g, byte b);

  void setColor(byte r, byte g, byte b);
  void setColor(uint32_t c);

  void incrColor(int r, int g, int b);
  void incrNext(int r, int g, int b);

  byte red();
  byte green();
  byte blue();
  uint32_t color();

  uint16_t pixel;

 private:
  color_t c;
};

class PixelUtil 
{
  public:

  // WARNING: PixulUtil initialization *must* be after the Serial.init()
  PixelUtil();
  PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=WS2801_RGB);
  void init(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
	    uint8_t order);
  
  uint32_t pixelColor(byte r, byte g, byte b);

  uint16_t numPixels();
  void setPixelRGB(uint16_t led, byte r, byte g, byte b);
  void setPixelRGB(uint16_t led, uint32_t color);
  void setPixelRGB(RGB *rgb);

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

uint32_t fadeTowards(uint32_t from, uint32_t to, byte percent);

#endif
