#include <Arduino.h>
// https://randomnerdtutorials.com/esp8266-adc-reading-analog-values-with-nodemcu/

const byte pinIn = A0;
const byte pinLed = LED_BUILTIN;

void setup()
{
    pinMode(pinLed, OUTPUT);
    pinMode(pinIn, INPUT);
    Serial.begin(115200);
}

const int MAX_ANALOG = 1023;

void loop()
{
    int a = analogRead(pinIn);
    Serial.println(a);
    digitalWrite(pinLed, a > MAX_ANALOG / 2); // LED is ON when the value on A0/ADC is lower
    delay(1000);
}
