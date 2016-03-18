/*******************************************************************************
 * Author: Adam Phelps
 * License: Create Commons Attribution-Share-Alike
 * Copyright: 2014
 *
 * Socket implementation on top of Low Power Lab's RF69 library:
 *     https://github.com/LowPowerLab/RFM69
 *
 *******************************************************************************/
#ifndef RM69SOCKET_H
#define RM69SOCKET_H

#include "Arduino.h"
#include "Socket.h"

class RM69Socket : public Socket {
  void setup();

  boolean initialized();

  byte * initBuffer(byte * data);

  void sendMsgTo(uint16_t address, const byte * data, const byte length);
  const byte *getMsg(uint16_t address, unsigned int *retlen);

  byte getLength();
}

#endif
