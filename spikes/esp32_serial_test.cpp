#include <Arduino.h>

void setup()
{
  Serial.begin(115200);
  Serial.println("start");
}

void loop()
{
  Serial.println("*");
  delay(1000);
}
