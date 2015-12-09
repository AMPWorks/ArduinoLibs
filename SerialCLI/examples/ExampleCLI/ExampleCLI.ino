/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This example parses the serial input, tokenizes it, and then prints each 
 * token.
 */
#include <Arduino.h>
#include <SerialCLI.h>

void handler(char **tokens, byte numtokens);

SerialCLI serialcli(32, handler);

void setup() {
  Serial.begin(9600);
}

void loop() {
  serialcli.checkSerial();
}

void handler(char **tokens, byte numtokens) {
  Serial.print(numtokens);
  Serial.print(" tokens: ");
  for (int token = 0; token < numtokens; token++) {
    if (token != 0) Serial.print(", ");
    Serial.print(tokens[token]);
  }
  Serial.println();
}
