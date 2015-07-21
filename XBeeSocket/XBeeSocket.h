/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2015
 *
 * XBee implementation of the socket class
 */

#ifndef XBEESOCKET_H
#define XBEESOCKET_H

#include "Arduino.h"
#include "Xbee.h"
#include "Socket.h"

typedef struct {
  socket_addr_t source;
  socket_addr_t address;
  byte flags;
} xbee_socket_hdr_t;

/* Calculate the total buffer size with a useable buffer of size x */
#define XBEE_BUFFER_TOTAL(x) (uint8_t)(x + sizeof (xbee_socket_hdr_t))
#define XBEE_DATA_LENGTH(x) (uint8_t)(x - sizeof (xbee_socket_hdr_t))

typedef struct {
  xbee_socket_hdr_t hdr;
  byte              data[];
} xbee_socket_msg_t;

class XBeeSocket : public Socket {
 public:
  XBeeSocket();
  XBeeSocket(XBee *xbee, socket_addr_t _address);
  void init(XBee *xbee, socket_addr_t _address);
  void setup();
  boolean initialized();

  byte * initBuffer(byte * data, uint16_t data_size);

  void sendMsgTo(uint16_t address, const byte * data, const byte length);
  const byte *getMsg(unsigned int *retlen);
  const byte *getMsg(uint16_t address, unsigned int *retlen);
  byte getLength();
  void *headerFromData(const void *data);
  socket_addr_t sourceFromData(void *data);
  socket_addr_t destFromData(void *data);

  byte recvLimit;
  uint16_t sourceAddress;

 private:

  XBee *xbee;
  ZBRxResponse rx;

};

#endif
