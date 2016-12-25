/*******************************************************************************
 * Author: Adam Phelps
 * License: MIT
 * Copyright: 2015
 *
 * XBee implementation of the socket class
 */

// By default the XBee uses the serial interface and so DEBUG data can't
// be sent to it.  This is included for debug error conditions.
#ifndef DEBUG_LEVEL
  #define DEBUG_LEVEL DEBUG_NONE
#endif
#include "Debug.h"

#include "XBee.h"
#include "XBeeSocket.h"


// Debug error codes
#define XBS_NO_XBEE         1
#define XBS_ALREADY_INITED  2
#define XBS_NOT_INITIALIZED 3

XBeeSocket::XBeeSocket() {
  xbee = NULL;
}

XBeeSocket::XBeeSocket(XBee *_xbee,  socket_addr_t _address) {
  init(_xbee, _address);
}

void XBeeSocket::init(XBee *_xbee, socket_addr_t _address) {
  if (_xbee == NULL) {
    DEBUG_ERR_STATE(XBS_NO_XBEE);
  } else if (xbee != NULL) {
    DEBUG_ERR_STATE(XBS_ALREADY_INITED);
  } else {
    sourceAddress = _address;
    xbee = _xbee;
  }
}

boolean XBeeSocket::initialized() {
  return (xbee != NULL);
}

void XBeeSocket::setup() {
  if (!initialized()) {
    DEBUG_ERR_STATE(XBS_NOT_INITIALIZED);
  }

  // TODO: Allow for SoftwareSerial
  xbee->begin(Serial);

  rx = ZBRxResponse();
}

/*
 * Initialize a data buffer.  The actual buffer should be at least a header
 * larger than the specified data_size.
 */
byte *XBeeSocket::initBuffer(byte * data, uint16_t data_size) 
{
  // TODO: Do validation of maximum packet size?
  send_data_size = data_size;
  send_buffer = data + sizeof (xbee_socket_hdr_t);
  return send_buffer;
}


void XBeeSocket::sendMsgTo(uint16_t address,
                           const byte *data,
                           const byte datalength) 
{
  xbee_socket_msg_t *msg = (xbee_socket_msg_t *)headerFromData(data);

  // TODO: Should this be using the broadcast address or something else?
  XBeeAddress64 addr = XBeeAddress64(0x0, 0xFFFF);
  ZBTxRequest tx_data = ZBTxRequest(addr, (uint8_t *)msg, 
                                    XBEE_BUFFER_TOTAL(datalength));

  msg->hdr.source = sourceAddress;
  msg->hdr.address = address;
  msg->hdr.flags = 0;
  
  xbee->send(tx_data);
}

const byte *XBeeSocket::getMsg(unsigned int *retlen)  {
  return getMsg(sourceAddress, retlen);
}

const byte *XBeeSocket::getMsg(uint16_t address, unsigned int *retlen)  {

  // Read data if available
  xbee->readPacket();

  if (xbee->getResponse().isAvailable()) {
    // Data was available, do something with the response

    if (xbee->getResponse().getApiId() == ZB_RX_RESPONSE) {

      // Populate the response data structure
      xbee->getResponse().getZBRxResponse(rx);

#if 0 // TODO: Handle acks
      // Check for ACK info
      if (rx.getOption() == ZB_PACKET_ACKNOWLEDGED) {
        // the sender got an ACK
      } else {
        // we got it (obviously) but sender didn't get an ACK
      }
#endif

      const xbee_socket_msg_t *msg = (xbee_socket_msg_t *)rx.getData();
      unsigned int datalength = XBEE_DATA_LENGTH(rx.getDataLength());

#if 1 //DEBUG_LEVEL == DEBUG_TRACE
      if (rx.getDataLength() < sizeof (xbee_socket_msg_t)) {
        DEBUG_ERR("ERROR-length < header");
        goto ERROR_OUT;
      }

      if (rx.getDataLength() < (sizeof (xbee_socket_hdr_t) + datalength)) {
        DEBUG_ERR("ERROR-length < header + data");
        goto ERROR_OUT;
      }
#endif

      if (SOCKET_ADDRESS_MATCH(address, msg->hdr.address)) {
        *retlen = datalength;
        return &msg->data[0];
      }
    } else {
      // TODO: Some other message types
    }
  }

 ERROR_OUT:
  *retlen = 0;
  return NULL;
}

byte XBeeSocket::getLength()
{
  return rx.getDataLength();
}

void *XBeeSocket::headerFromData(const void *data) {
  return ((xbee_socket_hdr_t *)((long)data - sizeof (xbee_socket_hdr_t)));
}

socket_addr_t XBeeSocket::sourceFromData(void *data) {
  return ((xbee_socket_hdr_t *)headerFromData(data))->source;
}

socket_addr_t XBeeSocket::destFromData(void *data) {
  return ((xbee_socket_hdr_t *)headerFromData(data))->address;
}

