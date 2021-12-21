#include "status_led.h"

void Status_Led::begin()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
}

void Status_Led::blink(int times)
{
    for (int i = 0; i < times; ++i)
    {
        digitalWrite(STATUS_LED_PIN, HIGH);
        delay(500);
        digitalWrite(STATUS_LED_PIN, LOW);
        delay(500);
    }
}