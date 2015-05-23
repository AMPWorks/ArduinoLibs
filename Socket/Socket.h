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

class Socket {
 public:

  virtual void setup();

  virtual boolean initialized();

  virtual byte * initBuffer(byte * data);

  virtual void sendMsgTo(uint16_t address, const byte * data, const byte length);
  virtual const byte *getMsg(unsigned int *retlen);
  virtual const byte *getMsg(uint16_t address, unsigned int *retlen);

  virtual byte getLength();

  byte recvLimit;
  uint16_t sourceAddress;
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
