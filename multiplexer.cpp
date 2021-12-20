#include "multiplexer.h"

Multiplexer::Multiplexer(int en, int sig, int s0, int s1, int s2) : _en(en),
                                                                    _sig(sig),
                                                                    _s0(s0),
                                                                    _s1(s1),
                                                                    _s2(s2) {}

Multiplexer::~Multiplexer() {}

void Multiplexer::begin()
{
    // Setup the pins for input/output
    pinMode(_en, INPUT);
    pinMode(_s0, OUTPUT);
    pinMode(_s1, OUTPUT);
    pinMode(_s2, OUTPUT);
}

int Multiplexer::read_analog(int idx)
{
    // Set the s bits to the correct position
    digitalWrite(_s0, (idx & (1 << 0)) >> 0 ? HIGH : LOW);
    digitalWrite(_s1, (idx & (1 << 1)) >> 1 ? HIGH : LOW);
    digitalWrite(_s2, (idx & (1 << 2)) >> 2 ? HIGH : LOW);

    // Enable reading and wait some time to allow the multiplexer
    // to settle in
    digitalWrite(_en, LOW);
    delay(10);

    // read the analog input
    int result = analogRead(_sig);

    // disable multiplexer 
    digitalWrite(_en, HIGH);

    return result;
}