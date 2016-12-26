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
  digitalWrite(LED_PIN, LOW);

  Serial.println("Initiating raw serial slave");
}

int i = 0;
boolean b = false;

void loop() {

  boolean read = false;
  while (rs485.available()) {
    char value = rs485.read();

    Serial.print(value);
    Serial.print("-0x");
    Serial.print(value, HEX);
    Serial.print(" ");
    digitalWrite(LED_PIN, HIGH);

    read = true;
  }
  
  if (read) {
     Serial.println();
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  delay(250);
}
