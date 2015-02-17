/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2014
 *
 * Abstract socket class
 */
#ifndef SOCKET_H
#define SOCKET_H

class Socket {
 public:

  virtual void setup();

  virtual boolean initialized();

  virtual byte * initBuffer(byte * data);

  virtual void sendMsgTo(uint16_t address, const byte * data, const byte length);
  virtual const byte *getMsg(uint16_t address, unsigned int *retlen);

  virtual byte getLength();

  byte recvLimit;
  uint16_t sourceAddress;
};


#endif
