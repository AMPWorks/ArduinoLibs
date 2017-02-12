/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * In order to enable this code in a sketch, place the following:
 *   #define DEBUG_LEVEL <level>
 * before including this header in a file.
 *
 * Enabling debugging will increase PROGMEM usage as all strings are stored
 * in progmem.
 */

// Print levels
#define DEBUG_NONE  0
#define DEBUG_ERROR 1
#define DEBUG_LOW   2
#define DEBUG_MID   3
#define DEBUG_HIGH  4
#define DEBUG_TRACE 5
#define DEBUG_ERROR_ONLY -1 // Will only compile error commands

#ifdef DEBUG_LEVEL
  #if DEBUG_LEVEL == DEBUG_ERROR_ONLY
    #define DEBUG_ERR(x)   Serial.println(F(x));
    #define DEBUG_ERR_STATE(x) debug_err_state(x);
  #else
    #define DEBUG
  #endif
#endif

#if DEBUG_LEVEL >= 1
  extern char close_line;

  #define DEBUG1_PRINT(x) { close_line = true; Serial.print(F(x)); }
  #define DEBUG1_PRINTLN(x) { close_line = false; Serial.println(F(x)); }
  #define DEBUG1_HEX(x) { close_line = true; Serial.print(x, HEX); }
  #define DEBUG1_HEXLN(x) { close_line = false; Serial.println(x, HEX); }
  #define DEBUG1_VALUE(x, y) { \
    close_line = true; Serial.print(F(x)); Serial.print(y); }
  #define DEBUG1_VALUELN(x, y) { \
    close_line = false; Serial.print(F(x)); Serial.println (y); }
  #define DEBUG1_HEXVAL(x, y) { \
    close_line = true; Serial.print(F(x)); Serial.print(y, HEX); }
  #define DEBUG1_HEXVALLN(x, y) { \
      close_line = false; Serial.print(F(x)); Serial.println(y, HEX); }
  #define DEBUG_ENDLN() { close_line = false; Serial.println();}

  #define DEBUG_ERR(x) DEBUG1_PRINTLN(x);
  #define DEBUG_ERR_STATE(x) debug_err_state(x);
  #define DEBUG1_COMMAND(x) { x; }

#else // #if DEBUG_LEVEL >= 1
  #define DEBUG1_PRINT(x)
  #define DEBUG1_PRINTLN(x)
  #define DEBUG1_HEX(x)
  #define DEBUG1_HEXLN(x)
  #define DEBUG1_VALUE(x, y)
  #define DEBUG1_VALUELN(x, y)
  #define DEBUG1_HEXVAL(x, y)
  #define DEBUG1_HEXVALLN(x, y)
  #define DEBUG_ENDLN()
\
  #if DEBUG_LEVEL == DEBUG_ERROR_ONLY
    #define DEBUG_ERR(x)   Serial.println(F(x));
    #define DEBUG_ERR_STATE(x) debug_err_state(x);
  #else
    #define DEBUG_ERR(x)
    #define DEBUG_ERR_STATE(x)
  #endif
  #define DEBUG1_COMMAND(x)
#endif // #if DEBUG_LEVEL >= 1

#if DEBUG_LEVEL >= 2
  #define DEBUG2_PRINT(x)          DEBUG1_PRINT(x)
  #define DEBUG2_PRINTLN(x)        DEBUG1_PRINTLN(x)
  #define DEBUG2_HEX(x)            DEBUG1_HEX(x)
  #define DEBUG2_HEXLN(x)          DEBUG1_HEXLN(x)
  #define DEBUG2_VALUE(x, y)    DEBUG1_VALUE(x, y)
  #define DEBUG2_VALUELN(x, y)  DEBUG1_VALUELN(x, y)
  #define DEBUG2_HEXVAL(x, y)   DEBUG1_HEXVAL(x, y)
  #define DEBUG2_HEXVALLN(x, y) DEBUG1_HEXVALLN(x, y)
  #define DEBUG2_COMMAND(x) { x; }
#else // #if DEBUG_LEVEL >= 2
  #define DEBUG2_PRINT(x)
  #define DEBUG2_PRINTLN(x)
  #define DEBUG2_HEX(x)
  #define DEBUG2_HEXLN(x)
  #define DEBUG2_VALUE(x, y)
  #define DEBUG2_VALUELN(x, y)
  #define DEBUG2_HEXVAL(x, y)
  #define DEBUG2_HEXVALLN(x, y)
  #define DEBUG2_COMMAND(x)
#endif // #if DEBUG_LEVEL >= 2

#if DEBUG_LEVEL >= 3
  #define DEBUG3_PRINT(x)          DEBUG1_PRINT(x)
  #define DEBUG3_PRINTLN(x)        DEBUG1_PRINTLN(x)
  #define DEBUG3_HEX(x)            DEBUG1_HEX(x)
  #define DEBUG3_HEXLN(x)          DEBUG1_HEXLN(x)
  #define DEBUG3_VALUE(x, y)    DEBUG1_VALUE(x, y)
  #define DEBUG3_VALUELN(x, y)  DEBUG1_VALUELN(x, y)
  #define DEBUG3_HEXVAL(x, y)   DEBUG1_HEXVAL(x, y)
  #define DEBUG3_HEXVALLN(x, y) DEBUG1_HEXVALLN(x, y)
  #define DEBUG3_COMMAND(x) { x; }
