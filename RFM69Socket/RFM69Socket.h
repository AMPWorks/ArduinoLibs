/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2014
 *
 * Socket wrapper of RFM69 code:
 *   https://github.com/LowPowerLab/RFM69
 *******************************************************************************/

#ifndef RM69SOCKET_H
#define RM69SOCKET_H

#include "Arduino.h"
#include "Socket.h"
#include <RFM69.h>

typedef struct {
  byte ID;
  socket_addr_t source; // TODO: Are these necessary?
  socket_addr_t address;
  byte flags;
} rfm69_socket_hdr_t;
/* Calculate the total buffer size with a useable buffer of size x */
#define RFM69_BUFFER_TOTAL(x) (uint8_t)(x + sizeof (rfm69_socket_hdr_t))
#define RFM69_DATA_LENGTH(x) (uint8_t)(x - sizeof (rfm69_socket_hdr_t))

/*
 * Convert an address such that the RFM69.h broadcast address is a Socket.h
 * broadcast address.
 */
#define RFM69_BROADCAST_CONVERT(x) ((x == RF69_BROADCAST_ADDR) ? SOCKET_ADDR_ANY : x)

typedef struct {
  rfm69_socket_hdr_t hdr;
  byte              data[];
} rfm69_socket_msg_t;


class RFM69Socket : public Socket {

public:
  /* RFM69 specific functions */
  RFM69Socket();
  RFM69Socket(socket_addr_t _address, uint8_t _network_ID,
             uint8_t _irq_pin, boolean _high_power, uint8_t _freq);
  void init(socket_addr_t _address, uint8_t _network_ID,
            uint8_t _irq_pin, boolean _high_power, uint8_t _freq);

  void setEncryptionKey(const char* key);

  /*
   * Implement functions from Socket.h
   */
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

private:
  RFM69 *radio;
  byte currentMsgID;
};

#endif
