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

  void update();

  private:
  Adafruit_WS2801 pixels;
};

uint32_t pixel_color(byte r, byte g, byte b);
  
#endif
