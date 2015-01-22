/*
 * Example of a minimal RS485Socket master
 */
#define DEBUG_LEVEL DEBUG_HIGH
#include "Debug.h"

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>
#include "RS485Utils.h"

// The following pins should be adjusted based on your setup
#define PIN_RS485_1     2
#define PIN_RS485_2     7
#define PIN_RS485_3     4

#define DEBUG_PIN 13

RS485Socket rs485(PIN_RS485_1, PIN_RS485_2, PIN_RS485_3, 0);

#define SEND_BUFFER_SIZE (sizeof (rs485_socket_hdr_t) + 64)
byte databuffer[SEND_BUFFER_SIZE];
byte *send_buffer;

void setup()
{
  Serial.begin(9600);
  pinMode(DEBUG_PIN, OUTPUT);

  rs485.setup();
  send_buffer = rs485.initBuffer(databuffer);
}

#define MY_ADDR   0
#define DEST_ADDR 1

byte count = 0;
boolean debugOn = false;
void loop()
{
  send_buffer[0] = 'T';
  send_buffer[1] = count++;

  DEBUG1_VALUELN("Sending ", count);

  rs485.sendMsgTo(DEST_ADDR, send_buffer, 2);

  debugOn = !debugOn;
  if (debugOn)
    digitalWrite(DEBUG_PIN, HIGH);
  else
    digitalWrite(DEBUG_PIN, LOW);
  delay(1000);
}
