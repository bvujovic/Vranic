#include <Arduino.h>

#define TEST (false)

// is it board (Wemos) or not (module without USB connection)
#define BOARD (false)

#if BOARD
#define DPRINT(s, x)    \
    Serial.print(s);    \
    Serial.print(": "); \
    Serial.println(x);
#else
#define DPRINT(s, x) ;
#endif

#if BOARD
const byte pinOut = LED_BUILTIN; // buzzer (LED_BUILTIN or D2)
#else
const byte pinOut = 4; // buzzer (LED_BUILTIN or D2)
#endif
const byte pinIn = A0;                                                // LDR
const uint64_t itvSleep = TEST ? 3 : 120;                             // (seconds) Duration of sleep.
const uint64_t usToSec = 1000000;                                     // Million microseconds = 1 second.
const uint16_t cntBeeps = 2;                                          // How many times will buzzer go off when LED/light is switched off.
const uint16_t maxWakesLightHigh = TEST ? 10 : (120 * 60) / itvSleep; // max count of wakes when ext. light (LED) is high (battery is charging)
const uint16_t maxWakesLightLow = TEST ? 5 : (60 * 60) / itvSleep;    // max count of wakes when ext. light (LED) is low (charging is done)
const int lightLevelThreshold = 256;                                  // If detected light is below this level - buzzer go off.
uint32_t rtc = 0;                                                     // 1B - low/high light, 3B - counter of equal consecutive values

void beep(ulong itvOn, ulong itvOff)
{
    digitalWrite(pinOut, BOARD ? LOW : HIGH);
    delay(itvOn);
    digitalWrite(pinOut, BOARD ? HIGH : LOW);
    delay(itvOff);
}

void setup()
{
#if TEST
    Serial.begin(115200);
    Serial.println();
#endif
    pinMode(pinOut, OUTPUT);
    pinMode(pinIn, INPUT);
    bool isLightLow = analogRead(pinIn) < lightLevelThreshold;
    if (isLightLow) // low light -> external LED is OFF (battery is charged)
        for (uint16_t i = 0; i < cntBeeps; i++)
            beep(1000, 1000);

    ESP.rtcUserMemoryRead(0, &rtc, sizeof(rtc));
    DPRINT("rtc", rtc);
    bool isLightLowOld = rtc & 0xFF000000;
    DPRINT("isLightLowOld", isLightLowOld);
    if (isLightLow == isLightLowOld)
    {
        rtc++;
        uint32_t cnt = rtc & 0x00FFFFFF;
        DPRINT("cnt", cnt);
        if ((isLightLowOld && cnt >= maxWakesLightLow) || (!isLightLowOld && cnt >= maxWakesLightHigh))
        {
            beep(3000, 10);
            ESP.deepSleep(0, RF_DISABLED);
        }
    }
    else
        rtc = isLightLow << 24;
    ESP.rtcUserMemoryWrite(0, &rtc, sizeof(rtc));
    ESP.deepSleep(itvSleep * usToSec, RF_DISABLED);
}

void loop()
{
}
