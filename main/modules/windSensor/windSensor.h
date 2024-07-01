#ifndef WINDSENSOR_H
#define WINDSENSOR_H

class WindSensor
{
public:
    WindSensor(int Wpin);
    float getWindSpeed();

private:
    int _Wpin;
    const int hallSensorPin = 5;     // YS-27 Hall effect sensor pin
    volatile int rotationCount = 0;  // Rotation count
    unsigned long sampleTime = 5000; // Sample time in milliseconds (5 seconds)
    const float wheelDiameter = 0.1; // Diameter of the wheel in meters
    const float pi = 3.14159265358979323846;
    unsigned long lastSampleTime = 0;
    void countRotation();
};

#endif
