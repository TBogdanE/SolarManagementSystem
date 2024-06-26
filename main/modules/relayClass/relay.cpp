#include "Relay.h"
#include <Arduino.h>

Relay::Relay(int pin, bool stat) : analogPin(pin), status(stat)
{
    // Constructor implementation if needed
}

void Relay::initialize()
{
    pinMode(analogPin, OUTPUT);
    digitalWrite(analogPin, LOW); // Ensure relay is initially off
}

void Relay::setState(bool state)
{
    digitalWrite(analogPin, state ? LOW : HIGH);
    status = state;
}

bool Relay::getState()
{
    return status;
}
