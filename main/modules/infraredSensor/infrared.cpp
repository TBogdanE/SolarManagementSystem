#include "infrared.h"

InfraredSensor::InfraredSensor(int IRpin)
{
    _IRpin = IRpin;
}

float InfraredSensor::getWindSpeed()
{
    unsigned long previousTime = 0;
    unsigned long currentTime = 0;
    float deltaTimeSeconds = 0.0;
    float rotationSpeed = 0.0;

    if (digitalRead(_IRpin) == HIGH)
    {
        // Record current time
        currentTime = millis();

        // Calculate time difference
        deltaTimeSeconds = (currentTime - previousTime) / 1000.0;

        // Calculate rotation speed (RPS)
        rotationSpeed = 1.0 / deltaTimeSeconds;

        // Convert to RPM if needed
        float rotationSpeedRPM = rotationSpeed * 60.0;

        // Update previous time
        previousTime = currentTime;

        return rotationSpeedRPM;
    }
}