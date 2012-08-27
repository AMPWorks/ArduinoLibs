/*
 * In order to enable this code in a sketch, place the following:
 *   #define DEBUG
 *   #define DEBUG_VERBOSE <level>
 * before including this header.
 */

// Print levels
#define DEBUG_ERROR 0
#define DEBUG_LOW   1
#define DEBUG_MID   2
#define DEBUG_HIGH  3

#ifdef DEBUG
  #define DEBUG_PRINT(v, x) if (v <= DEBUG_VERBOSE) Serial.print(x)
  #define DEBUG_PRINT_HEX(v, x) if (v <= DEBUG_VERBOSE) Serial.print(x, HEX)
  #define DEBUG_WRITE(v, x) if (v <= DEBUG_VERBOSE)  Serial.print(x, HEX)
  #define DEBUG_COMMAND(x) x;
#else
  #define DEBUG_PRINT(v, x)
  #define DEBUG_PRINT_HEX(v, x)
  #define DEBUG_WRITE(v, x)
  #define DEBUG_COMMAND(x)
#endif
