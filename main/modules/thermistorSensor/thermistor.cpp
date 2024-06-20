#include "thermistor.h"

ThermistorSensor::ThermistorSensor(int THERpin)
{
    _THERpin = THERpin;
}

float ThermistorSensor::readTemperature()
{
    int rawValue = analogRead(_THERpin); // Read the raw ADC value

    float voltage = rawValue * (5.0 / 1023.0); // Convert ADC value to voltage (assuming 5V Arduino)
    float thermistorResistance = referenceResistance * (5.0 / voltage - 1.0);

    // Calculate temperature using Steinhart-Hart equation for NTC thermistors
    float steinhart;
    steinhart = log(thermistorResistance / referenceResistance); // ln(R/Ro)
    steinhart /= betaCoefficient;                                // 1/B * ln(R/Ro)
    steinhart += 1.0 / (referenceTemperature + 273.15);          // + (1/To)
    steinhart = 1.0 / steinhart;                                 // Invert
    steinhart -= 273.15;                                         // Convert to Celsius
    return steinhart;
}