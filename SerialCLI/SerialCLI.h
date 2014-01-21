/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This provides an interface for providing a tokenize serial CLI
 */

#ifndef SERIALCLI_H
#define SERIALCLI_H

typedef void (*cliHandler_t)(char **tokens, byte numtokens);

class SerialCLI {
 public:
  static const uint8_t MAX_TOKENS = 8;

  SerialCLI();
  SerialCLI(byte maxLength, cliHandler_t handler);

  void checkSerial();

 private:
  byte maxLength;
  char *buffer;
  byte buffPtr;
  cliHandler_t handleTokens;

  void tokenize();
};

#endif
