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

#ifndef RS485UTILS_H
#define RS485UTILS_H

#include "Arduino.h"
#include "Socket.h"

#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

// TODO: Should be specified by the sketch that creates the socket rather than
// being statically defined
#define RS485_RECV_BUFFER 64 // 140 // XXX: This is a lot of buffer space

// "Broadcast" address
#define RS485_ADDR_ANY (uint16_t)-1

typedef struct {
  byte     ID;
  byte     length;
  uint16_t source;
  uint16_t address;
  byte     flags;
} rs485_socket_hdr_t;

/* Calculate the total buffer size with a useable buffer of size x */
#define RS485_BUFFER_TOTAL(x) (x + sizeof (rs485_socket_hdr_t))

typedef struct {
  rs485_socket_hdr_t hdr;
  byte               data[];
} rs485_socket_msg_t;

// Get the socket header from the data portion of a message
#define RS485_HDR_FROM_DATA(x) ((rs485_socket_hdr_t *)((long)x - sizeof (rs485_socket_hdr_t)))

// Get the source address from the data portion of a message
#define RS485_SOURCE_FROM_DATA(x) (RS485_HDR_FROM_DATA(x)->source)

void printSocketMsg(const rs485_socket_msg_t *msg);
void printBuffer(const byte *buff, int length);

class RS485Socket : public Socket
{
  public:
  RS485Socket();
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, uint16_t _address);
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, uint16_t _address,
	      boolean debug);
  void init(byte _recvPin, byte _xmitPin, byte _enablePin, uint16_t _address,
	    byte _recvsize, boolean _debug);

  void setup();
  byte * initBuffer(byte * data);

  void sendMsgTo(uint16_t address, const byte * data, const byte length);
  const byte *getMsg(uint16_t address, unsigned int *retlen);
  byte getLength();

  boolean initialized();

  byte recvLimit;
  uint16_t sourceAddress;

 private:
  byte enablePin;
  byte currentMsgID;

  RS485 *channel;

  static size_t serialWrite(const byte what);
  static size_t serialDebugWrite(const byte what);
  static int serialRead();
  static int serialDebugRead();
  static int serialAvailable();
};

#endif // RS485UTILS_H
