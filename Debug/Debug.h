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
#define DEBUG_TRACE 4

#ifdef DEBUG /* Compatibility with earlier Debug.h syntax */
  #define DEBUG_LEVEL DEBUG_VERBOSE 
#endif

#ifdef DEBUG_LEVEL
  extern char close_line;

  #define DEBUG_PRINT(v, x) if (v <= DEBUG_LEVEL) {                     \
    close_line = true;                                                  \
    Serial.print(F(x));							\
  }
  #define DEBUG_PRINTLN(v, x) if (v <= DEBUG_LEVEL) {                   \
    close_line = false;                                                 \
    Serial.println(F(x));						\
  }
  #define DEBUG_HEX(v, x) if (v <= DEBUG_LEVEL) {                 \
    close_line = true;                                                  \
    Serial.print(x, HEX);                                               \
  }
  #define DEBUG_VALUE(v, x, y) if (v <= DEBUG_LEVEL) {                  \
    close_line = true;                                                  \
    Serial.print(F(x)); Serial.print(y);				\
  }
  #define DEBUG_HEXVAL(v, x, y) if (v <= DEBUG_LEVEL) {                 \
    close_line = true;                                                  \
    Serial.print(F(x)); Serial.print(y, HEX);				\
  }
  #define DEBUG_HEXVALLN(v, x, y) if (v <= DEBUG_LEVEL) {                 \
    close_line = false;                                                  \
    Serial.print(F(x)); Serial.println(y, HEX);				\
  }
  #define DEBUG_VALUELN(v, x, y) if (v <= DEBUG_LEVEL) {                \
    close_line = false;                                                 \
    Serial.print(F(x)); Serial.println(y);				\
  }
  #define DEBUG_PRINT_END() if (close_line) {                          \
    close_line = false;                                                 \
    Serial.println();                                                   \
  }

  #define DEBUG_ERR(x) DEBUG_PRINTLN(DEBUG_ERROR, x);

  #define DEBUG_COMMAND(v, x) if (v <= DEBUG_LEVEL) {     \
    x; \
  }

  #define DEBUG_ERR_STATE(x) debug_err_state(x);

  #define DEBUG_MEMORY(v)  if (v <= DEBUG_LEVEL) {     \
    debug_print_memory(); \
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
  #define DEBUG_ERR_STATE(x)
  #define DEBUG_MEMORY(v)
#endif

/* Error codes */
#define DEBUG_ERR_MALLOC 0x10

/* Flash an error code */
void debug_err_state(int code);

/* Print memory pointers and free space */
void debug_print_memory();
