/*
 * This is based on the Gateway examples sketch from the RFM69 library:
 *    https://github.com/LowPowerLab/RFM69/tree/master/Examples/Gateway
 */

#include <RFM69.h>
#include <RFM69_ATC.h>
#include <SPI.h>
#include <SerialCLI.h>

//*********************************************************************************************
//************ IMPORTANT SETTINGS - YOU MUST CHANGE/CONFIGURE TO FIT YOUR HARDWARE *************
//*********************************************************************************************
#define NODEID        1    //unique for each node on same network
#define NETWORKID     100  //the same on all nodes that talk to each other
//Match frequency to the hardware version of the radio on your Moteino (uncomment one):
//#define FREQUENCY     RF69_433MHZ
//#define FREQUENCY     RF69_868MHZ
#define FREQUENCY     RF69_915MHZ
#define ENCRYPTKEY    "sampleEncryptKey" //exactly the same 16 characters/bytes on all nodes!
#define IS_RFM69HW    //uncomment only for RFM69HW! Leave out if you have RFM69W!
#define ENABLE_ATC    //comment out this line to disable AUTO TRANSMISSION CONTROL
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
}

byte ackCount=0;
uint32_t packetCount = 0;
void loop() {
  serialcli.checkSerial();

  if (radio.receiveDone())
  {
    Serial.print("#[");
    Serial.print(++packetCount);
    Serial.print(']');
    Serial.print('[');Serial.print(radio.SENDERID, DEC);Serial.print("] ");
    if (promiscuousMode)
    {
      Serial.print("to [");Serial.print(radio.TARGETID, DEC);Serial.print("] ");
    }
    for (byte i = 0; i < radio.DATALEN; i++)
      Serial.print((char)radio.DATA[i]);
    Serial.print("   [RX_RSSI:");Serial.print(radio.RSSI);Serial.print("]");

    if (radio.ACKRequested())
    {
      byte theNodeID = radio.SENDERID;
      radio.sendACK();
      Serial.print(" - ACK sent.");

      // When a node requests an ACK, respond to the ACK
      // and also send a packet requesting an ACK (every 3rd one only)
      // This way both TX/RX NODE functions are tested on 1 end at the GATEWAY
      if (ackCount++%3==0)
      {
        Serial.print(" Pinging node ");
        Serial.print(theNodeID);
        Serial.print(" - ACK...");
        Blink(LED_SEND, 3);
        delay(3); //need this when sending right after reception .. ?
        if (radio.sendWithRetry(theNodeID, "ACK TEST", 8, 0))  // 0 = only 1 attempt, no retries
          Serial.print("ok!");
        else Serial.print("nothing");
      }
    }
    Serial.println();
    Blink(LED_RECEIVE, 3);
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
  }
}