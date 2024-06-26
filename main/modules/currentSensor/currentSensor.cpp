#include "currentSensor.h"
#include <Arduino.h>

CurrentSensor::CurrentSensor(int CURRpin)
    : _CURRpin(CURRpin)
{
    // Constructor implementation
}

float CurrentSensor::getCurrent()
{
    int sensorValue = analogRead(_CURRpin);                                   // Read the analog input
    float volt = (sensorValue / 1024.0) * 5000;                               // Convert to mV
    float current = (volt - (zeroCurrentVoltage * 1000)) / sensorSensitivity; // Calculate current in A
    return abs(current);                                                      // Return the absolute value of the current
}
