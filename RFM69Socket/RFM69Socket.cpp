/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2014
 *
 * Socket wrapper of RFM69 code
 */

#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL DEBUG_NONE
#endif
#include "Debug.h"

#include <Arduino.h>
#include <RFM69.h>

#include "Socket.h"
#include "RFM69Socket.h"

// Debug error codes
#define RFM_OVERSIZED_SEND_BUFFER 1

// TODO: This needs to choose the correct interrupt # based on processor and pin
#define INT_FROM_PIN(pin) (2)

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

  radio = new RFM69(SS, _irq_pin, _high_power, INT_FROM_PIN(_irq_pin));

  // XXX: Should network ID be the top byte of the address?
  radio->initialize(_freq, _address, _network_ID);
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

void RFM69Socket::sendMsgTo(uint16_t address, const byte * data, const byte datalength) {
  rfm69_socket_msg_t *msg = (rfm69_socket_msg_t *)headerFromData(data);

  msg->hdr.source = sourceAddress; // TODO: These are already in RFM69 code!!!
  msg->hdr.address = address;
   msg->hdr.flags = 0;

  radio->send(address, data, RFM69_BUFFER_TOTAL(datalength), false /*ACKREQ*/);
}

const byte *RFM69Socket::getMsg(uint16_t address, unsigned int *retlen) {
  *retlen = radio->DATALEN;
  return &(radio->DATA[0]);
  XXX - What to do here?
}

byte RFM69Socket::getLength() {
  return radio->DATALEN;
}

virtual void *RFM69Socket::headerFromData(const void *data) {
  return ((rfm69_socket_hdr_t *)((long)data - sizeof (rfm69_socket_hdr_t)));
}

socket_addr_t RFM69Socket::sourceFromData(void *data) {
  return radio->SENDERID; // TODO: If we get network from address, add network!
}

socket_addr_t RFM69Socket::destFromData(void *data) {
  return radio->TARGETID; // TODO: If we get network from address, add network!
}



