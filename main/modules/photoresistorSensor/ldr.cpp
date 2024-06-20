#include "ldr.h"

LDRSensor::LDRSensor(int LDRpin) {
    _LDRpin = LDRpin;
}
    
   int LDRSensor::getRawLDRValue() {
        return analogRead(_LDRpin);
    }
    
    float LDRSensor::getLDRPercentage() {
        int rawLDR = getRawLDRValue();
        float brightness =  map(rawLDR, 0, 1023, 100, 0);
        return brightness;
    }