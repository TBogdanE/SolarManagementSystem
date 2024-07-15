#include "ldr.h"

// Constructor
LDRSensor::LDRSensor(int LDRpin)
{
    _LDRpin = LDRpin;
}

// Function to get the raw sensor value
int LDRSensor::getRawLDRValue()
{
    return analogRead(_LDRpin);
}

// Function to convert raw sensor value to lux
float LDRSensor::getLDRLux()
{
    int rawLDR = getRawLDRValue();

    // Calibrate the LDR sensor based on your specific environment
    // These values (0.0 and 1000.0) should be adjusted based on your calibration
    float lux = map(rawLDR, 1023, 0, 0.0, 1000.0);

    return lux;
}
