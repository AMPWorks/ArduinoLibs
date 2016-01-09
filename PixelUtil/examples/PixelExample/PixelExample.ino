/*
 * Example usage of PixelUtil
 */
#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include "FastLED.h"
#include "PixelUtil.h"

#ifdef PIXELS_WS2812B_3
#define NUM_LEDS  16
#define DATA_PIN   3
#define CLOCK_PIN  -1
#define COLOR_ORDER RGB
#else
#define NUM_LEDS  16
#define DATA_PIN  12
#define CLOCK_PIN  8
#define COLOR_ORDER RGB
#endif

PixelUtil pixels;

void setup() {
  Serial.begin(9600);
  DEBUG1_PRINTLN("Starting PixelExample");

  pixels.init(NUM_LEDS, DATA_PIN, CLOCK_PIN, COLOR_ORDER);
  pixels.setAllRGB(0, 0, 0);
  pixels.update();
}

int cycle;
void loop() {
  switch (cycle % 3) {
    case 0:
      pixels.setAllRGB(255,0,0);
      DEBUG1_PRINTLN("RED");
      break;
    case 1:
      pixels.setAllRGB(0,255,0);
      DEBUG1_PRINTLN("GREEN");
      break;
    case 2:
      pixels.setAllRGB(0,0,255);
      DEBUG1_PRINTLN("BLUE");
      break;
  }

  pixels.update();

  cycle++;
  delay(1000);
}