#ifndef VOLTAGESENSOR_H
#define VOLTAGESENSOR_H

class VoltageSensor
{
public:
    VoltageSensor(int Vpin);
    float getVoltage();

private:
    int _Vpin;
    const float referenceVoltage = 5.0; // Reference voltage of the Arduino (usually 5V)
    const float R1 = 10000.0;           // Resistance of R1 in ohms (10k ohms)
    const float R2 = 5000.0;
};
#endif