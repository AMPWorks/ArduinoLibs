/*
 * This tool sends intermitten heartbeats as well as tracking heartbeat messages
 * from other nodes.
 */

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SerialCLI.h>

#include <Debug.h>

#ifndef NODEID
#define NODEID        1
#endif

#ifndef NETWORKID
#define NETWORKID     100
#endif

//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY   RF69_433MHZ
//#define FREQUENCY   RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#define GATEWAYID     1 // NODEID of the Gateway
//*********************************************************************************************

#define SERIAL_BAUD   115200

#define LED_SEND    12
#define LED_RECEIVE 13

#ifdef ENABLE_ATC
RFM69_ATC radio;
#else
RFM69 radio;
#endif

//set to 'true' to sniff all packets on the same network
bool promiscuousMode = false;

void clihandler(char **tokens, byte numtokens);
SerialCLI serialcli(
        32,     // Max command length, this amount is allocated as a buffer
        clihandler // Function for handling tokenized commands
);

#define HEARTBEAT_MAGIC 0xefbeadde
typedef struct {
  uint32_t magic;
  uint32_t sequence;
  byte payload[32 - sizeof (char)*4 + sizeof (uint32_t)];
} heartbeat_msg_t;

heartbeat_msg_t heartbeat;

unsigned int heartbeat_period_ms = 1000;  // Period between heartbeats

void setup() {
  Serial.begin(SERIAL_BAUD);

  Serial.println("*** RFM69 Tool starting up ***");
  delay(1000);

#if FREQUENCY==RF69_433MHZ
  Serial.println("* Frequency: 433");
#endif
#if FREQUENCY==RF69_868MHZ
  Serial.println("* Frequency: 868");
#endif
#if FREQUENCY==RF69_915MHZ
  Serial.println("* Frequency: 915");
#endif

  Serial.print("* NodeID:");
  Serial.print(NODEID);
  Serial.print(" NetworkID:");
  Serial.println(NETWORKID);
  radio.initialize(FREQUENCY, NODEID, NETWORKID);

#ifdef IS_RFM69HW
  Serial.println("*** High power mode enabled");
  radio.setHighPower(); //only for RFM69HW!
#endif

#ifdef ENABLE_ATC
  Serial.println("*** RFM69_ATC Enabled (Auto Transmission Control)");
#endif

  radio.encrypt(ENCRYPTKEY);
  radio.promiscuous(promiscuousMode);

#ifdef ENABLE_ATC
  radio.enableAutoPower(-70);
#endif

  DEBUG1_PRINTLN("*** RFM69 Tool Initialized ***")
}

unsigned long last_heartbeat_ms = 0;

void loop() {
  serialcli.checkSerial();

  // Check for any received data
  if (radio.receiveDone())
  {
    DEBUG1_VALUE("Received from:", radio.SENDERID);
    DEBUG1_VALUE(" to:", radio.TARGETID);
    DEBUG1_VALUE(" size:", radio.DATALEN);
    DEBUG1_VALUE(" rssi:", radio.RSSI);

    // Check if the received data is a heartbeat message
    heartbeat_msg_t *msg = (heartbeat_msg_t *) radio.DATA;
    if ((radio.DATALEN == sizeof (heartbeat_msg_t)) &&
            (msg->magic == HEARTBEAT_MAGIC)) {
      DEBUG1_VALUE(" HB:", msg->sequence);
    } else {
      DEBUG1_PRINT(" Not HB");
    }

    for (byte i = 0; i < radio.DATALEN; i++) {
      DEBUG1_HEXVAL(" ", (byte)radio.DATA[i]);
    }

    DEBUG_ENDLN();

    Blink(LED_RECEIVE, 3);
  }

  // Send a new heartbeat message if the period has elapsed
  if (millis() - last_heartbeat_ms >= heartbeat_period_ms) {
    static uint32_t heartbeat_sequence = 0;

    last_heartbeat_ms = millis();

    heartbeat.magic = HEARTBEAT_MAGIC;
    heartbeat.sequence = heartbeat_sequence++;

    DEBUG1_VALUE("Heartbeat size:", sizeof(heartbeat_msg_t));
    DEBUG1_VALUE(" seq:", heartbeat.sequence);
    DEBUG1_PRINT(" data: ");
    for (byte i = 0; i < sizeof(heartbeat_msg_t); i++)
            DEBUG1_HEXVAL(" ", ((byte *)&heartbeat)[i]);

    unsigned long start_us = micros();
    radio.send(RF69_BROADCAST_ADDR, &heartbeat, sizeof(heartbeat_msg_t));
    Blink(LED_SEND, 3);
  }
}

void Blink(byte PIN, int DELAY_MS)
{
  pinMode(PIN, OUTPUT);
  digitalWrite(PIN,HIGH);
  delay(DELAY_MS);
  digitalWrite(PIN,LOW);
}

void print_usage() {
  Serial.print(F(
         "\n"
         "Usage:\n"
         "  h - print this help\n"
         "  r - read radio registers\n"
         "  E - Enable encryption\n"
         "  e - Disable encryption\n"
         "  p - Toggle promiscuous mode\n"
         "  t - Read temperature\n"
         "  d <num> - Set transmit period\n"
         "  a <addr> - Set this node's address\n"
         "  P <power> - Set the auto power\n"
 ));
}
void clihandler(char **tokens, byte numtokens) {
  switch (tokens[0][0]) {

    case 'h': {
      print_usage();
      break;
    }

    case 'r': {
      radio.readAllRegs();
      break;
    }

    case 'E': {
      Serial.println("Enabling encryption");
      radio.encrypt(ENCRYPTKEY);
      break;
    }

    case 'e': {
      Serial.println("Disabling encryption");
      radio.encrypt(null);
      break;
    }

    case 'p': {
      promiscuousMode = !promiscuousMode;
      radio.promiscuous(promiscuousMode);
      Serial.print("Promiscuous mode ");
      Serial.println(promiscuousMode ? "on" : "off");
      break;
    }

    case 't': {
      byte temperature = radio.readTemperature(-1); // -1 = user cal factor, adjust for correct ambient
      int fTemp = 1.8 * temperature + 32; // 9/5=1.8
      Serial.print("Radio Temp is ");
      Serial.print(temperature);
      Serial.print("C, ");
      Serial.print(fTemp); //converting to F loses some resolution, obvious when C is on edge between 2 values (ie 26C=78F, 27C=80F)
      Serial.println('F');
      break;
    }

    case 'd': {
      if (numtokens < 2) return;
      uint16_t value = atoi(tokens[1]);
      Serial.print("* Setting heartbeat period:");
      Serial.println(value);
      if ((value < 10) || (value > 10000)) {
        Serial.println("INVALID VALUE");
        return;
      }
      heartbeat_period_ms = value;
      break;
    }

    case 'a': {
      if (numtokens < 2) return;
      uint16_t value = atoi(tokens[1]);
      Serial.print("* Setting address:");
      Serial.println(value);
      if (value > 254) {
        Serial.println("INVALID VALUE");
        return;
      }
      radio.setAddress(value);
      break;
    }

    case 'P': {
#ifdef ENABLE_ATC
      if (numtokens < 2) return;
      int value = atoi(tokens[1]);
      DEBUG1_VALUELN("* Setting auto power:", value)
      radio.enableAutoPower(value);
#endif
      break;
    }
  }
}