/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2015
 *
 * Simple test to send and receive data using XBeeSocket
 */

#include <Arduino.h>

#define DEBUG_LEVEL DEBUG_TRACE
#include "Debug.h"

#include <XBee.h>

#include <Socket.h>
#include <XBeeSocket.h>

#define MY_ADDR   1 // SOCKET_ADDR_ANY
#define DEST_ADDR 2 // SOCKET_ADDR_ANY

#define STATUS_LED 13 // Green
#define ERROR_LED  10 // Blue
#define DATA_LED   11 // Red

#define PERIOD 1000

XBee xbee = XBee();
XBeeSocket xb(&xbee, MY_ADDR);

#define DATA_SIZE 64
#define SEND_BUFFER_SIZE (XBEE_BUFFER_TOTAL(DATA_SIZE))
byte databuffer[SEND_BUFFER_SIZE];
byte *send_buffer;

void setup()
{
  Serial.begin(9600);

  xb.setup();
  send_buffer = xb.initBuffer(databuffer, DATA_SIZE);

  pinMode(STATUS_LED, OUTPUT);
  pinMode(ERROR_LED, OUTPUT);
  pinMode(DATA_LED, OUTPUT);
}

typedef struct {
  char letter;
  byte value;
} payload_t;

byte count = 0;
void loop()
{
  unsigned long startMs = millis();
  unsigned long elapsed;

  // Send a message
  payload_t *payload = (payload_t *)send_buffer;
  unsigned int msglen = sizeof (payload_t);
  payload->letter = 'T';
  payload->value = count++;

  xb.sendMsgTo(DEST_ADDR, send_buffer, msglen);

  // Check for received messages
  boolean received = false;
  const byte *data;
  do {
    data = xb.getMsg(MY_ADDR, &msglen);
    if (data != NULL) {
      if (msglen != sizeof (payload_t)) {
        // The message length doesn't match what was expected
        flashLed(ERROR_LED, 10, 25);
      } else {
        payload_t *received = (payload_t *)data;
        flashLed(DATA_LED, received->value, 25);
      }
      received = true;
    }
    elapsed = startMs - millis();
  } while ((data != NULL) && (elapsed < PERIOD));

  if (received) {
    pinMode(STATUS_LED, HIGH);
  } else {
    pinMode(STATUS_LED, LOW);
  }

  if (elapsed < PERIOD) {
    delay(PERIOD - elapsed);
  }
}

void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}
