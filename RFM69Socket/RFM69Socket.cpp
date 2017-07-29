/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2014
 *
 * Socket wrapper of RFM69 code
 */

#ifdef DEBUG_LEVEL_RFM69SOCKET
#define DEBUG_LEVEL DEBUG_LEVEL_RFM69SOCKET
#endif
#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL DEBUG_TRACE
#endif
#include "Debug.h"

#include <Arduino.h>
#include <RFM69.h>

#include "Socket.h"
#include "RFM69Socket.h"

// Debug error codes
#define RFM_OVERSIZED_SEND_BUFFER 1

// TODO: This needs to choose the correct interrupt # based on processor and pin
#if defined(__AVR_ATmega644P__) || defined(__AVR_ATmega1284P__)
#define INT_FROM_PIN(pin) (pin == 2 ? 2 : ((pin == 10) ? 0 : ((pin == 11) ? 1 : -1)))
#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__) || defined(__AVR_ATmega88) || defined(__AVR_ATmega8__) || defined(__AVR_ATmega88__)
#define INT_FROM_PIN(pin) (pin == 2 ? 0 : ((pin == 3) ? 1 : -1))
#else
#error Unknown processor type
#endif

RFM69Socket::RFM69Socket() {
  radio = NULL;
}

RFM69Socket::RFM69Socket(socket_addr_t _address, uint8_t _network_ID,
        uint8_t _irq_pin, boolean _high_power, uint8_t _freq) {
  init(_address, _network_ID, _irq_pin, _high_power, _freq);
}

void RFM69Socket::init(socket_addr_t _address, uint8_t _network_ID,
                       uint8_t _irq_pin, boolean _high_power, uint8_t _freq) {

  sourceAddress = _address;
  currentMsgID = 0;

  byte irq_num = INT_FROM_PIN(_irq_pin);
  radio = new RFM69(RF69_SPI_CS, _irq_pin, _high_power, irq_num);

  // XXX: Should network ID be the top byte of the address?
  byte radio_address = (byte)(_address & 0xFF);
  radio->initialize(_freq, radio_address, _network_ID);

  setEncryptionKey(null);

  DEBUG4_VALUE("RFM69Socket:", radio_address);
  DEBUG4_VALUE(" net:", _network_ID);
  DEBUG4_VALUE(" freq:", _freq);
  DEBUG4_VALUE(" hw:", _high_power);
  DEBUG4_VALUE(" irq:", _irq_pin);
  DEBUG4_VALUELN(" irq#:", irq_num);
}

/*
 * Enable encryption with the indicated encryption key
 *
 * Note: EncryptionKey must be 16 bytes
 */
void RFM69Socket::setEncryptionKey(const char* key) {
  radio->encrypt(key);
}

/* Return true if the socket has been initialized */
boolean RFM69Socket::initialized() {
  return (radio != NULL);
}

void RFM69Socket::setup() {
  // TODO: Anything else need to be setup in order to start receive/transmit?
}

byte *RFM69Socket::initBuffer(byte *data, uint16_t data_size) {
  if (data_size > RF69_MAX_DATA_LEN) {
    DEBUG_ERR("sndbuf too big");
    DEBUG_ERR_STATE(RFM_OVERSIZED_SEND_BUFFER);
  }
  send_data_size = data_size;
  send_buffer = data + sizeof (rfm69_socket_hdr_t);
  return send_buffer;
}

#if DEBUG_LEVEL >= DEBUG_TRACE
void printSocketMsg(const rfm69_socket_msg_t *msg, byte length) {
  DEBUG4_HEXVAL(" id:",  msg->hdr.ID);
  DEBUG4_HEXVAL(" source:", msg->hdr.source);
  DEBUG4_HEXVAL(" addr:", msg->hdr.address);
  DEBUG4_HEXVAL(" flags:", msg->hdr.flags);
  DEBUG4_PRINT(" data:");
  print_hex_buffer((char *)msg->data, length);
}
#endif

void RFM69Socket::sendMsgTo(uint16_t address, const byte * data, const byte datalength) {
  rfm69_socket_msg_t *msg = (rfm69_socket_msg_t *)headerFromData(data);

  msg->hdr.ID = currentMsgID++;
  msg->hdr.source = sourceAddress; // TODO: 8-byte version is already in RFM69 code?
  msg->hdr.address = address;
  msg->hdr.flags = 0;

#ifdef RFM69_SOCKET_BROADCAST
  // Broadcast all messages
  byte send_address = RF69_BROADCAST_ADDR;
#else
  // Send to the lower byte of the socket address
  byte send_address = (byte)(address & 0xFF);
#endif

#if DEBUG_LEVEL == DEBUG_TRACE
  DEBUG5_VALUE("-> XMIT:", datalength);
  DEBUG5_VALUE(" addr:", send_address);
  printSocketMsg(msg, datalength);
  DEBUG_PRINT_END();
#endif

  radio->send(send_address, msg, RFM69_BUFFER_TOTAL(datalength),
              false /*ACKREQ*/);
}

const byte *RFM69Socket::getMsg(unsigned int *retlen) {
  return getMsg(sourceAddress, retlen);
}

const byte *RFM69Socket::getMsg(socket_addr_t address, unsigned int *retlen) {
  if (radio->receiveDone()) {
    const rfm69_socket_msg_t *msg = (rfm69_socket_msg_t *)&(radio->DATA[0]);

#if DEBUG_LEVEL == DEBUG_TRACE
    if (getLength() < sizeof (rfm69_socket_hdr_t)) {
      DEBUG_ERR("ERROR-length < header");
      goto ERROR_OUT;
    }

    DEBUG5_VALUE("<- RCV:", radio->DATALEN);
    DEBUG5_VALUE(" from:", radio->SENDERID);
    printSocketMsg(msg, RFM69_DATA_LENGTH(getLength()));
    DEBUG_PRINT_END();
#endif

    if (SOCKET_ADDRESS_MATCH(address,
                             RFM69_BROADCAST_CONVERT(msg->hdr.address))) {
      *retlen = RFM69_DATA_LENGTH(getLength());
      return &(msg->data[0]);
    } else {
      DEBUG5_VALUE("Not for me: ", address);
      DEBUG5_VALUELN(" to:", msg->hdr.address);
    }
  }

#if DEBUG_LEVEL >= DEBUG_TRACE
  ERROR_OUT:
#endif

  *retlen = 0;
  return NULL;
}

byte RFM69Socket::getLength() {
  return radio->DATALEN;
}

void *RFM69Socket::headerFromData(const void *data) {
  return ((rfm69_socket_hdr_t *)((long)data - sizeof (rfm69_socket_hdr_t)));
}

socket_addr_t RFM69Socket::sourceFromData(void *data) {
  return ((rfm69_socket_hdr_t *)headerFromData(data))->source;
}

socket_addr_t RFM69Socket::destFromData(void *data) {
  return ((rfm69_socket_hdr_t *)headerFromData(data))->address;
}


