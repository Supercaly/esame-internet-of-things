#ifndef MULTIPLEXER_H
#define MULTIPLEXER_H

#include "Arduino.h"

// Class for driving the CD74HC4067 16-channel analog multiplexer
class Multiplexer
{
public:
    // Create an instance of the multiplexer with given pins.
    // NOTE: pin s3 is omitted since in the circuit is always pulled-down.
    Multiplexer(int en, int sig, int s0, int s1, int s2);
    ~Multiplexer();

    // Setup the multiplexer
    void begin();

    // Read the analog input from sensor at given index
    int read_analog(int idx);

private:
    int _en;
    int _sig;
    int _s0, _s1, _s2;
};

#endif // MULTIPLEXER_H