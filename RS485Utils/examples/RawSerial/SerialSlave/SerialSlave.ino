#include <SoftwareSerial.h>

SoftwareSerial rs485 (2, 3);  // receive pin, transmit pin

#define ENABLE_PIN 4
#define LED_PIN 13

void setup()
{
  Serial.begin(9600);
  
  rs485.begin(28800);

  pinMode (LED_PIN, OUTPUT);  // driver output enable

  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
  digitalWrite(ENABLE_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
}

int i = 0;
boolean b = false;

void loop() {
  boolean r = false;
  while (rs485.available()) {
    r = true;
    Serial.print(rs485.read(), HEX);
    Serial.print(" ");
    digitalWrite(LED_PIN, HIGH);
  }
  
  if (r) {
     Serial.println();
  }
  
//  if (rs485.available()) {
//    Serial.write(rs485.read());
//    digitalWrite(LED_PIN, HIGH);
//  }
  delay(250);
}
