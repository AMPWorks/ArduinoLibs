/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2014
 *
 * This class provides a socket-like API for communicating via RS485.
 *
 * This relies on Nick Gammon's RS485 non-blocking protocol library to provide
 * the underlying transport-layer protocol. 
 * (See http://www.gammon.com.au/forum/?id=11428)
 *
 * TODO: With the addition of socket-level
 * CRC checks a lighter-weight protocol could be used to achieve higher line
 * speeds.
 ******************************************************************************/

#include <RS485_non_blocking.h>

#ifdef __AVR__
#include <SoftwareSerial.h>
#endif

#ifdef DEBUG_LEVEL_RS485UTILS
  #define DEBUG_LEVEL DEBUG_LEVEL_RS485UTILS
#endif

#ifndef DEBUG_LEVEL
//  #define DEBUG_LEVEL DEBUG_HIGH
#endif
#include "Debug.h"

#include "RS485Utils.h"

// XXX: Need to figure out how to use non-static function pointers so this
//      can be a class field.

SERIAL_TYPE *serial;

RS485Socket::RS485Socket() {
  enablePin = 0;
  serial = NULL;
  channel = NULL;
}

RS485Socket::RS485Socket(SERIAL_TYPE *_serial, byte _enablePin,
                         socket_addr_t _address) {
  serial = NULL;
  init(_serial, _enablePin, _address, RS485_RECV_BUFFER, false);
}

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin,
                         socket_addr_t _address)
{
  serial = NULL;
  init(_recvPin, _xmitPin, _enablePin, _address, RS485_RECV_BUFFER, false);
}

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin,
                         socket_addr_t _address, boolean _debug)
{
  serial = NULL;
  init(_recvPin, _xmitPin, _enablePin, _address, RS485_RECV_BUFFER, _debug);
}

void RS485Socket::init(SERIAL_TYPE *_serial, byte _enablePin,
                       socket_addr_t _address, byte _recvsize, boolean debug) {
  init_general(_serial, _enablePin, _address, _recvsize, debug);
}

void RS485Socket::init(byte _recvPin, byte _xmitPin, byte _enablePin,
                       socket_addr_t _address, byte _recvsize, boolean _debug) {
  if (serial != NULL) {
    DEBUG_ERR("RS485Socket::init already initialized");
    DEBUG_ERR_STATE(DEBUG_ERR_REINIT);
    // XXX - Could re-init the config?
  } else {
#ifdef RS485_HARDWARE_SERIAL

#if defined(ESP32)
    serial = new HardwareSerial(RS485_HARDWARE_SERIAL);
    serial->begin(DEFAULT_BAUD, SERIAL_8N1, _recvPin, _xmitPin);
#else
    serial = &RS485_HARDWARE_SERIAL;
#endif

#else
    serial = new SoftwareSerial(_recvPin, _xmitPin);
#endif
    init_general(serial, _enablePin, _address, _recvsize, _debug);
  }
}

void RS485Socket::init_general(SERIAL_TYPE *_serial, byte _enablePin,
                               socket_addr_t _address, byte _recvsize,
                               boolean _debug) {
  enablePin = _enablePin;
  sourceAddress = _address;
  recvLimit = _recvsize;
  serial = _serial;

  pinMode(enablePin, OUTPUT);
  channel = new RS485(serialRead, serialAvailable, serialWrite,
                      recvLimit);

  currentMsgID = 0;

}

boolean RS485Socket::initialized() {
  return (serial != NULL);
}

void RS485Socket::setup() 
{
  if (serial == NULL) {
    DEBUG_ERR("RS485Socket::setup called before initialized");
    DEBUG_ERR_STATE(DEBUG_ERR_UNINIT);
  }

  DEBUG5_VALUELN("RS485 setup: ", DEFAULT_BAUD);
#if !defined(ESP32)
  serial->begin(DEFAULT_BAUD);
#endif

  channel->begin();

  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW);
}


#if DEBUG_LEVEL == DEBUG_TRACE
size_t RS485Socket::serialWrite(const byte value) 
{
  DEBUG4_PRINT("_");
  if (value <= 0xF) {
    DEBUG4_HEX(0);
  }
  DEBUG4_HEX(value);

  return serial->write(value);
}

int RS485Socket::serialRead() 
{
  int value = serial->read();
  DEBUG4_PRINT("-");
  if (value <= 0xF) {
    DEBUG4_HEX(0);
  }
  DEBUG4_HEX(value);
  return value;
}
#else
size_t RS485Socket::serialWrite(const byte value)
{
  return serial->write(value);
}

