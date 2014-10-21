#include <SoftwareSerial.h>

SoftwareSerial rs485 (2, 3);  // receive pin, transmit pin

#define ENABLE_PIN 4
#define LED_PIN 13

void setup()
{
  Serial.begin(9600);
  
  rs485.begin (28800);

  pinMode (LED_PIN, OUTPUT);  // driver output enable

  pinMode (ENABLE_PIN, OUTPUT);  // driver output enable
}

int i = 0;
boolean b = false;

void loop() {
  i++;

  digitalWrite(ENABLE_PIN, HIGH);
  rs485.println(i);
  digitalWrite(ENABLE_PIN, LOW);

  Serial.print("Sent ");
  Serial.println(i);

  b = !b;
  digitalWrite(LED_PIN, b);

  delay(1000);
}
