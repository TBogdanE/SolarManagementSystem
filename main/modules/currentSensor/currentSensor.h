#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

class CurrentSensor
{
public:
    CurrentSensor(int CURRpin);
    float getCurrent(); // Change to float to allow for fractional current values

private:
    int _CURRpin;
    const float sensorSensitivity = 100.0; // Sensitivity of ACS712 (e.g., 100mV per A for ACS712-05B)
    const float zeroCurrentVoltage = 2.5;  // Zero current voltage in V
};

#endif // CURRENTSENSOR_H
