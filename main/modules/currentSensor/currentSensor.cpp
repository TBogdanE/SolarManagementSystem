#include "currentSensor.h"

CurrentSensor::CurrentSensor(int CURRpin)
{
    _CURRpin = CURRpin;
}

int CurrentSensor::getCurrent()
{
    const int numReadings = 100; // Number of readings to average
    long total = 0;

    for (int i = 0; i < numReadings; i++)
    {
        int sensorValue = analogRead(_CURRpin); // Read the analog input
        total += sensorValue;
        delay(1); // Short delay between readings
    }

    float averageSensorValue = total / numReadings; // Average the sensor readings

    // Convert the analog reading to voltage
    float voltage = averageSensorValue * (5.0 / 1023.0);

    // Calculate the current in Amperes
    float current = (voltage - zeroCurrentVoltage) / (sensorSensitivity / 1000.0);
    return current;
}
