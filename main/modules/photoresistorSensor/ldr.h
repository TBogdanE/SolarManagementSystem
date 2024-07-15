#ifndef LDR_H
#define LDR_H

class LDRSensor
{
public:
    LDRSensor(int LDRpin);
    int getRawLDRValue();
    float getLDRLux();

private:
    int _LDRpin;
};

#endif