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
 * CRC checks a lighter-weight protocol could be used to acheive higher line
 * speeds.
 ******************************************************************************/

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

//#define DEBUG_LEVEL DEBUG_HIGH
#include "Debug.h"

#include "RS485Utils.h"

// XXX: Need to figure out how to use non-static function pointers so this
//      can be a class field.
SoftwareSerial *serial;

RS485Socket::RS485Socket() {
  enablePin = 0;
  serial = NULL;
  channel = NULL;
}

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, 
			 uint16_t _address) 
{
  serial = NULL;
  init(_recvPin, _xmitPin, _enablePin, _address, RS485_RECV_BUFFER, false);
}

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, 
			 uint16_t _address, boolean _debug) 
{
  serial = NULL;
  init(_recvPin, _xmitPin, _enablePin, _address, RS485_RECV_BUFFER, _debug);
}

void RS485Socket::init(byte _recvPin, byte _xmitPin, byte _enablePin,
		       uint16_t _address, byte _recvsize, boolean _debug) {
  if (serial != NULL) {
    DEBUG_ERR("RS485Socket::init already initialized");
    DEBUG_ERR_STATE(DEBUG_ERR_REINIT);
    // XXX - Could re-init the config?
  } else {
    enablePin = _enablePin;
    sourceAddress = _address;
    recvLimit = _recvsize;

    serial = new SoftwareSerial(_recvPin, _xmitPin);
#if DEBUG_LEVEL == DEBUG_HIGH
    if (_debug) {
      channel = new RS485(serialDebugRead, serialAvailable, serialDebugWrite,
			  recvLimit);
    } else
#endif
    {
      channel = new RS485(serialRead, serialAvailable, serialWrite,
			  recvLimit);
    }

    currentMsgID = 0;
  }
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

  serial->begin(28800); // XXX - Could this be a higher rate?

  channel->begin();
  
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW);
}


size_t RS485Socket::serialWrite(const byte value) 
{
  return serial->write(value);
}

int RS485Socket::serialRead() 
{
  return serial->read();
}

#if (DEBUG_LEVEL == DEBUG_HIGH)
size_t RS485Socket::serialDebugWrite(const byte value) 
{
  DEBUG4_HEX(value);
  DEBUG4_PRINT(" ");
  return serial->write(value);
}

int RS485Socket::serialDebugRead() 
{
  int value = serial->read();
  DEBUG4_HEX(value);
  DEBUG4_PRINT(" ");
  return value;
}
#endif

int RS485Socket::serialAvailable() 
{
  return serial->available();
}


byte * RS485Socket::initBuffer(byte * data) 
{
  return data + sizeof (rs485_socket_hdr_t);
}

void RS485Socket::sendMsgTo(uint16_t address,
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
  DEBUG5_VALUE("XMIT:", msg_len);
  DEBUG5_PRINT(" ");
  printSocketMsg(msg);
  DEBUG5_PRINT(" raw:");
#endif

  digitalWrite(enablePin, HIGH);
  channel->sendMsg((byte *)msg, msg_len);
  digitalWrite(enablePin, LOW);

#if DEBUG_LEVEL ==DEBUG_TRACE
  DEBUG_PRINT_END();
#endif
}

const byte *RS485Socket::getMsg(unsigned int *retlen) {
  return getMsg(sourceAddress, retlen);
}

const byte *RS485Socket::getMsg(uint16_t address, unsigned int *retlen) 
{
  if (channel->update()) {

#if DEBUG_LEVEL == DEBUG_TRACE
    DEBUG5_VALUE("getMsg:", getLength());
#endif

    const rs485_socket_msg_t *msg = (rs485_socket_msg_t *)channel->getData();

#if DEBUG_LEVEL == DEBUG_TRACE
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

 ERROR_OUT:

  *retlen = 0;
  return NULL;
}

byte RS485Socket::getLength()
{
  return channel->getLength();
}

#if DEBUG_LEVEL >= DEBUG_HIGH
void printSocketMsg(const rs485_socket_msg_t *msg) 
{
  DEBUG4_VALUE( "i:",  msg->hdr.ID);
  DEBUG4_VALUE( " l:", msg->hdr.length);
  DEBUG4_VALUE( " s:", msg->hdr.source);
  DEBUG4_VALUE( " a:", msg->hdr.address);
  DEBUG4_HEXVAL( " f:", msg->hdr.flags);
  DEBUG4_PRINT(" data:");
printBuffer(msg->data, msg->hdr.length);
}

void printBuffer(const byte *buff, int length) 
{
  for (int b = 0; b < length; b++) {
    DEBUG4_HEXVAL( " ", buff[b]);
  }
}
#endif
