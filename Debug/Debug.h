/* Set to enable debugging macros */
#define DEBUG
#ifdef DEBUG
  #define DEBUG_VERBOSE 2
  #define DEBUG_PRINT(v, x) if (v <= DEBUG_VERBOSE) Serial.print(x)
  #define DEBUG_PRINT_HEX(v, x) if (v <= DEBUG_VERBOSE) Serial.print(x, HEX)
  #define DEBUG_WRITE(v, x) if (v <= DEBUG_VERBOSE)  Serial.print(x, HEX);
  #define DEBUG_COMMAND(x) x;
#else
  #define DEBUG_PRINT(v, x)
  #define DEBUG_PRINT_HEX(v, x)
  #define DEBUG_WRITE(v, x)
  #define DEBUG_COMMAND(x)
#endif
