
#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

#include "RS485Utils.h"

SoftwareSerial *serial;

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin) 
{
  recvPin = _recvPin;
  xmitPin = _xmitPin;
  enablePin = _enablePin;

  serial = new SoftwareSerial(recvPin, xmitPin);
  channel = new RS485(serialRead, serialAvailable, serialWrite, RS485_RECV_BUFFER);
}


void RS485Socket::setup() 
{
  
  serial->begin(28800);
  channel->begin();
  
  pinMode(enablePin, OUTPUT);

  digitalWrite(enablePin, LOW);
}

size_t RS485Socket::serialWrite(const byte what) 
{
  Serial.print("write:");
  Serial.println(what);
  return serial->write(what);
}

int RS485Socket::serialRead() 
{
  return serial->read();
}

int RS485Socket::serialAvailable() 
{
  return serial->available();
}

void RS485Socket::sendMsgTo(byte address, const byte * data, const byte length) 
{
  // XXX: Add address data
  Serial.print("XMIT:");
  Serial.print(*data);
  Serial.print("-");
  Serial.println(length);

  digitalWrite(enablePin, HIGH);
  channel->sendMsg(data, length);
  digitalWrite(enablePin, LOW);
}

const byte *RS485Socket::getMsg(byte address) 
{
  if (channel->update()) {
//    int datalen = channel->getLength();
    const byte *data = channel->getData();

    // XXX: Check if message destination matches address

    return data;
  } else {
    return NULL;
  }
}

int RS485Socket::getLength() 
{
  return channel->getLength();
}
