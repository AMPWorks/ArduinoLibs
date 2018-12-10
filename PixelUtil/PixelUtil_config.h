/*
 * Definitions for pixel configurations.
 *
 * Since the FastLED library uses templates each type and pin combo must
 * be explicitly specified in the code, and unused types should not be
 * compiled in order to save code space.
 *
 * This header uses compiler flags to define the LED type and pins.
 *
 * There are two methods of setting the flags with compiler flags:
 *   - Specifying the type, data, and clock pins, ie:
 *     * -DPIXELS_TYPE=PIXELS_TYPE_WS2812B -DPIXELS_DATA=10 -DPIXELS_CLOCK=6
 *   - Using an alias, which must be explicitly listed in this file
 *     * -DPIXELS_APA102_7_8
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2018
*/

#ifndef PIXELUTIL_CONFIG_H
#define PIXELUTIL_CONFIG_H

#include "FastLED.h"

#define PIXELS_DEFINE0(type, order) \
  { \
    initialized = true; \
    FastLED.addLeds<type, order>(leds, num_pixels); \
  }

#define PIXELS_DEFINE1(type, data, order) \
  if (dataPin == data) { \
    initialized = true; \
    FastLED.addLeds<type, data, order>(leds, num_pixels); \
  }

#define PIXELS_DEFINE2(type, data, clock, order) \
  if ((dataPin == data) && (clockPin == clock)) { \
    initialized = true; \
    FastLED.addLeds<type, data, clock, order>(leds, num_pixels); \
  }


/* Possible Pixel Types, for #def value comparisons */
#define PIXELS_TYPE_WS2812B 1
#define PIXELS_TYPE_APA102  2
#define PIXELS_TYPE_WS2801  3

/*
 * PIXELS_TYPE_WS2812B
 */
#ifdef PIXELS_WS2812B_3
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 3
#endif

#ifdef PIXELS_WS2812B_5
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 5
#endif

#ifdef PIXELS_WS2812B_6
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 6
#endif

#ifdef PIXELS_WS2812B_9
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 9
#endif

#ifdef PIXELS_WS2812B_10
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 10
#endif

#ifdef PIXELS_WS2812B_11
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 11
#endif

#ifdef PIXELS_WS2812B_12
#define PIXELS_TYPE PIXELS_TYPE_WS2812B
#define PIXELS_DATA 12
#endif

/*
 * PIXELS_TYPE_APA102
 */
#ifdef PIXELS_APA102
#define PIXELS_TYPE PIXELS_TYPE_APA102
#endif

#ifdef PIXELS_APA102_5_7
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 5
#define PIXELS_CLOCK 7
#endif

#ifdef PIXELS_APA102_7_8
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 7
#define PIXELS_CLOCK 8
#endif

#ifdef PIXELS_APA102_9_6
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 9
#define PIXELS_CLOCK 6
#endif

#ifdef PIXELS_APA102_12_8
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 12
#define PIXELS_CLOCK 8
#endif

#ifdef PIXELS_APA102_9_10
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 9
#define PIXELS_CLOCK 10
#endif

#ifdef PIXELS_APA102_20_21
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 20
#define PIXELS_CLOCK 21
#endif

#ifdef PIXELS_APA102_11_13
#define PIXELS_TYPE PIXELS_TYPE_APA102
#define PIXELS_DATA 11
#define PIXELS_CLOCK 13
#endif

/*
 * PIXELS_TYPE_WS2801
 */
#ifdef PIXELS_WS2801_SPI
#define PIXELS_TYPE PIXELS_TYPE_WS2801
#endif

#ifdef PIXELS_WS2801_5_7 // This is for the 1284P based module using hardware SPI
#define PIXELS_TYPE PIXELS_TYPE_WS2801
#define PIXELS_DATA 5
#define PIXELS_CLOCK 7
#endif

#ifdef PIXELS_WS2801_19_20
#define PIXELS_TYPE PIXELS_TYPE_WS2801
#define PIXELS_DATA 19
#define PIXELS_CLOCK 20
#endif

/*
 * Default for when no compiler flag, uses the pin configuration of the Adam's
 * original HMTL boards.
 */
#ifndef PIXELS_TYPE
  #warning No pixel definition compiler flag was specified, using WS2801_12_8
  #define PIXELS_TYPE PIXELS_TYPE_WS2801
  #define PIXELS_DATA 12
  #define PIXELS_CLOCK 8
#endif


/*
 * Define the definition function, color order, and correction based on the
 * defined LED type
 */
#if PIXELS_TYPE == PIXELS_TYPE_WS2812B
  #define PIXELS_LED_TYPE WS2812B
  #define PIXELS_PINS 1
  #define PIXELS_ORDER GRB
  #define PIXELS_CORRECTION TypicalSMD5050
#elif PIXELS_TYPE == PIXELS_TYPE_APA102
  #define PIXELS_LED_TYPE APA102
  #define PIXELS_PINS 2
  #define PIXELS_ORDER GBR
  #define PIXELS_CORRECTION TypicalSMD5050
#elif PIXELS_TYPE == PIXELS_TYPE_WS2801
  #define PIXELS_LED_TYPE WS2801
  #define PIXELS_PINS 2
  #define PIXELS_ORDER RGB
  #define PIXELS_CORRECTION TypicalPixelString
#else
  #error PIXELS_TYPE was not defined
#endif


/*
 * Set the final definitions
 */
#ifndef PIXELS_DATA
  /* If no data pin specified then use SPI */
  #warning USING SPI // TODO: Remove
  #define PIXELS_ADD() PIXELS_DEFINE0(PIXELS_LED_TYPE, PIXELS_ORDER)
#else
  #if PIXELS_PINS == 1
    #define PIXELS_ADD() PIXELS_DEFINE1(PIXELS_LED_TYPE, PIXELS_DATA, PIXELS_ORDER)
  #else
    #define PIXELS_ADD() PIXELS_DEFINE2(PIXELS_LED_TYPE, PIXELS_DATA, PIXELS_CLOCK, PIXELS_ORDER)
  #endif
#endif

#endif //PIXELUTIL_CONFIG_H
