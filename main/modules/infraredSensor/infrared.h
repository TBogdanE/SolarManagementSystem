#ifndef INFRARED_H
#define INFRARED_H

class InfraredSensor
{
public:
    InfraredSensor(int IRpin);
    float getWindSpeed();

private:
    int _IRpin;
};
#endif