#ifndef RS485UTILS_H
#define RS485UTILS_H

#include "Arduino.h"

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

#define RS485_RECV_BUFFER 64 // XXX: This is a lot of buffer space

#define RS485_ADDR_ANY (uint16_t)-1

typedef struct {
  byte ID;
  byte length;
  uint16_t address;
  byte flags;
} rs485_socket_hdr_t;

/* Calculate the total buffer size with a useable buffer of size x */
#define RS485_BUFFER_TOTAL(x) (x + sizeof (rs485_socket_hdr_t))

typedef struct {
  rs485_socket_hdr_t hdr;
  byte               data[];
} rs485_socket_msg_t;

#define RS485_HDR_FROM_DATA(x) ((rs485_socket_hdr_t *)((long)x - sizeof (rs485_socket_hdr_t)))

void printSocketMsg(const rs485_socket_msg_t *msg);
void printBuffer(const byte *buff, int length);

class RS485Socket 
{
  public:
  RS485Socket();
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin);
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, boolean debug);
  void init(byte _recvPin, byte _xmitPin, byte _enablePin, boolean _debug);

  void setup();
  byte * initBuffer(byte * data);

  void sendMsgTo(uint16_t address, const byte * data, const byte length);
  const byte *getMsg(uint16_t address, unsigned int *retlen);
  byte getLength();

  boolean initialized;

 private:
  byte recvPin, xmitPin, enablePin;
  byte currentMsgID;
  boolean debug;

  RS485 *channel;

  static size_t serialWrite(const byte what);
  static size_t serialDebugWrite(const byte what);
  static int serialRead();
  static int serialDebugRead();
  static int serialAvailable();
};

#endif
