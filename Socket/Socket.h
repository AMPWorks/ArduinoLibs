/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2014
 *
 * Abstract socket class
 */
#ifndef SOCKET_H
#define SOCKET_H

typedef uint16_t socket_addr_t;
#define SOCKET_ADDR_ANY (socket_addr_t)-1
#define SOCKET_ADDR_INVALID (socket_addr_t)-2

class Socket {
 public:

  virtual void setup();

  virtual boolean initialized();

  virtual byte * initBuffer(byte * data, uint16_t data_size);

  virtual void sendMsgTo(uint16_t address, const byte * data, const byte length);
  virtual const byte *getMsg(unsigned int *retlen);
  virtual const byte *getMsg(uint16_t address, unsigned int *retlen);

  virtual byte getLength();
  virtual void *headerFromData(const void *data);
  virtual socket_addr_t sourceFromData(void *data);
  virtual socket_addr_t destFromData(void *data);

  byte recvLimit;
  uint16_t sourceAddress;
  byte *send_buffer;
  byte send_data_size;
};

/*
 * Determine if two addresses are considered "matching" for purpose of
 * retrieving messages.  If they match or either is the broadcast address
 * then they "match".
 */
#define SOCKET_ADDRESS_MATCH(x, y) (                                    \
                                    (x == SOCKET_ADDR_ANY) ||           \
                                    (y == SOCKET_ADDR_ANY) ||           \
                                    (x == y)                            \
                                   )

#endif