int RS485Socket::serialRead()
{
  return serial->read();
}

#endif

int RS485Socket::serialAvailable() 
{
  return serial->available();
}

/*
 * Initialize a data buffer.  The actual buffer should be at least a header
 * larger than the specified data_size.
 */
byte * RS485Socket::initBuffer(byte * data, uint16_t data_size) 
{
  DEBUG4_VALUELN("initBuffer: size:", data_size);
  send_data_size = data_size; // XXX?
  send_buffer = data + sizeof (rs485_socket_hdr_t);
  return send_buffer;
}

byte * RS485Socket::initBuffer(byte * data) {
  DEBUG1_PRINTLN("initbuffer() deprecated");
  return initBuffer(data, 0);
}

void RS485Socket::sendMsgTo(socket_addr_t address,
                            const byte *data,
                            const byte datalength) 
{
  rs485_socket_msg_t *msg =
    (rs485_socket_msg_t *)(data - sizeof (rs485_socket_hdr_t));

  unsigned int msg_len = sizeof (rs485_socket_hdr_t) + datalength;

  msg->hdr.ID = currentMsgID++;
  msg->hdr.length = datalength;
  msg->hdr.source = sourceAddress;
  msg->hdr.address = address;
  msg->hdr.flags = 0;

#if DEBUG_LEVEL == DEBUG_TRACE
  DEBUG_ENDLN();
  DEBUG5_VALUE("XMIT:", msg_len);
  DEBUG5_PRINT(" ");
  printSocketMsg(msg);
  DEBUG_ENDLN()
#endif

  digitalWrite(enablePin, HIGH);
  channel->sendMsg((byte *)msg, msg_len);
#ifdef RS485_HARDWARE_SERIAL
  // XXX: A delay is required here with hardware serial, or checking registers based on the serial device: http://www.gammon.com.au/forum/?id=11428 or possibly use a timer to disable enablePin
  serial->flush(); // Block s until the send buffer is empty
#endif
  digitalWrite(enablePin, LOW);
}

const byte *RS485Socket::getMsg(unsigned int *retlen) {
  return getMsg(sourceAddress, retlen);
}

const byte *RS485Socket::getMsg(socket_addr_t address, unsigned int *retlen)
{
  if (channel->update()) {

#if DEBUG_LEVEL >= DEBUG_TRACE
    DEBUG_ENDLN()
    DEBUG5_VALUE("getMsg:", getLength());
#endif

    const rs485_socket_msg_t *msg = (rs485_socket_msg_t *)channel->getData();

#if DEBUG_LEVEL >= DEBUG_TRACE
    if (getLength() < sizeof (rs485_socket_hdr_t)) {
      DEBUG_ERR("ERROR-length < header");
      goto ERROR_OUT;
    }

    if (getLength() < (sizeof (rs485_socket_hdr_t) + msg->hdr.length)) {
      DEBUG_ERR("ERROR-length < header + data");
      goto ERROR_OUT;
    }

    DEBUG4_PRINT(" RECV: ");
    printSocketMsg(msg);
    DEBUG_PRINT_END();
#endif

    if (SOCKET_ADDRESS_MATCH(address, msg->hdr.address)) {
      *retlen = msg->hdr.length;
      return &msg->data[0];
    }
  }

#if DEBUG_LEVEL >= DEBUG_TRACE
 ERROR_OUT:
#endif

  *retlen = 0;
  return NULL;
}

byte RS485Socket::getLength()
{
  return channel->getLength();
}

void *RS485Socket::headerFromData(const void *data) {
  return ((rs485_socket_hdr_t *)((long)data - sizeof (rs485_socket_hdr_t)));
}

socket_addr_t RS485Socket::sourceFromData(void *data) {
  return ((rs485_socket_hdr_t *)headerFromData(data))->source;
}

socket_addr_t RS485Socket::destFromData(void *data) {
  return ((rs485_socket_hdr_t *)headerFromData(data))->address;
}


#if DEBUG_LEVEL >= DEBUG_TRACE
void printSocketMsg(const rs485_socket_msg_t *msg) 
{
  DEBUG4_VALUE( "i:",  msg->hdr.ID);
  DEBUG4_VALUE( " l:", msg->hdr.length);
  DEBUG4_VALUE( " s:", msg->hdr.source);
  DEBUG4_VALUE( " a:", msg->hdr.address);
  DEBUG4_HEXVAL( " f:", msg->hdr.flags);
  DEBUG4_PRINT(" data:");
  print_hex_buffer((char *)msg->data, msg->hdr.length);
}
#endif
