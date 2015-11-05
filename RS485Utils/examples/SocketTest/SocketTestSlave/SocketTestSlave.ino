/*
 * Example of a minimal RS485Socket slave
 */
#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

#include "Socket.h"
#include "RS485Utils.h"

// The following pins should be adjusted based on your setup
#if 1
#define PIN_RS485_RECV        4
#define PIN_RS485_XMIT        7
#define PIN_RS485_ENABLED     2
#else
#define PIN_RS485_RECV        21
#define PIN_RS485_XMIT        22
#define PIN_RS485_ENABLED     23
#endif

#define RCV_LED 0 // 13
 

RS485Socket rs485(PIN_RS485_RECV, PIN_RS485_XMIT, PIN_RS485_ENABLED, (DEBUG_LEVEL != 0));

void setup() {
  Serial.begin(9600);

  pinMode(RCV_LED, OUTPUT);

  /* Setup the RS485 connection */  
  rs485.setup();

  DEBUG1_PRINTLN("Socket Test Slave initialized 1");
}

#define MY_ADDR 1
unsigned long last_print = 0;

void loop() {
  unsigned int msglen;

  const byte *data = rs485.getMsg(MY_ADDR, &msglen);
  if (data != NULL) {
    char letter = data[0];
    byte count = data[1];
    DEBUG1_VALUE("letter=", letter);
    DEBUG1_VALUELN(" count=", count);
    digitalWrite(RCV_LED, HIGH);
    last_print = millis();
  } else {
    delay(10);
    digitalWrite(RCV_LED, LOW);
  }

  if ((millis() - last_print) > 2000) {
    DEBUG1_PRINTLN("No data");
    last_print = millis();
  }
}
