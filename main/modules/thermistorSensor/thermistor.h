#ifndef THERMISTOR_H
#define THERMISTOR_H

class ThermistorSensor
{
public:
    ThermistorSensor(int THERpin);
    float readTemperature();

private:
    int _THERpin;                              // Analog pin connected to the thermistor
    const float referenceResistance = 10000.0; // Resistance of the resistor in the voltage divider
    const float referenceTemperature = 25.0;   // Reference temperature for NTC thermistor
    const float betaCoefficient = 3950.0;
};

#endif