#ifndef PIXELUTIL_H
#define PIXELUTIL_H

#include "SPI.h"
#include "Adafruit_WS2801.h"

typedef union {
#define RED 2
#define GREEN 1
#define BLUE 0
  byte argb[4];
  uint32_t color;
} color_t;

class RGB {
 public:
  RGB();
  RGB(byte r, byte g, byte b);

  void setColor(byte r, byte g, byte b);
  void setColor(uint32_t c);
  void setNext(byte r, byte g, byte b);

  void incrColor(int r, int g, int b);
  void incrNext(int r, int g, int b);

  byte red();
  byte green();
  byte blue();
  uint32_t color();

  void update();

  uint16_t pixel;

 private:
  color_t c, next;
};

class PixelUtil 
{
  public:

  // WARNING: PixulUtil initialization *must* be after the Serial.init()
  PixelUtil();
  PixelUtil(uint16_t numPixels, uint8_t dataPin, uint8_t clockPin,
            uint8_t order=WS2801_RGB);
  
  uint32_t pixelColor(byte r, byte g, byte b);

  uint16_t numPixels();
  void setPixelRGB(uint16_t led, byte r, byte g, byte b);
  void setPixelRGB(uint16_t led, uint32_t color);
  void setPixelRGB(RGB *rgb);

  void update();

  /* Loop the pixels through a pattern */
  void patternLoop(byte pattern[][3], int patten_size, int periodms);
  void patternOne(int periodms);
  void patternRed(int periodms);
  void patternGreen(int periodms);
  void patternBlue(int periodms);

  private:
  Adafruit_WS2801 pixels;
};


uint32_t pixel_color(byte r, byte g, byte b);
uint32_t pixel_wheel(byte WheelPos);
uint32_t pixel_wheel(byte WheelPos, byte max);

#endif
