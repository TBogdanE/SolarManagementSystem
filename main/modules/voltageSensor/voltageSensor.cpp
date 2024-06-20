#include "voltageSensor.h"

VoltageSensor::VoltageSensor(int Vpin)
{
    _Vpin = Vpin;
}

float VoltageSensor::getVoltage()
{
    int sensorValue = analogRead(_Vpin); // Read the analog input

    // Calculate the voltage at the analog pin
    float voltage = sensorValue * (referenceVoltage / 1023.0);

    // Calculate the input voltage using the voltage divider formula
    float inputVoltage = voltage * ((R1 + R2) / R2);
    return inputVoltage;
}