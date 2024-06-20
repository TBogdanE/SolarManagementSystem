#ifndef CURRENTSENSOR_H
#define CURRENTSENSOR_H

class CurrentSensor
{
public:
    CurrentSensor(int CURRpin);
    int getCurrent();

private:
    int _CURRpin;
    const float sensorSensitivity = 100.0; // Sensitivity of ACS712 (e.g., 100mV per A for ACS712-05B)
    const float zeroCurrentVoltage = 2.5;
};

#endif