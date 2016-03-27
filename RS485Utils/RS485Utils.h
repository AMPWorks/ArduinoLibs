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

//#define RS485_HARDWARE_SERIAL
#ifdef RS485_HARDWARE_SERIAL
#define SERIAL_TYPE HardwareSerial
#else
#define SERIAL_TYPE SoftwareSerial
#endif


// "Broadcast" address
#define RS485_ADDR_ANY SOCKET_ADDR_ANY

// "Invalid address"
#define RS485_ADDR_INVALID SOCKET_ADDR_INVALID

typedef struct {
  byte     ID;
  byte     length;
	socket_addr_t source;
	socket_addr_t address;
  byte     flags;
} rs485_socket_hdr_t;

/* Calculate the total buffer size with a useable buffer of size x */
#define RS485_BUFFER_TOTAL(x) (x + sizeof (rs485_socket_hdr_t))

typedef struct {
  rs485_socket_hdr_t hdr;
  byte               data[];
} rs485_socket_msg_t;

// Get the socket header from the data portion of a message
#define RS485_HDR_FROM_DATA(x) ((rs485_socket_hdr_t *)((long)x - sizeof (rs485_socket_hdr_t))) // DEPRECATED

// Get the source address from the data portion of a message
#define RS485_SOURCE_FROM_DATA(x) (RS485_HDR_FROM_DATA(x)->source) // DEPRECATED

// Get the destination address from the data portion of a message
#define RS485_ADDRESS_FROM_DATA(x) (RS485_HDR_FROM_DATA(x)->address) // DEPRECATED

void printSocketMsg(const rs485_socket_msg_t *msg);
void printBuffer(const byte *buff, int length);

class RS485Socket : public Socket
{
  public:
  RS485Socket();
	RS485Socket(SERIAL_TYPE *_serial, byte _enablePin, socket_addr_t _address);
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, socket_addr_t _address);
  RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin, socket_addr_t _address,
	      boolean debug);

	void init(SERIAL_TYPE *_serial, byte _enablePin, socket_addr_t _address,
						byte _recvsize, boolean debug);
  void init(byte _recvPin, byte _xmitPin, byte _enablePin, socket_addr_t _address,
	    byte _recvsize, boolean _debug);

  void setup();
  byte * initBuffer(byte * data, uint16_t data_size);
  byte * initBuffer(byte * data);

  void sendMsgTo(socket_addr_t address, const byte * data, const byte length);
  const byte *getMsg(socket_addr_t address, unsigned int *retlen);
  const byte *getMsg(unsigned int *retlen);
  byte getLength();
  void *headerFromData(const void *data);
  socket_addr_t sourceFromData(void *data);
  socket_addr_t destFromData(void *data);

  boolean initialized();

  byte recvLimit;
	socket_addr_t sourceAddress;

 private:
  byte enablePin;
  byte currentMsgID;

  RS485 *channel;

  static size_t serialWrite(const byte what);
  static size_t serialDebugWrite(const byte what);
  static int serialRead();
  static int serialDebugRead();
  static int serialAvailable();

	void init_general(SERIAL_TYPE *_serial,  byte _enablePin,
										socket_addr_t _address, byte _recvsize,
										boolean _debug);
};

#endif // RS485UTILS_H
