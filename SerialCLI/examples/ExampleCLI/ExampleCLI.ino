/*
 * Written by Adam Phelps, amp@cs.stanford.edu, 2014
 *
 * This example parses the serial input, tokenizes it, and then prints each 
 * token.
 */
#include <Arduino.h>
#include <SerialCLI.h>

void handler(char **tokens, byte numtokens);

SerialCLI serialcli(
        32,     // Max command length, this amount is allocated as a buffer
        handler // Function for handling tokenized commands
);

void setup() {
  Serial.begin(9600);
}

void loop() {
  serialcli.checkSerial();
}

void print_usage() {
  Serial.print(F(
" \n"
"Usage:\n"
 "  h - print this help\n"
 "  s <value> - Send a value\n"
));
}
void handler(char **tokens, byte numtokens) {
  Serial.print(numtokens);
  Serial.print(" tokens: ");
  for (int token = 0; token < numtokens; token++) {
    if (token != 0) Serial.print(", ");
    Serial.print(tokens[token]);
  }
  Serial.println();

  switch (tokens[0][0]) {

    case 'h': {
      print_usage();
      break;
    }

    case 's': {
      if (numtokens < 2) return;
      uint16_t value = atoi(tokens[1]);
      Serial.print("* Sensor request to: ");
      Serial.println(value);
      break;
    }

  }
}

