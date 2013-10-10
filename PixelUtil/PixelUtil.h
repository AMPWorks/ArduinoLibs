#ifndef PIXELUTIL_H
#define PIXELUTIL_H

#include "SPI.h"
#include "Adafruit_WS2801.h"

class PixelUtil 
{
  public:

  PixelUtil();
  PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=WS2801_RGB);
  
  uint32_t pixelColor(byte r, byte g, byte b);

  uint16_t numPixels();
  void setPixelRGB(uint16_t led, byte r, byte g, byte b);
  void setPixelRGB(uint16_t led, uint32_t color);

  void update();

  /* Loop the pixels through a pattern */
  void patternLoop(byte pattern[][3], int patten_size, int periodms);
  void patternOne(int periodms);
  void patternRed(int periodms);

  private:
  Adafruit_WS2801 pixels;
};

class RGB {
 public:
  RGB();
  RGB(byte r, byte g, byte b);

  void setColor(byte r, byte g, byte b);
  void incrColor(int r, int g, int b);
  byte red();
  byte green();
  byte blue();

  uint16_t pixel;

 private:
#define RED 1
#define GREEN 2
#define BLUE 3
  union {
    byte argb[4];
    uint32_t color;
  } c;
};

uint32_t pixel_color(byte r, byte g, byte b);
uint32_t pixel_wheel(byte WheelPos);
uint32_t pixel_wheel(byte WheelPos, byte max);

#endif
