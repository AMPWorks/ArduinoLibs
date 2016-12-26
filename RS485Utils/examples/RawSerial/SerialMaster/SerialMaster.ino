#include <Arduino.h>
#include <SoftwareSerial.h>


#if 1
SoftwareSerial rs485 (2, 3);  // receive pin, transmit pin
#define ENABLE_PIN 4
#elif 0
SoftwareSerial rs485 (4, 7);  // receive pin, transmit pin
#define ENABLE_PIN 2
#elif 0
SoftwareSerial rs485 (4, 10);  // receive pin, transmit pin
#define ENABLE_PIN 11
#else
#define rs485 Serial1
#define ENABLE_PIN 23
#define HARDWARE_SERIAL
#endif

#define LED_PIN 13

void setup()
{
  Serial.begin(9600);

  rs485.begin(28800);

  pinMode (LED_PIN, OUTPUT);  // driver output enable

  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite(ENABLE_PIN, LOW);

  Serial.println("Initiating raw serial master");
}

int i = 0;
boolean b = false;

char data[] = "abcdef";

void loop() {
  i++;

  digitalWrite(ENABLE_PIN, HIGH);
  rs485.write(data);
  rs485.write(i);
  delay(10);
#ifdef HARDWARE_SERIAL
  // XXX: A delay may be required here with hardware serial, or checking registers based on the serial device: http://www.gammon.com.au/forum/?id=11428
  //delayMicroseconds(330 * (sizeof (data) + sizeof (i)));
  rs485.flush(); // Waits for the send buffer to empty
#endif
  digitalWrite(ENABLE_PIN, LOW);

  Serial.print("Sent ");
  Serial.print(data);
  Serial.println(i);

  b = !b;
  digitalWrite(LED_PIN, b);

  delay(1000);
}

