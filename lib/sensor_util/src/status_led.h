#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "Arduino.h"

#define STATUS_LED_PIN D8

class Status_Led
{
public:
    void begin();
    void blink(int times=1);
};

#endif // STATUS_LED_H
