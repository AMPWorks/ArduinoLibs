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
  extern char close_line;

  #define DEBUG_PRINT(v, x) if (v <= DEBUG_LEVEL) {                     \
    close_line = true;                                                  \
    Serial.print(x);                                                    \
  }
  #define DEBUG_PRINTLN(v, x) if (v <= DEBUG_LEVEL) {                   \
    close_line = false;                                                 \
    Serial.println(x);                                                  \
  }
  #define DEBUG_PRINT_HEX(v, x) if (v <= DEBUG_LEVEL) {                 \
    close_line = true;                                                  \
    Serial.print(x, HEX);                                               \
  }
  #define DEBUG_VALUE(v, x, y) if (v <= DEBUG_LEVEL) {                  \
    close_line = true;                                                  \
    Serial.print(x); Serial.print(y);                                   \
  }
  #define DEBUG_HEXVAL(v, x, y) if (v <= DEBUG_LEVEL) {                 \
    close_line = true;                                                  \
    Serial.print(x); Serial.print(y, HEX);                              \
  }
  #define DEBUG_VALUELN(v, x, y) if (v <= DEBUG_LEVEL) {                \
    close_line = false;                                                 \
    Serial.print(x); Serial.println(y);                                 \
  }
  #define DEBUG_PRINT_END() if (close_line) {                          \
    close_line = false;                                                 \
    Serial.println();                                                   \
  }

  #define DEBUG_ERR(x) DEBUG_PRINTLN(DEBUG_ERROR, x);

#define DEBUG_COMMAND(v, x) if (v <= DEBUG_LEVEL) {     \
    x; \
  }
#else
  #define DEBUG_PRINT(v, x)
  #define DEBUG_PRINTLN(v, x)
  #define DEBUG_PRINT_HEX(v, x)
  #define DEBUG_WRITE(v, x)
  #define DEBUG_COMMAND(x)
  #define DEBUG_VALUE(v, x, y)
  #define DEBUG_HEXVAL(v, x, y)
  #define DEBUG_VALUELN(v, x, y)
  #define DEBUG_PRINT_END()
  #define DEBUG_ERR(x)
#endif
