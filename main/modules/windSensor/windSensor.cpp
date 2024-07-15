#include "WindSensor.h"
#include <Arduino.h>

// Constructor
WindSensor::WindSensor(int Wpin) : _Wpin(Wpin)
{
    _Wpin = Wpin;
    pinMode(_Wpin, INPUT);
}

// Function to count rotations
void WindSensor::countRotation()
{
    rotationCount++;
}

// Function to get wind speed
float WindSensor::getWindSpeed()
{
    unsigned long currentTime = millis();
    int ane = digitalRead(_Wpin);

    if (ane == LOW)
    {
        countRotation();
        delay(10); // Debounce delay
    }

    if (currentTime - lastSampleTime >= sampleTime)
    {
        float rpm = ((float)rotationCount * 60.0 / (float)(sampleTime / 1000.0)) / 2;
        float circumference = pi * wheelDiameter;
        float speed_m_per_s = (circumference / 60.0) * rpm;

        // Reset the rotation count and sample time
        rotationCount = 0;
        lastSampleTime = currentTime;

        return speed_m_per_s;
    }

    // Return the last calculated speed if the sample time has not elapsed
    return -1.0; // Indicates that the speed hasn't been recalculated yet
}