#else // #if DEBUG_LEVEL >= 3
  #define DEBUG3_PRINT(x)
  #define DEBUG3_PRINTLN(x)
  #define DEBUG3_HEX(x)
  #define DEBUG3_HEXLN(x)
  #define DEBUG3_VALUE(x, y)
  #define DEBUG3_VALUELN(x, y)
  #define DEBUG3_HEXVAL(x, y)
  #define DEBUG3_HEXVALLN(x, y)
  #define DEBUG3_COMMAND(x)
#endif // #if DEBUG_LEVEL >= 3

#if DEBUG_LEVEL >= 4
  #define DEBUG4_PRINT(x)          DEBUG1_PRINT(x)
  #define DEBUG4_PRINTLN(x)        DEBUG1_PRINTLN(x)
  #define DEBUG4_HEX(x)            DEBUG1_HEX(x)
  #define DEBUG4_HEXLN(x)          DEBUG1_HEXLN(x)
  #define DEBUG4_VALUE(x, y)    DEBUG1_VALUE(x, y)
  #define DEBUG4_VALUELN(x, y)  DEBUG1_VALUELN(x, y)
  #define DEBUG4_HEXVAL(x, y)   DEBUG1_HEXVAL(x, y)
  #define DEBUG4_HEXVALLN(x, y) DEBUG1_HEXVALLN(x, y)
  #define DEBUG4_COMMAND(x) { x; }
#else // #if DEBUG_LEVEL >= 4
  #define DEBUG4_PRINT(x)
  #define DEBUG4_PRINTLN(x)
  #define DEBUG4_HEX(x)
  #define DEBUG4_HEXLN(x)
  #define DEBUG4_VALUE(x, y)
  #define DEBUG4_VALUELN(x, y)
  #define DEBUG4_HEXVAL(x, y)
  #define DEBUG4_HEXVALLN(x, y)
  #define DEBUG4_COMMAND(x)
#endif // #if DEBUG_LEVEL >= 4

#if DEBUG_LEVEL >= 5
  #define DEBUG5_PRINT(x)          DEBUG1_PRINT(x)
  #define DEBUG5_PRINTLN(x)        DEBUG1_PRINTLN(x)
  #define DEBUG5_HEX(x)            DEBUG1_HEX(x)
  #define DEBUG5_HEXLN(x)          DEBUG1_HEXLN(x)
  #define DEBUG5_VALUE(x, y)    DEBUG1_VALUE(x, y)
  #define DEBUG5_VALUELN(x, y)  DEBUG1_VALUELN(x, y)
  #define DEBUG5_HEXVAL(x, y)   DEBUG1_HEXVAL(x, y)
  #define DEBUG5_HEXVALLN(x, y) DEBUG1_HEXVALLN(x, y)
  #define DEBUG5_COMMAND(x) { x; }
#else // #if DEBUG_LEVEL >= 5
  #define DEBUG5_PRINT(x)
  #define DEBUG5_PRINTLN(x)
  #define DEBUG5_HEX(x)
  #define DEBUG5_HEXLN(x)
  #define DEBUG5_VALUE(x, y)
  #define DEBUG5_VALUELN(x, y)
  #define DEBUG5_HEXVAL(x, y)
  #define DEBUG5_HEXVALLN(x, y)
  #define DEBUG5_COMMAND(x)
#endif // #if DEBUG_LEVEL >= 5

  /*
   * TODO: This section is deprecated, delete when possible
   */
#ifdef DEBUG
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

  #ifndef DEBUG_ERR
    #define DEBUG_ERR(x) DEBUG_PRINTLN(DEBUG_ERROR, x);
  #endif

  #ifndef DEBUG_ERR_STATE
    #define DEBUG_ERR_STATE(x) debug_err_state(x);
  #endif

  #define DEBUG_COMMAND(v, x) if (v <= DEBUG_LEVEL) {     \
    x; \
  }

  #define DEBUG_MEMORY(v)  if (v <= DEBUG_LEVEL) {     \
    debug_print_memory(); \
  }
#else
  #define DEBUG_PRINT(v, x)
  #define DEBUG_PRINTLN(v, x)
  #define DEBUG_PRINT_HEX(v, x)
  #define DEBUG_WRITE(v, x)
  #define DEBUG_VALUE(v, x, y)
  #define DEBUG_HEX(v, x)
  #define DEBUG_HEXVAL(v, x, y)
  #define DEBUG_HEXVALLN(v, x, y)
  #define DEBUG_VALUELN(v, x, y)
  #define DEBUG_PRINT_END()
  #define DEBUG_COMMAND(v, x)
  #define DEBUG_MEMORY(v)
  #ifndef DEBUG_ERR
    #define DEBUG_ERR(x)
  #endif
  #ifndef DEBUG_ERR_STATE
    #define DEBUG_ERR_STATE(x)
  #endif
#endif

/* Error codes */
#define DEBUG_ERR_MALLOC  0x10
#define DEBUG_ERR_UNINIT  0x11
#define DEBUG_ERR_INVALID 0x12
#define DEBUG_ERR_REINIT  0x13
#define DEBUG_ERR_BADPINS 0x14

/*
 * Enter a loop which will continually flash an error code on
 * an Arduino's build in LED (typically pin 13).
 */
void debug_err_state(int code);

/* Print memory pointers and free space */
void debug_print_memory();

/* Print out a data buffer in hex */
void print_hex_buffer(const char *buff, int length);