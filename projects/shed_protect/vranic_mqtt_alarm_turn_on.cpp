//*

#include <Arduino.h>
#include <WiFiServerBasics.h>
#include <CredWiFi.h>
#include <PubSubClient.h>
#include <CredAdafruit.h>

WiFiClient wiFiClient;
PubSubClient mqtt(wiFiClient);
typedef unsigned int uint;
void callback(char *topic, byte *data, uint length);

#define FEED_ALARM_ON "vranic.vranic-alarm-on"
const uint SEC = 1000;
const uint MIN = 60 * SEC;
const byte pinBuzzer = D1;
const int MAX_ALARM_ON = SEC; // Maksimalno trajanje (u ms) aktiviranog alarma.

#include "Blinky.h"
Blinky led(LED_BUILTIN, false, SEC, 3);

void alarmOn(bool on)
{
    digitalWrite(pinBuzzer, on);
    led.ledOn(on);
}

// Priprema (paljenje LEDa) i pull solenoida (trajanje zadato sa ms).
void turnAlarmOn(uint ms)
{
    Serial.println("Alarm ON");
    alarmOn(true);
    delay(ms);
    alarmOn(false);
}

void reconnect()
{
    if (!WiFi.isConnected())
    {
        led.blink();
        ESP.reset();
    }
    while (!mqtt.connected())
    {
        Serial.println("MQTT connecting...");
        if (mqtt.connect("", AIO_USER, AIO_KEY))
        {
            Serial.println("Connected");
            mqtt.subscribe(AIO_USER AIO_FEEDS FEED_ALARM_ON, 0);
            led.off();
        }
        else
        {
            Serial.print("Failed. MQTT state: ");
            Serial.print(mqtt.state());
            led.blink();
        }
    }
}

void setup()
{
    pinMode(pinBuzzer, OUTPUT);
    Serial.begin(115200);
    led.on();
    WiFi.mode(WIFI_STA);
    ConnectToWiFi();
    Serial.print("\nWiFi connected, IP address: ");
    Serial.println(WiFi.localIP());
    mqtt.setServer(AIO_SERVER, AIO_SERVERPORT);
    mqtt.setCallback(callback);
}

void loop()
{
    if (!mqtt.connected())
        reconnect();
    mqtt.loop();
    delay(100);
}

void callback(char *topic, byte *data, uint length)
{
    Serial.println(topic);
    for (size_t i = 0; i < length; i++)
        Serial.printf("%c", data[i]);
    Serial.println();
    turnAlarmOn(3000);
}
