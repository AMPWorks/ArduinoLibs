/*
 * This is based on the Node examples sketch from the RFM69 library:
 *    https://github.com/LowPowerLab/RFM69/tree/master/Examples/Node
 */

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SerialCLI.h>

//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID        2    //must be unique for each node on same network (range up to 254, 255 is used for broadcast)
#define NETWORKID     100  //the same on all nodes that talk to each other (range up to 255)
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


int transmit_period = 250; //transmit a packet to gateway so often (in ms)
char payload[] = "123 ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char buff[20];
byte sendSize=1;


void setup() {
  Serial.begin(SERIAL_BAUD);

  Serial.println("*** RFM69 Gateway starting up ***");
  delay(2000);

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

  //radio.setFrequency(919000000); //set frequency to some custom frequency

//Auto Transmission Control - dials down transmit power to save battery (-100 is the noise floor, -90 is still pretty good)
//For indoor nodes that are pretty static and at pretty stable temperatures (like a MotionMote) -90dBm is quite safe
//For more variable nodes that can expect to move or experience larger temp drifts a lower margin like -70 to -80 would probably be better
//Always test your ATC mote in the edge cases in your own environment to ensure ATC will perform as you expect
#ifdef ENABLE_ATC
  radio.enableAutoPower(-70);
#endif
}

unsigned long last_sent_ms = 0;

long lastPeriod = 0;
void loop() {
  serialcli.checkSerial();

  //check for any received packets
  if (radio.receiveDone())
  {
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.ACKRequested())
    {
      radio.sendACK();
      Serial.print(" - ACK sent");
    }
    Serial.println();
    Blink(LED_RECEIVE, 3);
  }

  if (millis() - last_sent_ms >= transmit_period) {
    last_sent_ms = millis();

    Serial.print("Sending[");
    Serial.print(sendSize);
    Serial.print("]: ");
    for(byte i = 0; i < sendSize; i++)
      Serial.print((char)payload[i]);

    if (radio.sendWithRetry(GATEWAYID, payload, sendSize))
      Serial.print(" ok!");
    else Serial.print(" nothing...");

    sendSize = (sendSize + 1) % 31;
    Serial.println();
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
" \n"
"Usage:\n"
"  h - print this help\n"
"  r - read radio registers\n"
"  E - Enable encryption\n"
"  e - Disable encryption\n"
"  p - Toggle promiscuous mode\n"
"  t - Read temperature\n"
"  d <num> - Set transmit period\n"
"  a <addr> - Set this node's address\n"
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
      Serial.print("* Setting transmit period:");
      Serial.println(value);
      if ((value < 10) || (value > 10000)) {
        Serial.println("INVALID VALUE");
        return;
      }
      transmit_period = value;
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
  }
}