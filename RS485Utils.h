#ifndef RS485UTILS_H
#define RS485UTILS_H

#include "Arduino.h"

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

#define RS485_RECV_BUFFER 256

class RS485Socket 
{
  public:
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin);
  void setup();

  void sendMsgTo(byte address, const byte * data, const byte length);
  const byte *getMsg(byte address);
  int getLength();

  private:
  byte recvPin, xmitPin, enablePin;

  RS485 *channel;

  static size_t serialWrite(const byte what);
  static int serialRead();
  static int serialAvailable();
};

#endif
