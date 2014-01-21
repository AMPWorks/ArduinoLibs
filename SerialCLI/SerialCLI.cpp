/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This provides an interface for providing a tokenize serial CLI
 */

#include <Arduino.h>

#include <SerialCLI.h>

SerialCLI::SerialCLI(byte _maxLength, cliHandler_t handler) {
  maxLength = _maxLength;
  buffer = (char *)malloc(maxLength);
  buffPtr = 0;
  handleTokens = handler;
}

/*
 * Read data from the serial interface and when a new lines is encountered
 * or the data exceeds the buffer length, then send it to the tokenizer.
 */
void SerialCLI::checkSerial() {
  while (Serial.available()) {
    char c = (char)Serial.read();
    buffer[buffPtr] = c;
    if (
	(c == '\n') // Line terminated
	|| 
	(buffPtr >= maxLength - 2) // Hit end of buffer
	) {

      if (c != '\n') buffPtr++; // Move past the read character
      buffer[buffPtr] = 0;

      tokenize();
      buffPtr = 0;
    } else {
      buffPtr++;
    }
  }
}

/*
 * Break the input into space-separated tokens and pass the resulting token
 * array to the token handling function.
 */
void SerialCLI::tokenize() {
  char *tokens[MAX_TOKENS];
  for (int i = 0; i < MAX_TOKENS; i++) {
    tokens[i] = NULL;
  }

  char *value_ptr = buffer;
  int token = 0;
  tokens[token] = value_ptr;
  while (*value_ptr != 0) {
    if (*value_ptr == ' ') {
      /* Replace the space at the end of the token with a termination */
      *value_ptr = '\0';
     
      if (token == (MAX_TOKENS - 1)) {
	break;
      }
      token++;

      value_ptr++;
      tokens[token] = value_ptr;
      continue;
    }
    value_ptr++;
  }

  /* Call the token handler */
  handleTokens(tokens, token + 1);
}
