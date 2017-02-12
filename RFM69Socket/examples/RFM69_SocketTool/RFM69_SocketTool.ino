/*
 * Example of a minimal RFM69Socket node
 */

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include <RFM69.h>
#include <SPI.h>
#include <RFM69Socket.h>


#ifndef NODEID
#define NODEID 123
#endif

#define NETWORK 100

#ifndef IRQ_PIN
#define IRQ_PIN 2
#endif

#ifndef IS_RFM69HW
#define IS_RFM69HW true
#endif

#define DATA_SIZE RFM69_DATA_LENGTH(RF69_MAX_DATA_LEN)
#define SEND_BUFFER_SIZE RFM69_BUFFER_TOTAL(DATA_SIZE)
byte databuffer[SEND_BUFFER_SIZE];
byte *send_buffer;

RFM69Socket rfm69;


void setup() {
  Serial.begin(115200);
  delay(2000);

  rfm69.init(NODEID, NETWORK, IRQ_PIN, IS_RFM69HW, RF69_915MHZ);
  send_buffer = rfm69.initBuffer(databuffer, DATA_SIZE);

  DEBUG1_PRINTLN("*** RFM69_SocketTool initialized ***")
}

#define SEND_PERIOD 1000
unsigned long last_send_ms = 0;
byte count = 0;

void loop() {
  unsigned long now = millis();

  if (now - SEND_PERIOD >= last_send_ms) {
    send_buffer[0] = 'T';
    send_buffer[1] = count++;
    DEBUG1_VALUELN("* Sending ", count);

    rfm69.sendMsgTo(RF69_BROADCAST_ADDR, send_buffer, 2);

    last_send_ms = now;
  }

  unsigned int retlen = -1;
  const byte *data = rfm69.getMsg(&retlen);
  if (data != NULL) {
    DEBUG1_VALUE("* Received ", data[1]);
    print_hex_buffer((char *)rfm69.headerFromData(data), rfm69.getLength());
    DEBUG_PRINT_END();
  }
}