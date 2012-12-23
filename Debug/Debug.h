/*
 * In order to enable this code in a sketch, place the following:
 *   #define DEBUG_LEVEL <level>
 * before including this header.
 */

// Print levels
#define DEBUG_ERROR 0
#define DEBUG_LOW   1
#define DEBUG_MID   2
#define DEBUG_HIGH  3

#ifdef DEBUG /* Compatibility with earlier Debug.h syntax */
  #define DEBUG_LEVEL DEBUG_VERBOSE 
#endif

#ifdef DEBUG_LEVEL
  #define DEBUG_PRINT(v, x) if (v <= DEBUG_LEVEL) Serial.print(x)
  #define DEBUG_PRINTLN(v, x) if (v <= DEBUG_LEVEL) Serial.println(x)
  #define DEBUG_PRINT_HEX(v, x) if (v <= DEBUG_LEVEL) Serial.print(x, HEX)
  #define DEBUG_WRITE(v, x) if (v <= DEBUG_LEVEL)  Serial.print(x, HEX)
  #define DEBUG_COMMAND(x) x;
  #define DEBUG_VALUE(v, x, y) if (v <= DEBUG_LEVEL) {        \
      Serial.print(x); Serial.print(y);                         \
    }
  #define DEBUG_VALUELN(v, x, y) if (v <= DEBUG_LEVEL) {        \
      Serial.print(x); Serial.println(y);                         \
    }
#else
  #define DEBUG_PRINT(v, x)
  #define DEBUG_PRINTLN(v, x)
  #define DEBUG_PRINT_HEX(v, x)
  #define DEBUG_WRITE(v, x)
  #define DEBUG_COMMAND(x)
  #define DEBUG_VALUE(v, x, y)
  #define DEBUG_VALUELN(v, x, y)
#endif
