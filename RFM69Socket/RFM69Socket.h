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
  socket_addr_t source; // TODO: Are these necessary?
  socket_addr_t address;
  byte flags;
} rfm69_socket_hdr_t;
/* Calculate the total buffer size with a useable buffer of size x */
#define RFM69_BUFFER_TOTAL(x) (uint8_t)(x + sizeof (rfm69_socket_hdr_t))
#define RFM69_DATA_LENGTH(x) (uint8_t)(x - sizeof (rfm69_socket_hdr_t))

typedef struct {
  rfm69_socket_hdr_t hdr;
  byte              data[];
} rfm69_socket_msg_t;


class RFM69Socket : public Socket {

public:
  RFM69Socket();
  RFM69Socket(socket_addr_t _address, uint8_t _network_ID,
             uint8_t _irq_pin, boolean _high_power, uint8_t _freq);
  void init(socket_addr_t _address, uint8_t _network_ID,
            uint8_t _irq_pin, boolean _high_power, uint8_t _freq);
  void setup();
  boolean initialized();
  byte * initBuffer(byte * data, uint16_t data_size);

  void setEncryptionKey(const char* key);

  void sendMsgTo(uint16_t address, const byte * data, const byte length);
  const byte *getMsg(uint16_t address, unsigned int *retlen);

  byte getLength();
  virtual byte getLength();
  void *headerFromData(const void *data);
  socket_addr_t sourceFromData(void *data);
  socket_addr_t destFromData(void *data);

private:
  RFM69 *radio;

}

#endif
