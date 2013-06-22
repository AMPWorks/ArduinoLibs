
#include <RS485_non_blocking.h>
#include <SoftwareSerial.h>

#include "RS485Utils.h"

// XXX: Need to figure out how to use non-static function pointers so this
//      can be a class field.
SoftwareSerial *serial;

RS485Socket::RS485Socket(byte _recvPin, byte _xmitPin, byte _enablePin) 
{
  recvPin = _recvPin;
  xmitPin = _xmitPin;
  enablePin = _enablePin;

  serial = new SoftwareSerial(recvPin, xmitPin);
  channel = new RS485(serialRead, serialAvailable, serialWrite, RS485_RECV_BUFFER);

  currentMsgID = 0;
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
//  Serial.print("write:");
//  Serial.println(what);
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


byte * RS485Socket::initBuffer(byte * data) 
{
  return data + sizeof (rs485_socket_hdr_t);
}

void RS485Socket::sendMsgTo(byte address,
                            const byte *data,
                            const byte datalength) 
{
  rs485_socket_msg_t *msg =
    (rs485_socket_msg_t *)(data - sizeof (rs485_socket_hdr_t));

  msg->hdr.msgID = currentMsgID++;
  msg->hdr.length = datalength + sizeof (rs485_socket_hdr_t);
  msg->hdr.address = address;
  msg->hdr.flags = 0;

  Serial.print("XMIT:");
  Serial.print(*data);
  Serial.print("-");
  Serial.print(datalength);
  Serial.print(" ");
  Serial.println(msg->hdr.length);

  digitalWrite(enablePin, HIGH);
  channel->sendMsg((byte *)msg, msg->hdr.length);
  digitalWrite(enablePin, LOW);
}

const byte *RS485Socket::getMsg(byte address) 
{
  if (channel->update()) {
    const rs485_socket_msg_t *msg = (rs485_socket_msg_t *)channel->getData();

    Serial.print("RCV:");
    Serial.println(channel->getLength());

//    if (msg->hdr.address == address) {
      return &msg->data[0];
//    }

  }
  
  return NULL;
}

int RS485Socket::getLength() 
{
  return channel->getLength();
}
