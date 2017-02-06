/*
 * This tool sends intermittent heartbeats as well as tracking heartbeat messages
 * from other nodes.
 */

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SerialCLI.h>

#define DEBUG_LEVEL DEBUG_TRACE
#include <Debug.h>

/*******************************************************************************
 * Configurable settings, set here or via compiler flags
 */
#ifndef NODEID
#define NODEID        1
#endif

#ifndef NETWORKID
#define NETWORKID     100
#endif

#ifndef IS_RFM69HW
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#endif

#ifndef FREQUENCY
#define FREQUENCY     RF69_915MHZ // RF69_433MHZ,RF69_868MHZ,RF69_915MHZ
#endif

#ifndef ENCRYPTKEY
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#endif

#ifndef ENABLE_ATC
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
#endif
/******************************************************************************/

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

/*
 * Heartbeat structure
 */
#define HEARTBEAT_MAGIC 0xefbeadde
typedef struct {
  uint32_t magic;
  uint32_t sequence;
  byte payload[32 - sizeof (char)*4 + sizeof (uint32_t)];
} heartbeat_msg_t;
heartbeat_msg_t heartbeat;
unsigned int heartbeat_period_ms = 2000;  // Period between heartbeats

void initialize_trackers();

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

  initialize_trackers();

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
    boolean is_heartbeat = false;
    if ((radio.DATALEN == sizeof (heartbeat_msg_t)) &&
            (msg->magic == HEARTBEAT_MAGIC)) {
      DEBUG1_VALUE(" HB:", msg->sequence);
      is_heartbeat = true;
    } else {
      DEBUG1_PRINT(" Not HB");
    }

    for (byte i = 0; i < radio.DATALEN; i++) {
      DEBUG1_HEXVAL(" ", (byte)radio.DATA[i]);
    }

    DEBUG_ENDLN();

    if (is_heartbeat) {
      track_heartbeat(radio.SENDERID, msg->sequence);
    }

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

/*******************************************************************************
 * Heartbeat tracking
 */

#define NUM_RECORDS 20
#define NO_ADDRESS (uint16_t)-1
typedef struct {
  uint16_t address;
  uint16_t last_sequence;
  uint32_t last_heartbeat_ms;
  uint16_t count;
  uint16_t misses;
} heartbeat_record_t;
heartbeat_record_t records[NUM_RECORDS];
void track_heartbeat(uint16_t address, uint16_t sequence);

/* Initialize the tracking array */
void initialize_trackers() {
  for (byte i = 0; i < NUM_RECORDS; i++) {
    records[i].address = NO_ADDRESS;
  }
}

/*
 * Lookup an address in the trackere
 */
heartbeat_record_t *lookup_record(uint16_t address) {
  for (byte i = 0; i < NUM_RECORDS; i++) {
    if (records[i].address == address) {
      return &records[i];
    }
  }

  return null;
}

/*
 * Add a new node record, either in an unused slot or the oldest allocated slot
 */
heartbeat_record_t *add_record(uint16_t address) {
  for (byte i = 0; i < NUM_RECORDS; i++) {
    if (records[i].address == NO_ADDRESS) {
      return &records[i];
    }
  }

  heartbeat_record_t *oldest = &records[0];
  for (byte i = 1; i < NUM_RECORDS; i++) {
    if (records[i].address <= oldest->last_heartbeat_ms) {
      oldest = &records[i];
    }
  }
  return oldest;
}

/*
 * Add a heartbeat message to the tracking system and check for missed sequences
 */
void track_heartbeat(uint16_t address, uint16_t sequence) {
  uint32_t now = millis();

  heartbeat_record_t *record = lookup_record(address);
  if (record == null) {
    record = add_record(address);
    record->address = address;
    record->count = 0;
    record->misses = 0;
  } else {
    DEBUG1_VALUE("* Last heartbeat elapsed:", now - record->last_heartbeat_ms);
    if (record->last_sequence + 1 != sequence) {
      DEBUG1_VALUE(" Bad Sequence last:", record->last_sequence);
      record->misses++;
      DEBUG1_VALUE(" misses:", record->misses);
    }
  }
  record->last_sequence = sequence;
  record->last_heartbeat_ms = now;
  record->count++;

  DEBUG_PRINT_END();
}

void dump_tracker() {
  DEBUG1_PRINTLN("addr\tseq\telapsed\tcount\tmisses")
  for (byte i = 0; i < NUM_RECORDS; i++) {
    if (records[i].address != NO_ADDRESS) {
      DEBUG1_VALUE("*", records[i].address);
      DEBUG1_VALUE("\t", records[i].last_sequence);
      DEBUG1_VALUE("\t", millis() - records[i].last_heartbeat_ms)
      DEBUG1_VALUE("\t", records[i].count);
      DEBUG1_VALUELN("\t", records[i].misses);
    }
  }
}
/******************************************************************************/

/*******************************************************************************
 * Serial CLI handling
 */
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
         "  T - Dump out state of heartbeat trackers\n"
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
      DEBUG1_VALUELN("* Setting auto power:", value);
      radio.enableAutoPower(value);
#endif
      break;
    }

    case 'T': {
      dump_tracker();
      break;
    }
  }
}